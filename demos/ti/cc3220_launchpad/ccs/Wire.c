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
/* EDIT THIS FILE:
 * Wi-Fi SSID, password & security settings,
 * AWS endpoint, certificate, private key & thing name. */
#include "aws_clientcredential.h"

#include "aws_default_root_certificates.h"
#include "aws_secure_sockets_config.h"

/* Demo priorities & stack sizes. */
#include "aws_demo_config.h"

/* FreeRTOS header files. */
#include "FreeRTOS.h"
#include "task.h"

/* Wi-Fi Interface files. */
#include "aws_wifi.h"
#include "aws_pkcs11.h"

/* Demo files. */
#include "aws_logging_task.h"
#include "aws_demo_runner.h"
#include "aws_system_init.h"
#include "aws_dev_mode_key_provisioning.h"

/* TI-Driver includes. */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/net/wifi/simplelink.h>

/* CC3220SF board file. */
#include "Board.h"

/* Application version info. */
#include "aws_application_version.h"

//LORENZO#include <ti/drivers/I2C.h>
#include <iot_i2c.h>

#define OPT_ADDR 0x18
I2C_Handle i2cHandle = NULL;
void Wire_begin() {
    //LORENZOI2C_init();
    //LORENZOi2cHandle = I2C_open(0, NULL);

    iot_i2c_open(0);
}

uint8_t Wire_requestFrom(uint8_t opt, uint8_t num) {
    I2C_Transaction i2cTransaction = {0};
    uint8_t readBuffer[num];
    i2cTransaction.slaveAddress = opt;
    i2cTransaction.writeBuf = NULL;
    i2cTransaction.writeCount = 0;
    i2cTransaction.readBuf = readBuffer;
    i2cTransaction.readCount = num;
    bool status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
        // Unsuccessful I2C transfer
        configPRINTF(("unsuccessful I2C read"));
    } else{
       configPRINTF(("successful I2C read"));
    }
    //not sure how to get data that's read yet
    return (int) i2cTransaction.readBuf;
}

uint8_t Wire_write(uint8_t val) {
    uint8_t valSize = sizeof(val)/sizeof(uint8_t);
    I2C_Transaction i2cTransaction = {0};
    uint8_t writeBuffer[0];
//    int i;
//    for (i = 0; i < valSize; i++){
//        writeBuffer[i] = val[i];
//    }
    writeBuffer[0] = val;
    i2cTransaction.slaveAddress = OPT_ADDR;
    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.writeCount = valSize;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;
    bool status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
       // Unsuccessful I2C transfer
       configPRINTF(("unsuccessful I2C write"));
    } else{
       configPRINTF(("successful I2C write"));
    }


    iot_i2c_ioctl( master config );
    iot_i2c_ioctl( slave address);

    iot_i2c_write_sync( .. );

    //gp bacl to blocking
    //wait for flag from callback, put somehwere so function can find the flag
    return valSize;
};

//read a value to send as a single byte
int Wire_read() {
    I2C_Transaction i2cTransaction = {0};
    uint8_t readBuffer[1];
    i2cTransaction.slaveAddress = OPT_ADDR;
    i2cTransaction.writeBuf = NULL;
    i2cTransaction.writeCount = 0;
    i2cTransaction.readBuf = readBuffer;
    i2cTransaction.readCount = 1;
    bool status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
        // Unsuccessful I2C transfer
        configPRINTF(("unsuccessful I2C read"));
    } else{
       configPRINTF(("successful I2C read"));
    }
    //not sure how to get data that's read yet
    return (int) i2cTransaction.readBuf;
};
int Wire_available(void) {
    return 1;
};

