/*
 * Amazon FreeRTOS V1.4.8
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#include "FreeRTOS.h"
#include "task.h"
#include "aws_demo_runner.h"
#include <stddef.h>
// Import I2C Driver definitions
#include <ti/drivers/I2C.h>
// Define name for an index of an I2C bus
#define SENSORS 0
// Define the slave address of device on the SENSORS bus
#define OPT_ADDR 0x18

#include "Wire.h"

/* Demo declarations. */
extern void vStartMQTTEchoDemo( void );

/*-----------------------------------------------------------*/

void SensorsLoop( void * context )
{
    Wire_begin();
    for( ; ; )
    {
//        WIRE CODE

        int val = Wire_read();
        configPRINTF(("Read1 Status %d\r\n", val));
        int val2 = Wire_write(1);
        configPRINTF(("Write Status %d\r\n", val2));
        int val3 = Wire_write(2);
        configPRINTF(("Write2 Status %d\r\n", val3));

//      One-time init of I2C driver
//        I2C_init();
//
//        //LO
//        uint8_t rawDataIn[2] = { 0x00 } ;
//        uint8_t rawDataOut[1] = { 0x00 };
//
//        // Open I2C bus for usage
//        I2C_Handle i2cHandle = I2C_open(0, NULL);
//        configPRINTF(("Open2 handle2 %08p\r\n", i2cHandle));
//        // Initialize slave address of transaction
//        I2C_Transaction transaction = {0};
//        transaction.slaveAddress = OPT_ADDR;
//        configPRINTF(("slave Status %d\r\n", transaction.slaveAddress));
//        // Write/Read from I2C slave device
//        transaction.writeBuf = rawDataOut;
//        transaction.writeCount = 0;
//        transaction.readBuf = rawDataIn;
//        transaction.readCount = 1;
//
//        //first read doesn't work for some reason TODO: figure out why not
//        bool readStatus = I2C_transfer(i2cHandle, &transaction);
//        configPRINTF(("Read1 Status %d, %d\r\n", readStatus, rawDataIn[0]));
//
//        transaction.writeCount = 1;
//        transaction.readCount = 1;
//
//        //2nd read gets NACK try, try to mimic multiple read
//        readStatus = I2C_transfer(i2cHandle, &transaction);
//        configPRINTF(("Read2 Status %d, %d\r\n", readStatus, rawDataIn[0]));
//
//        //3nd read gets lucky
//        readStatus = I2C_transfer(i2cHandle, &transaction);
//        configPRINTF(("Read2 Status %d, %d\r\n", readStatus, rawDataIn[0]));
//
//
//        //write back to back
//        transaction.slaveAddress = OPT_ADDR;
//        transaction.writeBuf = NULL;
//        transaction.writeCount = 0;
//        transaction.readBuf = rawDataIn;
//        transaction.readCount = 1;
//        readStatus = I2C_transfer(i2cHandle, &transaction);
//        configPRINTF(("wrote2 Status %d, %d\r\n", readStatus, rawDataIn[0]));
//
//        // Write to I2C slave device
//        transaction.writeCount = 1;
//        transaction.readCount = 0;
//        I2C_transfer(i2cHandle, &transaction);
//        // Close I2C
//        I2C_close(i2cHandle);

//        //open I2C handle
//        i2cHandle = I2C_open(0, NULL);
//        if (i2cHandle == NULL) {
//                // Error opening I2C
//                while (1) {configPRINTF(("ERROR"));}
//        } else{
//                configPRINTF(("SUCCESS"));
//        }
//
//        //write only
//        I2C_Transaction i2cTransaction = {0};
//        uint8_t writeBuffer[3];
//        writeBuffer[0] = 0xAB;
//        writeBuffer[1] = 0xCD;
//        writeBuffer[2] = 0xEF;
//        i2cTransaction.slaveAddress = OPT_ADDR;
//        i2cTransaction.writeBuf = writeBuffer;
//        i2cTransaction.writeCount = 3;
//        i2cTransaction.readBuf = NULL;
//        i2cTransaction.readCount = 0;
//        bool status = I2C_transfer(i2cHandle, &i2cTransaction);
//        if (status == false) {
//                // Unsuccessful I2C transfer
//                configPRINTF(("unsuccessful I2C transfer"));
//        } else{
//                configPRINTF(("successful I2C transfer"));
//        }
//
//        //read and write in single transaction
//        uint8_t readBuffer[4];
//        i2cTransaction.slaveAddress = OPT_ADDR;
//        i2cTransaction.writeBuf = writeBuffer;
//        i2cTransaction.writeCount = 2;
//        i2cTransaction.readBuf = readBuffer;
//        i2cTransaction.readCount = 4;
//        status = I2C_transfer(i2cHandle, &i2cTransaction);
//        if (status == false) {
//                configPRINTF(("FAILED READ AND WRITE SAME TIME"));
//        } else {
//                configPRINTF(("success read and write same time"));
//        }
    }
}

/**
 * @brief Runs demos in the system.
 */
void DEMO_RUNNER_RunDemos( void )
{
    //vStartMQTTEchoDemo();

    ( void ) xTaskCreate( SensorsLoop,          /* The function that implements the demo task. */
                          "Arduino Wire Loop",  /* The name to assign to the task being created. */
                          1024,                  /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          NULL,                 /* The task parameter is not being used. */
                          0,                    /* The priority at which the task being created will run. */
                          NULL                  /* Not storing the task handle. */
                          );



}
