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

#define OPT_ADDR 0x18

IotI2CHandle_t I2CHandle = NULL;

void Wire_begin() {
    I2CHandle = iot_i2c_open(0);
}

uint8_t Wire_requestFrom(uint8_t opt, uint8_t num) {
    return 0;
}

size_t Wire_write(uint8_t val) {
    IotI2CIoctlConfig_t config = {100000, 100000};
    uint8_t writeBuffer[1]= { 0x00 };

    const uint8_t address = OPT_ADDR;
    iot_i2c_ioctl( I2CHandle, eI2CSetSlaveAddrWrite, (void *)&address );
    iot_i2c_ioctl( I2CHandle, eI2CSetMasterConfig, &config);

    iot_i2c_write_async(I2CHandle, writeBuffer, 1);
    return val;
};

//read a value to send as a single byte
int Wire_read() {
    IotI2CIoctlConfig_t config = {100000, 100000};
    uint8_t readBuffer[1];

    const uint8_t address = OPT_ADDR;
    iot_i2c_ioctl(I2CHandle, eI2CSetSlaveAddrWrite,(void *)&address);
    iot_i2c_ioctl(I2CHandle, eI2CSetMasterConfig, &config );

    iot_i2c_read_async(I2CHandle, readBuffer, 1 );

    return (int) readBuffer[0];
};
int Wire_available(void) {
    return 1;
};
