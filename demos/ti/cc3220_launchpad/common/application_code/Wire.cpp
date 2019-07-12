
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
        uint8_t *readBuffer;
        uint32_t readBufferSize;
        uint32_t readAvailable;
        uint8_t *writeBuffer;
        uint32_t writeBufferSize;
        uint32_t writeAvailable;

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

    iot_i2c_set_completion_callback( transactionContext.handle, Wire_CallbackInternal );
}

void TwoWire::end()
{
    iot_i2c_close( transactionContext.handle );
}

void TwoWire::setClock(uint32_t speed)
{
    setFrequency( transactionContext.handle, (void *)&speed );
}

void TwoWire::beginTransmission(int addr)
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
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        iot_i2c_ioctl( transactionContext.handle, eI2CSendStop, NULL);
    }

    transactionContext.error = IOT_I2C_SUCCESS;


    return 0;
}

size_t TwoWire::write(uint8_t val)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        uint8_t writeBuffer[ 1 ]= { val };

        return write(writeBuffer, 1);
    }

    return 0;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if (transactionContext.writeBuffer == NULL)
        {
            transactionContext.writeBuffer = (uint8_t *)pvPortMalloc(BUFFER_LENGTH * 2 * sizeof(uint8_t));
            transactionContext.writeBufferSize = BUFFER_LENGTH;
            transactionContext.writeAvailable = 0;
        }

        uint8_t writeBuffer[ 1 ] = {data[0]};

        for(size_t i = 0; i < quantity; ++i){

            writeBuffer[ 1 ]= { data[i] };
        }

        iot_i2c_write_async( transactionContext.handle, writeBuffer, 1 );

        xSemaphoreTake( transactionContext.semaphore, pdMS_TO_TICKS( WIRE_TRANSACTION_TIMEOUT ) );

        return 1;
    }

    return 0;
}

int TwoWire::read()
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if( transactionContext.readAvailable > 0 )
        {
//            configPRINTF(("r %d\r\n", transactionContext.available));
            configASSERT( transactionContext.readBufferSize >= transactionContext.readAvailable );

            uint8_t val = transactionContext.readBuffer[ transactionContext.readBufferSize - transactionContext.readAvailable ];

            if( --transactionContext.readAvailable == 0)
            {
                vPortFree( transactionContext.readBuffer );

                transactionContext.readBuffer = NULL;
                transactionContext.readBufferSize = 0;
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
        if( transactionContext.readAvailable > 0 )
        {
//            configPRINTF(("r %d\r\n", transactionContext.available));
            configASSERT( transactionContext.readBufferSize >= transactionContext.readAvailable );

            uint8_t val = transactionContext.readBuffer[ transactionContext.readBufferSize - transactionContext.readAvailable ];
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
        if( transactionContext.readBuffer != NULL )
        {
            vPortFree( transactionContext.readBuffer );

            transactionContext.readBuffer = NULL;
            transactionContext.readBufferSize = 0;
            transactionContext.readAvailable = 0;

        }

        transactionContext.readBuffer = (uint8_t*) pvPortMalloc( num * sizeof( uint8_t) );


        if( transactionContext.readBuffer != NULL )
        {
            transactionContext.readBufferSize = num;

            int status = iot_i2c_read_async( transactionContext.handle, transactionContext.readBuffer, num );

            if( status == IOT_I2C_SUCCESS )
            {
                if( xSemaphoreTake( transactionContext.semaphore, pdMS_TO_TICKS( WIRE_TRANSACTION_TIMEOUT)) == pdTRUE )
                {
                    transactionContext.readAvailable = num;
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
    return transactionContext.readAvailable;
}


