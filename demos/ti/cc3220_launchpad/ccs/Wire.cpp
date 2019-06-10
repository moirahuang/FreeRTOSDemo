/*
 * Wire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

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

#include <ti/drivers/I2C.h>

#define OPT_ADDR 0x18
I2C_Handle i2cHandle = NULL;
void begin(void) {
    I2C_init();
    i2cHandle = I2C_open(0, NULL);
}

void RequestFrom() {
    
}

void receive() {
    I2C_Transaction i2cTransaction = {0};
    uint8_t readBuffer[5];
    i2cTransaction.slaveAddress = OPT_ADDR;
    i2cTransaction.writeBuf = NULL;
    i2cTransaction.writeCount = 0;
    i2cTransaction.readBuf = readBuffer;
    i2cTransaction.readCount = 5;
    bool status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
        // Unsuccessful I2C transfer
    }
}

void send() {
    I2C_Transaction i2cTransaction = {0};
    uint8_t writeBuffer[3];
    writeBuffer[0] = 0xAB;
    writeBuffer[1] = 0xCD;
    writeBuffer[2] = 0xEF;
    i2cTransaction.slaveAddress = OPT_ADDR;
    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;
    bool status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
       // Unsuccessful I2C transfer
       configPRINTF(("unsuccessful I2C transfer"));
    } else{
       configPRINTF(("successful I2C transfer"));
    }
}

