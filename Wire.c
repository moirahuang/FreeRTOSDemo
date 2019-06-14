/*
 * Wire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */
/* Standard includes. */

//MOIRA'S IMPLEMENTATION OF WIRE API, IN BETWEEN ARDUINO AND AFR I2C
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

///* TI-Driver includes. */
//#include <ti/drivers/GPIO.h>
//#include <ti/drivers/SPI.h>
//#include <ti/drivers/net/wifi/simplelink.h>
//
///* CC3220SF board file. */
//#include "Board.h"

/* Application version info. */
//#include "aws_application_version.h"

//LORENZO#include <ti/drivers/I2C.h>
#include"iot_i2c.h"
#define OPT_ADDR 0x18
IotI2CHandle_t I2CHandle = NULL;
//IotI2CDescriptor descriptor = NULL;
void Wire_begin() {
    I2CHandle = iot_i2c_open(0);
//    descriptor.handle = I2CHandle;
}

uint8_t Wire_requestFrom(uint8_t opt, uint8_t num) {
//    I2C_Transaction i2cTransaction = {0};
//    uint8_t readBuffer[num];
//    i2cTransaction.slaveAddress = opt;
//    i2cTransaction.writeBuf = NULL;
//    i2cTransaction.writeCount = 0;
//    i2cTransaction.readBuf = readBuffer;
//    i2cTransaction.readCount = num;
//    bool status = I2C_transfer(i2cHandle, &i2cTransaction);
//    if (status == false) {
//        // Unsuccessful I2C transfer
//        configPRINTF(("unsuccessful I2C read"));
//    } else{
//       configPRINTF(("successful I2C read"));
//    }
//    //not sure how to get data that's read yet
//    return (int) i2cTransaction.readBuf;

    return 0;
}

size_t Wire_write(uint8_t val) {
    IotI2CIoctlConfig_t config = {10, 100000};
    uint8_t valSize = sizeof(val)/sizeof(uint8_t);
    uint8_t writeBuffer[valSize];
    //slave
    const uint8_t address = OPT_ADDR;
    IotI2CIoctlRequest_t slaveAction = eI2CSetSlaveAddrWrite;
    IotI2CIoctlRequest_t masterAction = eI2CSetMasterConfig;
    iot_i2c_ioctl( I2CHandle, slaveAction, (void *)&address );
    //master
    iot_i2c_ioctl( I2CHandle, masterAction, (void *)&config);
    iot_i2c_write_sync(I2CHandle, writeBuffer, valSize);
    //gp back to blocking
    //wait for flag from callback, put somehwere so function can find the flag
    return valSize;
};

//read a value to send as a single byte
int Wire_read() {
    IotI2CIoctlConfig_t config = {10, 100000};
    I2C_Transaction i2cTransaction = {0};
    uint8_t readBuffer[1];
    IotI2CIoctlRequest_t slaveAction = eI2CSetSlaveAddrWrite;
    IotI2CIoctlRequest_t masterAction = eI2CSetMasterConfig;
    const uint8_t address = OPT_ADDR;
    iot_i2c_ioctl( I2CHandle, slaveAction, (void *)&address );
    //master
    iot_i2c_ioctl( I2CHandle, masterAction, (void *)&config);

    iot_i2c_read_sync(I2CHandle, readBuffer, 1 );
    //not sure how to get data that's read yet
    return (int) i2cTransaction.readBuf;
};
int Wire_available(void) {
    return 1;
};
