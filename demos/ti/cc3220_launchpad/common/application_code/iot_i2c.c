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
#include "iot_i2c.h"
#include "Board.h"
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
        bool busy;
//        bool transactionInProgress; toggle in the callback
} IotI2CDescriptor_t;

#define I2C_INSTANCES (2)

IotI2CDescriptor_t i2cInstances[I2C_INSTANCES] = {0, NULL, {I2C_MODE_BLOCKING}, {0}, NULL, NULL, false};
/**
 * The internal callback from native driver implementation.
 */
static void I2C_CallbackInternal(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus)
{
        IotI2CHandle_t pDescriptor = NULL;

        uint32_t i = 0;

        for (i = 0; i < I2C_INSTANCES; i++)
        {
                if (i2cInstances[i].handle == handle)
                {
                        pDescriptor = &i2cInstances[i];
                }
        }

        if (pDescriptor != NULL && pDescriptor->userCallback)
        {
                pDescriptor->userCallback(NULL, pDescriptor->userCallback);
        }
}

/**
 * Translated between frequency and internal representation by picking the closest floor value.
 */
static I2C_BitRate FrequencyToBitRate(uint32_t frequency);

/**
 * @brief Initiates the I2C bus as master.
 *
 * @lI2CInstance The instance of I2C to initialize.
 *
 * @return The handle to the I2C port if SUCCESS else NULL.
 */
IotI2CHandle_t iot_i2c_open(int32_t I2CInstance)
{
        IotI2CHandle_t pDescriptor;

        if (I2CInstance > I2C_INSTANCES)
        {
                pDescriptor = NULL;
        }

        pDescriptor = &i2cInstances[I2CInstance];

        pDescriptor->instance = I2CInstance;
        pDescriptor->params.transferCallbackFxn = I2C_CallbackInternal;
        pDescriptor->params.bitRate = I2C_100kHz;
        pDescriptor->params.transferMode = I2C_MODE_CALLBACK;
        pDescriptor->params.custom = NULL;
        pDescriptor->busy = false;

        return &i2cInstances[I2CInstance];
}

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * @param[in] pxI2CPeripheral The I2C peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 */
void iot_i2c_set_callback(IotI2CHandle_t const pxI2CPeripheral,
                          IotI2CCallback_t xCallback,
                          void *pvUserContext)
{
        IotI2CHandle_t pDescriptor = pxI2CPeripheral;

        pDescriptor->userCallback = xCallback;
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
int32_t iot_i2c_read_sync(IotI2CHandle_t const pxI2CPeripheral,
                          uint8_t *const pvBuffer,
                          size_t xBytes)
{
        bool status = false;
        int32_t readStatus = IOT_I2C_READ_FAILED;

        pxI2CPeripheral->transaction.writeBuf = NULL;
        pxI2CPeripheral->transaction.writeCount = 0;
        pxI2CPeripheral->transaction.readBuf = pvBuffer;
        pxI2CPeripheral->transaction.readCount = xBytes;

        status = I2C_transfer(pxI2CPeripheral->handle, &pxI2CPeripheral->transaction);

        if (status == false)
        {
                readStatus = IOT_I2C_NACK;
        }
        else
        {
                readStatus = IOT_I2C_SUCCESS;
        }

        return readStatus;
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
int32_t iot_i2c_write_sync(IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t *const pvBuffer,
                           size_t xBytes)
{
        bool status = false;
        uint8_t *writeBuffer = pvBuffer;
        int32_t writeStatus = IOT_I2C_WRITE_FAILED;

        pxI2CPeripheral->transaction.writeBuf = writeBuffer;
        pxI2CPeripheral->transaction.writeCount = xBytes;
        pxI2CPeripheral->transaction.readBuf = NULL;
        pxI2CPeripheral->transaction.readCount = 0;

        status = I2C_transfer(pxI2CPeripheral->handle, &pxI2CPeripheral->transaction);
        //semaphore to wait until done and then return

        if (status == false)
        {
                writeStatus = IOT_I2C_NACK;
        }
        else
        {
                writeStatus = IOT_I2C_SUCCESS;
        }

        return writeStatus;
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
int32_t iot_i2c_read_async(IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t *const pvBuffer,
                           size_t xBytes)
{
        bool status = false;
        int32_t readStatus = IOT_I2C_READ_FAILED;

        pxI2CPeripheral->transaction.writeBuf = NULL;
        pxI2CPeripheral->transaction.writeCount = 0;
        pxI2CPeripheral->transaction.readBuf = pvBuffer;
        pxI2CPeripheral->transaction.readCount = xBytes;
        status = I2C_transfer(pxI2CPeripheral->handle, &pxI2CPeripheral->transaction);
        if (status == false)
        {
                readStatus = IOT_I2C_NACK;
        }
        else
        {
                readStatus = IOT_I2C_SUCCESS;
        }

        return readStatus;
}

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
int32_t iot_i2c_write_async(IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t *const pvBuffer,
                            size_t xBytes)
{
        bool status = false;
        int32_t writeStatus = IOT_I2C_WRITE_FAILED;
        uint8_t *writeBuffer = pvBuffer;

        pxI2CPeripheral->transaction.writeBuf = writeBuffer;
        pxI2CPeripheral->transaction.writeCount = xBytes;
        pxI2CPeripheral->transaction.readBuf = NULL;
        pxI2CPeripheral->transaction.readCount = 0;

        status = I2C_transfer(pxI2CPeripheral->handle, &pxI2CPeripheral->transaction);

        if (status == false)
        {
                writeStatus = IOT_I2C_NACK;
        }
        else
        {
                writeStatus = IOT_I2C_SUCCESS;
        }

        return writeStatus;
}

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
int32_t iot_i2c_ioctl(IotI2CHandle_t const pxI2CPeripheral,
                      IotI2CIoctlRequest_t xI2CRequest,
                      void *const pvBuffer)
{
        IotI2CDescriptor_t *pDescriptor = (IotI2CDescriptor_t *)pxI2CPeripheral;

        int32_t ioctlStatus = IOT_I2C_FUNCTION_NOT_SUPPORTED;

        switch (xI2CRequest)
        {
            case eI2CSetMasterConfig:
            {
                    IotI2CConfig_t *config = (IotI2CConfig_t *)pvBuffer;

                    pDescriptor->params.bitRate = FrequencyToBitRate(config->ulBusFreq);

                    I2C_Handle i2cHandle = NULL;

                    if (pDescriptor->busy == false)
                    {
                            i2cHandle = I2C_open(pDescriptor->instance, &pDescriptor->params);

                            if (i2cHandle != NULL)
                            {
                                    pDescriptor->handle = i2cHandle;
                            }

                            pDescriptor->busy = true;

                            ioctlStatus = IOT_I2C_SUCCESS;
                    }
                    else if (pxI2CPeripheral != NULL)
                    {
                            pDescriptor->handle = pxI2CPeripheral->handle;

                            ioctlStatus = IOT_I2C_SUCCESS;
                    }
            }
            break;
            case eI2CSetSlaveAddrWrite:
            case eI2CSetSlaveAddrRead:
            {
                uint8_t *address = (uint8_t *)pvBuffer;

                pDescriptor->transaction.slaveAddress = (uint_least8_t)*address;

                ioctlStatus = IOT_I2C_SUCCESS;
            }
            break;
            case eI2CSendNoStopFlag:
            {

            }
            break;
            default:
            {
                break;
            }
        }

        return ioctlStatus;
}

/**
 * @brief Stops the ongoing operation and de-initializes the I2C peripheral.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE,
 */
int32_t iot_i2c_close(IotI2CHandle_t const pxI2CPeripheral)
{
        I2C_close(pxI2CPeripheral->handle);

        return IOT_I2C_SUCCESS;
}

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
int32_t iot_i2c_cancel(IotI2CHandle_t const pxI2CPeripheral)
{
        return IOT_I2C_INVALID_VALUE;
}
/**
 * @}
 */
/* end of group iot_i2c */

/*
 * -----------------------------------------------------------------------------------
 */

static I2C_BitRate FrequencyToBitRate(uint32_t frequency)
{
        I2C_BitRate ebitRate;

        if (frequency >= 1000000)
        {
                ebitRate = I2C_1000kHz;
        }
        if (frequency >= 400000)
        {
                ebitRate = I2C_400kHz;
        }

        return ebitRate;
}

void setFrequency(IotI2CHandle_t const pxI2CPeripheral,
                  void *const pvBuffer)
{
        IotI2CDescriptor_t *pDescriptor = (IotI2CDescriptor_t *)pxI2CPeripheral;

        IotI2CConfig_t *config = (IotI2CConfig_t *)pvBuffer;

        pDescriptor->params.bitRate = FrequencyToBitRate(config->ulBusFreq);
}
