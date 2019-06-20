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

} I2CTransactionContext_t;

I2CTransactionContext_t transactionContext = { 0 };

#define WIRE_OK         ((uint8_t)0)
#define WIRE_TOO_LONG   ((uint8_t)1)
#define WIRE_ADDR_NACK  ((uint8_t)2)
#define WIRE_DATA_NACK  ((uint8_t)3)
#define WIRE_FAIL       ((uint8_t)4)

void Wire_CallbackInternal( void * context )
{

}
/* ------------------------------------------------------------------------ */

void Wire_begin()
{
    IotI2CHandle_t handle = iot_i2c_open(0);

    transactionContext.handle = handle;

    iot_i2c_set_completion_callback( transactionContext.handle, Wire_CallbackInternal );
}

void Wire_end()
{
    iot_i2c_close( transactionContext.handle );
}

void Wire_beginTransmission(int addr)
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

uint8_t Wire_endTransmission(void)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        iot_i2c_ioctl( transactionContext.handle, eI2CSendStop, NULL);
    }

    transactionContext.error = IOT_I2C_SUCCESS;

    configPRINTF(("ERROR %d\r\n", transactionContext.error));

    return 0;
}


size_t Wire_write(uint8_t val)
{
    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        uint8_t writeBuffer[1]= { val };

        iot_i2c_write_async( transactionContext.handle, writeBuffer, 1 );
    }

    return val;
}

int Wire_read()
{
    uint8_t readBuffer[4] = {0};

    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
            iot_i2c_read_async( transactionContext.handle, readBuffer, 1 );
    }

    return (int) readBuffer[0];
}

uint8_t Wire_requestFrom(uint8_t opt, uint8_t num)
{
    iot_i2c_ioctl( transactionContext.handle, eI2CSetSlaveAddrWrite, (void *)&opt );

    if(transactionContext.error == IOT_I2C_SUCCESS)
    {
        int i = 0;
        for (i = 0; i < num; i++)
        {
                Wire_read();
        }
    }

    return num;
}

int Wire_available(void)
{
        return 1;
}
