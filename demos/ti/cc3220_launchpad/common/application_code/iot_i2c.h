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
 * @file iot_i2c.h
 * @brief File for the APIs of I2C called by application layer.
 */
#ifndef _IOT_I2C_H_
#define _IOT_I2C_H_

#include "stdint.h"
#include "stdlib.h"
#include "FreeRTOS.h"
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
#define IOT_I2C_NOT_INITIALIZED               ( 2 )
#define IOT_I2C_BUSY                          ( 3 )
#define IOT_I2C_WRITE_FAIL                    ( 4 )
#define IOT_I2C_READ_FAIL                     ( 5 )
#define IOT_I2C_NACK                          ( 6 )
#define IOT_I2C_FUNCTION_NOT_SUPPORTED        ( 7 )
#define IOT_I2C_BUS_IDLE                      ( 8 )


typedef struct
{
    uint32_t ulMasterTimeout;
    uint32_t ulBusFreq;
} IotI2CIoctlConfig_t;

typedef enum
{
    eI2CSendStop,           /**< Send the stop condition on the bus. */
    eI2CSetSlaveAddrWrite,  /**< Master sends the stop condition followed by slave address to Write. */
    eI2CSetSlaveAddrRead,   /**< Master sends the stop condition followed by slave address to Read.  */
    eI2CSetMasterConfig,    /**< Sets the I2C bus frequency and timeout using the struct IotI2CIoctlConfig_t,
                              *   default speed is Standard, fast, fast-plus and High speed. */
    eI2CGetMasterConfig,    /**< Gets the I2C bus frequency and timeout set for the I2C master. */
    eI2CGetBusState,        /**< Gets the mode from one of the error codes. */
    eI2CClearBusReset,      /**< Master resets the bus by sending 9 clock signals. */
    eI2CGetTxNoOfbytes,     /** Get the number of bytes sent in write operation. */
    eI2CGetRxNoOfbytes,     /** Get the number of bytes received in read operation. */
} IotI2CIoctlRequest_t;

/**
 * @brief The I2C descriptor type defined in the source file.
 */
struct IotI2CDescriptor;

/**
 * @brief IotI2CHandle_t is the handle type returned by calling iot_i2c_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.syst
 */
typedef struct IotI2CDescriptor * IotI2CHandle_t;

/**
 * @brief The callback function for completion of I2C operation.
 */
typedef void (* IotI2CCallback_t) ( void * pvI2Cparam );

/**
 * @brief Initiates the I2C bus as master.
 *
 * @lI2CInstance The instance of I2C to initialize.
 *
 * @return The handle to the I2C port if SUCCESS else NULL.
 */
IotI2CHandle_t iot_i2c_open( int32_t lI2CInstance );

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * @param[in] pxI2CPeripheral The I2C peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 */
void iot_i2c_set_completion_callback( IotI2CHandle_t const pxI2CPeripheral,
                                      IotI2CCallback_t xCallback );

/**
 * @brief Starts the I2C master read operation in blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return ACK = IOT_I2C_SUCCESS,
 *         NACK = IOT_I2C_NACK,
 *         Driver_Error = IOT_I2C_READ_FAIL,
 *         Not initialized = IOT_I2C_NOT_INITIALIZED
 */
int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes );

/**
 * @brief Starts the I2C master write operation in blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pvBuffer The transmit buffer containing the data to be written.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return ACK = IOT_I2C_SUCCESS,
 *         NACK = IOT_I2C_NACK,
 *         Driver_Error = IOT_I2C_WRITE_FAIL,
 *         Not initialized = IOT_I2C_NOT_INITIALIZED
 */
int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the I2C master read operation in non-blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE,
 *         Not initialized = IOT_I2C_NOT_INITIALIZED
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
 *         FAILED = IOT_I2C_INVALID_VALUE,
 *         Not initialized = IOT_I2C_NOT_INITIALIZED
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
 *         BUS IDLE(For eGetBusState ) = IOT_I2C_BUS_IDLE,
 *         Busy(For eGetBusState, eSetSlaveAddrWrite/Read, eSendStop) = IOT_I2C_BUSY,
 *         Not initialized = IOT_I2C_NOT_INITIALIZED
 */
int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral,
                       IotI2CIoctlRequest_t xI2CRequest,
                       void * const pvBuffer );


/**
 * @brief Stops the ongoing operation and de-initializes the I2C peripheral.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         Not initialized = IOT_I2C_NOT_INITIALIZED
 */
int32_t iot_i2c_close( IotI2CHandle_t const pxI2CPeripheral );

/**
 * @brief This function is used to cancel the current operation in progress
 * if possible.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *          FAILED = IOT_I2C_FUNCTION_NOT_SUPPORTED,
 *          Bus_IDLE = IOT_I2C_BUS_IDLE,
 *          Not initialized = IOT_I2C_NOT_INITIALIZED
 */
int32_t iot_i2c_cancel( IotI2CHandle_t const pxI2CPeripheral );

/**
 * @}
 */
/* end of group iot_i2c */

#endif /* ifndef _IOT_I2C_H_ */
