
/*
 * Wire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */
/* Standard includes. */

//MOIRA'S IMPLEMENTATION OF WIRE API, IN BETWEEN ARDUINO AND AFR I2C
extern "C"
{
#include "iot_i2c.h"
#include "FreeRTOS.h"
#include "semphr.h"
}
#include <Wire.h>

typedef struct I2CTrasanctionContext
{
    IotI2CHandle_t handle;
    uint8_t OPT_ADDR;
    uint8_t error;
    uint8_t *readBuffer;
    uint32_t readBufferSize;
    uint32_t readAvailable;
    uint8_t *writeBuffer;
    uint32_t writeBufferSize;
    uint32_t writeAvailable;
    SemaphoreHandle_t semaphore;
    StaticSemaphore_t semaphoreBuffer;

} I2CTransactionContext_t;

I2CTransactionContext_t transactionContext = {0};

#define DEFAULT_WIRE_TRANSACTION_TIMEOUT 1000
#define DEFAULT_WIRE_BAUD_RATE 100000
#define WIRE_OK ((uint8_t)0)
#define WIRE_TOO_LONG ((uint8_t)1)
#define WIRE_ADDR_NACK ((uint8_t)2)
#define WIRE_DATA_NACK ((uint8_t)3)
#define WIRE_FAIL ((uint8_t)4)

/* ------------------------------------------------------------------------ */

void Wire_CallbackInternal(void *context)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(transactionContext.semaphore, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ------------------------------------------------------------------------ */

TwoWire::TwoWire()
{
}

//"opens" handle but doesn't actually open yet, just sets necessary parameters, creates semaphore, sets callback
//doesn't actually open handle yet because TI open() needs sensor address which isn't known yet
void TwoWire::begin()
{
    IotI2CHandle_t handle = iot_i2c_open(0);

    transactionContext.handle = handle;

    transactionContext.semaphore = xSemaphoreCreateBinaryStatic(&transactionContext.semaphoreBuffer);

    if (transactionContext.handle != NULL)
    {
        iot_i2c_set_callback(transactionContext.handle, (IotI2CCallback_t)Wire_CallbackInternal, &transactionContext);
    }
}

//CommonIO and TI both don't support being the slave
void TwoWire::begin(uint8_t slaveAddr)
{
    transactionContext.error = IOT_I2C_FUNCTION_NOT_SUPPORTED;

    return;
}

void TwoWire::begin(int slaveAddr)
{
    begin((uint8_t)slaveAddr);
}

void TwoWire::end()
{
    if (transactionContext.handle != NULL)
    {
        iot_i2c_close(transactionContext.handle);
    }
}

void TwoWire::setClock(uint32_t speed)
{
    IotI2CConfig_t config = {DEFAULT_WIRE_TRANSACTION_TIMEOUT, speed};

    uint8_t error = IOT_I2C_INVALID_VALUE;

    if (transactionContext.handle != NULL)
    {
        error = iot_i2c_ioctl(transactionContext.handle, eI2CSetMasterConfig, &config);
    }

    transactionContext.error = error;
}

void TwoWire::beginTransmission(int addr)
{
    beginTransmission((uint8_t)addr);
}

//begin transmission to I2C slave device with given address
//queue bytes for transmission with write() function and transmit when endTransmission() is called
void TwoWire::beginTransmission(uint8_t addr)
{
    if (transactionContext.handle != NULL)
    {
        iot_i2c_set_callback(transactionContext.handle, (IotI2CCallback_t)Wire_CallbackInternal, &transactionContext);

        if (transactionContext.error == IOT_I2C_SUCCESS)
        {
            IotI2CConfig_t config = {DEFAULT_WIRE_TRANSACTION_TIMEOUT, DEFAULT_WIRE_BAUD_RATE};

            if (transactionContext.handle != NULL)
            {
                uint32_t error = iot_i2c_ioctl(transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&addr);

                if (error != IOT_I2C_SUCCESS)
                {
                    transactionContext.error = error;

                    return;
                }

                error = iot_i2c_ioctl(transactionContext.handle, eI2CSetMasterConfig, &config);

                if (error != IOT_I2C_SUCCESS)
                {
                    transactionContext.error = error;

                    return;
                }
            }
        }
    }
}

uint8_t TwoWire::endTransmission()
{
    return endTransmission(1);
}

//Ends a transmission to a slave device that was begun by beginTransmission() and transmits the bytes that were queued by write().
//If true, endTransmission(), release the I2C bus, TI sends stop by default, clear the buffer
//If false, endTransmission() the bus is not released and everything is kept on the buffer
uint8_t TwoWire::endTransmission(uint8_t stop)
{
    if (transactionContext.error == IOT_I2C_SUCCESS)
    {
        if (stop == 1)
        {
            if (transactionContext.writeAvailable > 0 && transactionContext.writeBuffer != NULL && transactionContext.handle != NULL)
            {
                int status = iot_i2c_write_sync(transactionContext.handle, transactionContext.writeBuffer, transactionContext.writeAvailable);

                if (status == IOT_I2C_SUCCESS)
                {
                    vPortFree(transactionContext.writeBuffer);
                    transactionContext.writeBuffer = NULL;
                    transactionContext.writeBufferSize = 0;
                    transactionContext.writeAvailable = 0;
                }
                if (status == IOT_I2C_NACK)
                {
                    return 2;
                }
                if (status == IOT_I2C_WRITE_FAILED)
                {
                    return 3;
                }
            }
        }
        else if (stop == 0)
        {
            //do nothing, keep everything on the buffer until the next endTransmission
        }
    }
    transactionContext.error = IOT_I2C_SUCCESS;

    return 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
}

//request bytes and save to the buffer to be accessed later by read()
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t num, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
    if (transactionContext.handle != NULL)
    {
        iot_i2c_ioctl(transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&addr);

        if (transactionContext.error == IOT_I2C_SUCCESS)
        {

            if (transactionContext.readBuffer != NULL)
            {
                vPortFree(transactionContext.readBuffer);
                transactionContext.readBuffer = NULL;
                transactionContext.readBufferSize = 0;
                transactionContext.readAvailable = 0;
            }

            transactionContext.readBuffer = (uint8_t *)pvPortMalloc(num * sizeof(uint8_t));

            if (transactionContext.readBuffer != NULL)
            {
                transactionContext.readBufferSize = num;

                int status = iot_i2c_read_sync(transactionContext.handle, transactionContext.readBuffer, num);

                if (status == IOT_I2C_SUCCESS)
                {
                    transactionContext.readAvailable = num;
                    return num;
                }
            }
        }
    }
    return 0;
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

size_t TwoWire::write(uint8_t val)
{
    if (transactionContext.error == IOT_I2C_SUCCESS)
    {
        uint8_t writeBuffer[1] = {val};

        return write(writeBuffer, 1);
    }

    return 0;
}

//queues bytes into buffer for transmission when endTransmission() is called
size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
    if (transactionContext.error == IOT_I2C_SUCCESS)
    {
        if (transactionContext.writeBuffer == NULL)
        {
            transactionContext.writeBuffer = (uint8_t *)pvPortMalloc(BUFFER_LENGTH * sizeof(uint8_t));
            transactionContext.writeBufferSize = BUFFER_LENGTH;
            transactionContext.writeAvailable = 0;
        }
        for (int i = 0; i < quantity; i++)
        {
            transactionContext.writeBuffer[transactionContext.writeAvailable] = data[i];
            transactionContext.writeAvailable++;

            if (transactionContext.writeAvailable >= transactionContext.writeBufferSize)
            {
                return i;
            }
        }

        return quantity;
    }

    return 0;
}

//return number of bytes available for retrieval with read()
int TwoWire::available()
{
    return transactionContext.readAvailable;
}

//returns a byte from the buffer that was queued during requestFrom()
int TwoWire::read()
{
    if (transactionContext.error == IOT_I2C_SUCCESS)
    {
        if (transactionContext.readAvailable > 0)
        {
            configASSERT(transactionContext.readBufferSize >= transactionContext.readAvailable);

            uint8_t val = transactionContext.readBuffer[transactionContext.readBufferSize - transactionContext.readAvailable];

            if (--transactionContext.readAvailable == 0)
            {
                vPortFree(transactionContext.readBuffer);

                transactionContext.readBuffer = NULL;
                transactionContext.readBufferSize = 0;
            }

            return val;
        }
    }

    return -1;
}

//Return first byte available without removing it from the buffer
int TwoWire::peek()
{
    if (transactionContext.error == IOT_I2C_SUCCESS)
    {

        if (transactionContext.readAvailable > 0)
        {
            configASSERT(transactionContext.readBufferSize >= transactionContext.readAvailable);

            uint8_t val = transactionContext.readBuffer[transactionContext.readBufferSize - transactionContext.readAvailable];
            return val;
        }
    }

    return -1;
}

//Registers a function to be called when slave device receives transmission from master--NOT SUPPORTED
void TwoWire::onReceive(void (*function)(int))
{
    transactionContext.error = IOT_I2C_FUNCTION_NOT_SUPPORTED;

    return;
}

//Register a function to be called when a master requests data from this slave devic--NOT SUPPORTED
void TwoWire::onRequest(void (*function)(void))
{
    transactionContext.error = IOT_I2C_FUNCTION_NOT_SUPPORTED;

    return;
}
