
/*
 * Wire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */
/* Standard includes. */

//MOIRA'S IMPLEMENTATION OF WIRE API, IN BETWEEN ARDUINO AND AFR I2C
extern "C"{
#include <stdlib.h>
#include "iot_i2c.h"
#include "FreeRTOS.h"
#include "semphr.h"
}
#include <Wire.h>

typedef struct I2CTrasanctionContext
{
        IotI2CHandle_t handle;
        uint8_t OPT_ADDR;
        uint8_t error; // no error is zero (0)
        uint8_t * buffer;
        uint32_t bufferSize;
        uint32_t available;
        SemaphoreHandle_t semaphore;
        StaticSemaphore_t semaphoreBuffer;

} I2CTransactionContext_t;

I2CTransactionContext_t transactionContext = { 0 };

#define WIRE_TRANSACTION_TIMEOUT 10000

#define WIRE_OK         ((uint8_t)0)
#define WIRE_TOO_LONG   ((uint8_t)1)
#define WIRE_ADDR_NACK  ((uint8_t)2)
#define WIRE_DATA_NACK  ((uint8_t)3)
#define WIRE_FAIL       ((uint8_t)4)

struct bufferBlock
{
    bool write;
    uint8_t* data;
    size_t quantity;
};

/* ------------------------------------------------------------------------ */
//constructors

TwoWire::TwoWire()
{

}
/* ------------------------------------------------------------------------ */

void Wire_CallbackInternal( void * context )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR( transactionContext.semaphore, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/* ------------------------------------------------------------------------ */

void TwoWire::begin()
{
    IotI2CHandle_t handle = iot_i2c_open(0);

    transactionContext.handle = handle;

    transactionContext.semaphore = xSemaphoreCreateBinaryStatic( &transactionContext.semaphoreBuffer );

    transactionContext.bufferSize = 3;

    transactionContext.buffer =  (uint8_t*) pvPortMalloc( transactionContext.bufferSize * sizeof( uint8_t) );

    iot_i2c_set_completion_callback( transactionContext.handle, Wire_CallbackInternal );
}
void TwoWire::begin(uint8_t addr)
{
    begin();

    uint32_t error = iot_i2c_ioctl( transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&addr );

    if(error != IOT_I2C_SUCCESS)
    {
        transactionContext.error = error;

        return;
    }
}

void TwoWire::setClock(uint32_t speed)
{
    setFrequency( transactionContext.handle, (void *)&speed );
}

void TwoWire::beginTransmission(int addr)
{
    beginTransmission((uint8_t) addr);
}
void TwoWire::beginTransmission(uint8_t addr)
{
    iot_i2c_set_completion_callback( transactionContext.handle, Wire_CallbackInternal );

    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        IotI2CIoctlConfig_t config = {100000, 100000};

        uint32_t error = iot_i2c_ioctl( transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&addr );

        if(error != IOT_I2C_SUCCESS)
        {
            transactionContext.error = error;

            return;
        }

        error = iot_i2c_ioctl( transactionContext.handle, eI2CSetMasterConfig, &config );

        if(error != IOT_I2C_SUCCESS)
        {
            transactionContext.error = error;

            return;
        }
    }
}

uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

uint8_t TwoWire::endTransmission(bool stop)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        iot_i2c_ioctl( transactionContext.handle, eI2CSendStop, NULL);

        for (int i = 0; i < transactionContext.bufferSize; i++)
        {
            bufferBlock currentBlock = *transactionContext[i];

            if (currentBlock)
            {
                int status = iot_i2c_write_async( transactionContext.handle, currentBlock.data, currentBlock.quantity );

                xSemaphoreTake( transactionContext.semaphore, pdMS_TO_TICKS( WIRE_TRANSACTION_TIMEOUT ) );
            }

        }

        if (stop)
        {
            iot_i2c_ioctl( transactionContext.handle, eI2CSendStop, NULL);
        }
        else
        {
            //not sure how to send a restart message
            begin();
        }
        if (status == 0)
        {
            return 0;
        }
        else if (status == 1)
        {
            return 1;
        }
        else if (status == 2)
        {
            return 2;
        }
        else if (status == 6 or status == 4)
        {
            return 3;
        }
        else
        {
            return 4;
        }
    }

    transactionContext.error = IOT_I2C_SUCCESS;

    if( transactionContext.buffer != NULL )
    {
        vPortFree( transactionContext.buffer );

        transactionContext.buffer = NULL;
        transactionContext.bufferSize = 0;
        transactionContext.available = 0;

    }

    return 0;
}
size_t TwoWire::write(uint8_t data)
{
    //set size of buffer, each time you add to it, keep track of if it's a write + the data
        if(transactionContext.error == IOT_I2C_SUCCESS)
        {
            return write(&data, 1);
        }
        return 0;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if (transactionContext.available < transactionContext.bufferSize -1)
        {
            bufferBlock newBlock;

            newBlock.write = true;
            newBlock.data = data;
            newBlock.quantity = quantity;

            transactionContext.buffer[ transactionContext.available ] = (uint8_t) &newBlock;

            transactionContext.available++;

            return quantity;
        }
        else
        {
            uint8_t * tempBuffer = (uint8_t*) pvPortMalloc( transactionContext.bufferSize * 2 * sizeof( uint8_t) );
            for (int i = 0; i < transactionContext.bufferSize; i++)
            {
                tempBuffer[i] =  transactionContext.buffer[i];
            }

            transactionContext.bufferSize = transactionContext.bufferSize * 2;

            vPortFree( transactionContext.buffer );
            transactionContext.buffer = tempBuffer;
            vPortFree( tempBuffer );

            bufferBlock newBlock;

            newBlock.write = true;
            newBlock.data = *data;
            newBlock.quantity = 1;

            transactionContext.buffer[ transactionContext.available ] = (uint8_t) &newBlock;

            transactionContext.available++;

            return 1;
        }
    }

    return 0;
}

int TwoWire::read()
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if( transactionContext.available > 0 )
        {
            configASSERT( transactionContext.bufferSize >= transactionContext.available );

            uint8_t val = transactionContext.buffer[ transactionContext.bufferSize - transactionContext.available ];

            if( --transactionContext.available == 0)
            {
                vPortFree( transactionContext.buffer );

                transactionContext.buffer = NULL;
                transactionContext.bufferSize = 0;
            }

            return val;
        }
    }
    return -1;
}
int TwoWire::peek()
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if( transactionContext.available > 0 )
        {
//            configPRINTF(("r %d\r\n", transactionContext.available));
            configASSERT( transactionContext.bufferSize >= transactionContext.available );

            uint8_t val = transactionContext.buffer[ transactionContext.bufferSize - transactionContext.available ];
            return val;
        }
    }

    return -1;
}
uint8_t TwoWire::requestFrom( uint8_t addr, uint8_t num, uint32_t iaddress, uint8_t isize, uint8_t sendStop )
{
    iot_i2c_ioctl( transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&addr );

    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if( transactionContext.buffer != NULL )
        {
            vPortFree( transactionContext.buffer );

            transactionContext.buffer = NULL;
            transactionContext.bufferSize = 0;
            transactionContext.available = 0;
        }

        transactionContext.buffer = (uint8_t*) pvPortMalloc( num * sizeof( uint8_t) );

        if( transactionContext.buffer != NULL )
        {
            transactionContext.bufferSize = num;

            int status = iot_i2c_read_async( transactionContext.handle, transactionContext.buffer, num );

            if( status == IOT_I2C_SUCCESS )
            {
                if( xSemaphoreTake( transactionContext.semaphore, pdMS_TO_TICKS( WIRE_TRANSACTION_TIMEOUT)) == pdTRUE )
                {
                    transactionContext.available = num;
                    return num;
                }
            }
        }
    }

    return 0;
}
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
    return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

int TwoWire::available(void)
{
    return transactionContext.available;
}


