/*
 * Amazon FreeRTOS V1.2.3
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file iot_i2c.c
 * @brief file containing the implementation of UART APIs calling STM drivers.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <ti/drivers/I2C.h>

#define MOIRA_ASSERT(condition) if(!condition) { printf( "ASSEERT!!!!!"); while(1) {} }
/**
 * @defgroup iot_i2c I2C Abstraction APIs.
 * @{
 */

/**
 * The speeds supported by I2C bus.
 */
#define IOT_I2C_STANDARD_MODE_BPS         ( 100000 )
#define IOT_I2C_FAST_MODE_BPS             ( 400000 )
#define IOT_I2C_FAST_MODE_PLUS_BPS        ( 1000000 )
#define IOT_I2C_HIGH_SPEED_BPS            ( 3400000 )

/**
 * The return codes for the functions in I2C.
 */
#define IOT_I2C_SUCCESS                       ( 0 )
#define IOT_I2C_INVALID_VALUE                 ( 1 )
#define IOT_I2C_BUSY                          ( 2 )
#define IOT_I2C_WRITE_FAILED                  ( 3 )
#define IOT_I2C_READ_FAILED                   ( 4 )
#define IOT_I2C_NACK                          ( 5 )
#define IOT_I2C_BUS_TIMEOUT                   ( 6 )
#define IOT_I2C_NOTHING_TO_CANCEL             ( 7 )
#define IOT_I2C_FUNCTION_NOT_SUPPORTED        ( 8 )

/**
 * @brief I2C Bus status
 */
typedef enum
{
        eI2CBusIdle = 0,
        eI2cBusBusy = IOT_I2C_BUSY,
} IotI2CBusStatus_t;

/**
 * @brief I2C operation status.
 */
typedef enum
{
        eI2CCompleted = IOT_I2C_SUCCESS,
        eI2CNackFromSlave = IOT_I2C_NACK,
        eI2CMasterTimeout = IOT_I2C_BUS_TIMEOUT,

} IotI2COperationStatus_t;

/**
 * @brief I2C bus configuration
 */
typedef struct IotI2CConfig
{
        uint32_t ulMasterTimeout; /**<! Master timeout value in msec, to relinquish the bus if slave
                                      does not respond */
        uint32_t ulBusFreq;     /**<! Bus frequency/baud rate */
} IotI2CConfig_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
        eI2CSendStop,       /**<! Send the stop condition on the bus. */
        eI2CSetSlaveAddrWrite, /**<! Master sends the start condition followed by slave address to Write. */
        eI2CSetSlaveAddrRead, /**<! Master sends the start condition followed by slave address to Read.  */
        eI2CSetMasterConfig, /**<! Sets the I2C bus frequency and timeout using an instance of type IotI2CConfig_t,
                              *   default speed is Standard, fast, fast-plus and High speed. */
        eI2CGetMasterConfig, /**<! Gets the I2C bus frequency and timeout set for the I2C master. */
        eI2CGetBusState,    /**<! Get the current I2C bus status. Returns eI2CBusIdle or eI2CBusy */
        eI2CBusReset,       /**<! Master resets the bus by sending 9 clock signals. */
        eI2CGetTxNoOfbytes, /**<! Get the number of bytes sent in write operation. */
        eI2CGetRxNoOfbytes, /**<! Get the number of bytes received in read operation. */
} IotI2CIoctlRequest_t;

/**
 * @brief IotI2CHandle_t is the handle type returned by calling iot_i2c_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotI2CDescriptor * IotI2CHandle_t;

/**
 * @brief The callback function for completion of I2C operation.
 *
 * @param[out] xOpStatus    I2C asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef void (* IotI2CCallback_t) ( IotI2COperationStatus_t xOpStatus, void * pvUserContext );

/**
 * @brief The I2C descriptor type defined in the source file.
 */
typedef struct IotI2CDescriptor
{
        uint32_t instance;
        I2C_Handle handle;
        I2C_Params params;
        I2C_Transaction transaction;
        IotI2CCallback_t userCallback;
        void * userContext;
        bool busy;
} IotI2CDescriptor_t;

#define I2C_INSTANCES 2

IotI2CDescriptor_t i2cInstances[ I2C_INSTANCES ] = { { 0 }, { 0 } };

/**
 * The internal callback from native driver implementation.
 */
static void I2C_CallbackInternal(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus)
{
        IotI2CDescriptor_t * pDescriptor = NULL;

        // Find the correct descriptor
        //int32_t i = 0;
        for( uint32_t i = 0; i < I2C_INSTANCES; ++i )
        {
                if( i2cInstances[ i ].handle == handle )
                {
                        pDescriptor = &i2cInstances[ i ];
                }
        }

        if( pDescriptor != NULL )
        {
                pDescriptor->userCallback( eI2CCompleted, pDescriptor->userContext );
        }
}

/**
 * Translated between frequency and internal representation by picking the closest floor value.
 */
static I2C_BitRate FrequencyToBitRate( uint32_t frequency );

/**
 * @brief Initiates the I2C bus as master.
 *
 * @lI2CInstance The instance of I2C to initialize.
 *
 * @return The handle to the I2C port if SUCCESS else NULL.
 */
IotI2CHandle_t iot_i2c_open( int32_t I2CInstance )
{
        IotI2CDescriptor_t * pDescriptor;
        I2C_Handle i2cHandle = NULL;
        I2C_init();

        if( I2CInstance > I2C_INSTANCES )
        {
                return NULL;
        }

        pDescriptor = &i2cInstances[ I2CInstance ];

        if( pDescriptor->busy == true )
        {
                return NULL;
        }

        pDescriptor->instance = I2CInstance;

        pDescriptor->params.transferCallbackFxn = I2C_CallbackInternal;
        pDescriptor->params.bitRate = I2C_100kHz;
        pDescriptor->params.transferMode = I2C_MODE_CALLBACK;
        pDescriptor->params.custom = NULL;

        pDescriptor->busy = true;


        return pDescriptor;
}

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * @param[in] pxI2CPeripheral The I2C peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_i2c_set_callback( IotI2CHandle_t const pxI2CPeripheral,
                           IotI2CCallback_t xCallback,
                           void * pvUserContext )
{
        IotI2CDescriptor_t * pDescriptor = pxI2CPeripheral;

        pDescriptor->userCallback = xCallback;
        pDescriptor->userContext = pvUserContext;
}

/**
 * @brief Starts the I2C master read operation in blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return ACK = IOT_I2C_SUCCESS,
 *         NACK = IOT_I2C_NACK,
 *         Driver_Error = IOT_I2C_READ_FAILED,
 */
int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes )
{
        I2C_Transaction i2cTransaction = {0};
        i2cTransaction.slaveAddress = pxI2CPeripheral->transaction.slaveAddress;
        i2cTransaction.slaveAddress = pxI2CPeripheral->transaction.slaveAddress;
        i2cTransaction.writeBuf = NULL;
        i2cTransaction.writeCount = 0;
        i2cTransaction.readBuf = pvBuffer;
        i2cTransaction.readCount = xBytes;
        bool status = I2C_transfer(pxI2CPeripheral->handle, &i2cTransaction);
        if (status == false) {
                // Unsuccessful I2C transfer
                return IOT_I2C_NACK;
        } else{
                return IOT_I2C_SUCCESS;
        }
}

/**
 * @brief Starts the I2C master write operation in blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pvBuffer The transmit buffer containing the data to be written.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return ACK = IOT_I2C_SUCCESS,
 *         NACK = IOT_I2C_NACK,
 *         Driver_Error = IOT_I2C_WRITE_FAILED,
 *
 */
int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
        I2C_Transaction i2cTransaction = {0};
        uint8_t writeBuffer[xBytes];
        int i;
        for (i = 0; i < xBytes; i++) {
                writeBuffer[i] = pvBuffer[i];
        }
        i2cTransaction.slaveAddress = pxI2CPeripheral->transaction.slaveAddress;
        i2cTransaction.writeBuf = writeBuffer;
        i2cTransaction.writeCount = xBytes;
        i2cTransaction.readBuf = NULL;
        i2cTransaction.readCount = 0;
        bool status = I2C_transfer(pxI2CPeripheral->handle, &i2cTransaction);
        if (status == false) {
                // Unsuccessful I2C transfer
                return IOT_I2C_NACK;
        } else{
                return IOT_I2C_SUCCESS;
        }
}

/**
 * @brief Starts the I2C master read operation in non-blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE
 */
int32_t iot_i2c_read_async( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the I2C master write operation in non-blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pvBuffer The transmit buffer containing the data to be written.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE
 */
int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief Configures the I2C master with user configuration.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] xI2CRequest Should be one of I2C_Ioctl_Request_t.
 * @param[in,out] pvBuffer The configuration values for the IOCTL request.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_FUNCTION_NOT_SUPPORTED,
 *         Busy(For eGetBusState, eSetSlaveAddrWrite/Read, eSendStop) = IOT_I2C_BUSY,
 */
int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral,
                       IotI2CIoctlRequest_t xI2CRequest,
                       void * const pvBuffer )
{
        IotI2CDescriptor_t * pDescriptor = pxI2CPeripheral;

        switch(xI2CRequest)
        {
        case eI2CSetMasterConfig:
        {
                IotI2CConfig_t * config = (IotI2CConfig_t*)pvBuffer;

                pDescriptor->params.bitRate = FrequencyToBitRate( config->ulBusFreq );

                I2C_Handle i2cHandle = I2C_open( pDescriptor->instance, &pDescriptor->params );

                if(i2cHandle == NULL)
                {
                        return IOT_I2C_INVALID_VALUE;
                }

                pDescriptor->handle = i2cHandle;
        }
        break;

        case eI2CSetSlaveAddrWrite:
        {
                uint8_t * address = (uint8_t *)pvBuffer;

                pDescriptor->transaction.slaveAddress = *address;
        }
        break;
        case eI2CSetSlaveAddrRead:
        {
                uint8_t * address = (uint8_t *)pvBuffer;

                pDescriptor->transaction.slaveAddress = *address;
        }
        break;

        default:
                MOIRA_ASSERT( 0 );
                break;
        }

        return IOT_I2C_SUCCESS;
}


/**
 * @brief Stops the ongoing operation and de-initializes the I2C peripheral.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE,
 */
int32_t iot_i2c_close( IotI2CHandle_t const pxI2CPeripheral );

/**
 * @brief This function is used to cancel the current operation in progress
 * if possible.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *          FAILED = IOT_I2C_FUNCTION_INVALID_VALUE
 *          or IOT_I2C_FUNCTION_NOT_SUPPORTED, IOT_I2C_NOTHING_TO_CANCEL
 */
int32_t iot_i2c_cancel( IotI2CHandle_t const pxI2CPeripheral );

/**
 * @}
 */
/* end of group iot_i2c */

/*
 * -----------------------------------------------------------------------------------
 */

static I2C_BitRate FrequencyToBitRate( uint32_t frequency )
{
        if(frequency >= 1000000)
        {
                return I2C_1000kHz;
        }
        if(frequency >= 400000)
        {
                return I2C_400kHz;
        }

        return I2C_100kHz;
}

