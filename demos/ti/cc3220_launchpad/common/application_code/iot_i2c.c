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

#include "iot_i2c.h"
#include "Board.h"
#include <unistd.h>

typedef struct IotI2CDescriptor
{
    uint32_t instance;
    I2C_Handle handle;
    I2C_Params params;
    I2C_Transaction transaction;
    IotI2CCallback_t userCallback;
    void *userContext;
    bool busy;
    volatile bool transactionDone;

} IotI2CDescriptor_t;

#define I2C_INSTANCES (2)

IotI2CDescriptor_t i2cInstances[I2C_INSTANCES] = {0, NULL, {I2C_MODE_CALLBACK}, {0}, NULL, NULL, false, false};

//If the driver wants to make reads/writes synchronously, set the parameter "pDescriptor->params.transferCallbackFxn" to be = I2C_Blocking_Callback. This callback will toggle the boolean within this file to force the sync functions to wait until the transfer is done.
void I2C_Blocking_Callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus)
{
    IotI2CHandle_t pDescriptor = NULL;

    for (int i = 0; i < I2C_INSTANCES; i++)
    {
        if (i2cInstances[i].handle == handle)
        {
            pDescriptor = &i2cInstances[i];
        }
    }

    if (pDescriptor != NULL)
    {
        pDescriptor->transactionDone = true;
    }
}

//If the driver wants to make reads/writes asynchronously, set the parameter "pDescriptor->params.transferCallbackFxn" to be = I2C_CallbackInternal. This callback will call the callback in the driver upon completing the transfer.
static void I2C_CallbackInternal(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus)
{
    IotI2CHandle_t pDescriptor = NULL;

    for (int i = 0; i < I2C_INSTANCES; i++)
    {
        if (i2cInstances[i].handle == handle)
        {
            pDescriptor = &i2cInstances[i];
        }
    }

    if (pDescriptor != NULL && pDescriptor->userCallback)
    {
        pDescriptor->userCallback(transferStatus == true ? eI2CCompleted : eI2CMasterTimeout, pDescriptor->userContext);
    }
}

static I2C_BitRate FrequencyToBitRate(uint32_t frequency);

IotI2CHandle_t iot_i2c_open(int32_t I2CInstance)
{
    IotI2CHandle_t pDescriptor;

    if (I2CInstance > I2C_INSTANCES)
    {
        pDescriptor = NULL;
    }

    pDescriptor = &i2cInstances[I2CInstance];

    pDescriptor->instance = I2CInstance;
    pDescriptor->params.transferCallbackFxn = I2C_Blocking_Callback;
    pDescriptor->params.bitRate = I2C_100kHz;
    pDescriptor->params.transferMode = I2C_MODE_CALLBACK;
    pDescriptor->params.custom = NULL;
    pDescriptor->busy = false;

    return &i2cInstances[I2CInstance];
}

void iot_i2c_set_callback(IotI2CHandle_t const pxI2CPeripheral,
                          IotI2CCallback_t xCallback,
                          void *pvUserContext)
{
    IotI2CHandle_t pDescriptor = pxI2CPeripheral;

    pDescriptor->userCallback = xCallback;

    pDescriptor->userContext = pvUserContext;
}

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
    pxI2CPeripheral->transactionDone = false;

    //Save callback/context to install a new callback/context. When the new callback completes, we will reinstall the previous one. The implication is that this API is not thread safe, which is nevertheless the contract of all of Common IO's API.
    I2C_CallbackFxn oldCallback = pxI2CPeripheral->params.transferCallbackFxn;
    pxI2CPeripheral->params.transferCallbackFxn = I2C_Blocking_Callback;

    status = I2C_transfer(pxI2CPeripheral->handle, &pxI2CPeripheral->transaction);

    while (!pxI2CPeripheral->transactionDone)
    {
        sleep(1);
    }

    if (status == false)
    {
        readStatus = IOT_I2C_NACK;
    }

    else
    {
        readStatus = IOT_I2C_SUCCESS;
    }

    pxI2CPeripheral->transactionDone = false;
    pxI2CPeripheral->params.transferCallbackFxn = oldCallback;

    return readStatus;
}

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
    pxI2CPeripheral->transactionDone = false;

    //Save callback/context to install a new callback/context. When the new callback completes, we will reinstall the previous one. The implication is that this API is not thread safe, which is nevertheless the contract of all of Common IO's API.
    I2C_CallbackFxn oldCallback = pxI2CPeripheral->params.transferCallbackFxn;
    pxI2CPeripheral->params.transferCallbackFxn = I2C_Blocking_Callback;

    status = I2C_transfer(pxI2CPeripheral->handle, &pxI2CPeripheral->transaction);

    while (!pxI2CPeripheral->transactionDone)
    {
        sleep(1);
    }

    if (status == false)
    {
        writeStatus = IOT_I2C_NACK;
    }

    else
    {
        writeStatus = IOT_I2C_SUCCESS;
    }

    pxI2CPeripheral->transactionDone = false;
    pxI2CPeripheral->params.transferCallbackFxn = oldCallback;

    return writeStatus;
}

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

int32_t iot_i2c_close(IotI2CHandle_t const pxI2CPeripheral)
{
    I2C_close(pxI2CPeripheral->handle);

    return IOT_I2C_SUCCESS;
}

int32_t iot_i2c_cancel(IotI2CHandle_t const pxI2CPeripheral)
{
    return IOT_I2C_INVALID_VALUE;
}

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
