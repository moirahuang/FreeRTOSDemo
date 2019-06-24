/*
 * Wire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */
/* Standard includes. */

//MOIRA'S IMPLEMENTATION OF WIRE API, IN BETWEEN ARDUINO AND AFR I2C
#include <stdlib.h>

#include "iot_i2c.h"
#include "FreeRTOS.h"
#include "semphr.h"

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

#define WIRE_TRANSACTION_TIMEOUT 1000

#define WIRE_OK         ((uint8_t)0)
#define WIRE_TOO_LONG   ((uint8_t)1)
#define WIRE_ADDR_NACK  ((uint8_t)2)
#define WIRE_DATA_NACK  ((uint8_t)3)
#define WIRE_FAIL       ((uint8_t)4)

void Wire_CallbackInternal( void * context )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR( transactionContext.semaphore, &xHigherPriorityTaskWoken );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/* ------------------------------------------------------------------------ */

void begin()
{
    IotI2CHandle_t handle = iot_i2c_open(0);

    transactionContext.handle = handle;

    transactionContext.semaphore = xSemaphoreCreateBinaryStatic( &transactionContext.semaphoreBuffer );

    iot_i2c_set_completion_callback( transactionContext.handle, Wire_CallbackInternal );
}

void end()
{
    iot_i2c_close( transactionContext.handle );
}

void setClock(uint32_t speed)
{
    setFrequency( transactionContext.handle, (void *)&speed );
}

void beginTransmission(int addr)
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
//void Wire_beginTransmission(uint8_t addr)
//{
//    iot_i2c_set_completion_callback( transactionContext.handle, Wire_CallbackInternal );
//
//    if(transactionContext.error == IOT_I2C_SUCCESS)
//    {
//        IotI2CIoctlConfig_t config = {100000, 100000};
//
//        uint32_t error = iot_i2c_ioctl( transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&addr );
//
//        if(error != IOT_I2C_SUCCESS)
//        {
//            transactionContext.error = error;
//
//            return;
//        }
//
//        error = iot_i2c_ioctl( transactionContext.handle, eI2CSetMasterConfig, &config );
//
//        if(error != IOT_I2C_SUCCESS)
//        {
//            transactionContext.error = error;
//
//            return;
//        }
//    }
//}

uint8_t endTransmission(void)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        iot_i2c_ioctl( transactionContext.handle, eI2CSendStop, NULL);
    }

    transactionContext.error = IOT_I2C_SUCCESS;

    configPRINTF(("ERROR %d\r\n", transactionContext.error));

    return 0;
}


size_t write(uint8_t val)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        uint8_t writeBuffer[ 1 ]= { val };

        iot_i2c_write_async( transactionContext.handle, writeBuffer, 1 );

        xSemaphoreTake( transactionContext.semaphore, pdMS_TO_TICKS( WIRE_TRANSACTION_TIMEOUT ) );

        return 1;
    }

    return 0;
}

int read()
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if( transactionContext.available > 0 )
        {
            configASSERT( transactionContext.bufferSize >= transactionContext.available );

            int val = transactionContext.buffer[ transactionContext.bufferSize - transactionContext.available ];

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

uint8_t requestFrom( uint8_t opt, uint8_t num )
{
    iot_i2c_ioctl( transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&opt );

    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        if( transactionContext.buffer != NULL )
        {
            vPortFree( transactionContext.buffer );

            transactionContext.buffer = NULL;
            transactionContext.bufferSize = 0;
            transactionContext.available = 0;
        }

        transactionContext.buffer = pvPortMalloc( num * sizeof( uint8_t) );

        if( transactionContext.buffer != NULL )
        {
            transactionContext.bufferSize = num;

            int status = iot_i2c_read_async( transactionContext.handle, transactionContext.buffer, num );

            if( status == IOT_I2C_SUCCESS )
            {
                if( xSemaphoreTake( transactionContext.semaphore, pdMS_TO_TICKS( WIRE_TRANSACTION_TIMEOUT ) == pdTRUE ))
                {
                    transactionContext.available = num;

                    return num;
                }
            }
        }
    }

    return 0;
}

int available(void)
{
    return transactionContext.available;
}
