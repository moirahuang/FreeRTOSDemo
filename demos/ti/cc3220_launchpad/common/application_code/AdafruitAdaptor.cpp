/*
 * AdafruitAdaptor.c
 *
 *  Created on: Jul 18, 2019
 *      Author: huanmoir
 */
extern "C"
{
#include <stdlib.h>
#include <unistd.h>
#include "aws_hello_world.h"
#include "AdafruitAdaptor.h"
#include "Adafruit_TMP006.h"
#include "Serial.h"
}
#include <BMI160Gen.h>
extern void vStartMQTTEchoDemo(void);
SerialOutput Serial = SerialOutput();

TwoWire Wire = TwoWire();
//Adafruit_TMP006 tmp006;
Adafruit_TMP006 tmp006(0x41); // start with a different i2c address!
//
//void setup(void)
//{
//    vStartMQTTEchoDemo();
//    Serial.begin(9600);
//    Serial.println("Adafruit TMP006 example");
//
//    // you can also use tmp006.begin(TMP006_CFG_1SAMPLE) or 2SAMPLE/4SAMPLE/8SAMPLE to have
//    // lower precision, higher rate sampling. default is TMP006_CFG_16SAMPLE which takes
//    // 4 seconds per reading (16 samples)
//    if (!tmp006.begin())
//    {
//        Serial.println("No sensor found");
//        while (1)
//            ;
//    }
//
//    Serial.println("Send s to enter sleep mode, or w to wake up.  Measurements are not updated while asleep!");
//}
//
//void loop(void)
//{
//    // Grab temperature measurements and print them.
//    double objt = tmp006.readObjTempC();
//    Serial.print("Obj Temperature: ");
//    Serial.print(objt);
//    Serial.println("*C");
//    prvPublishNextMessage("Obj Temperature: %lf'\r\n", objt);
//    double diet = tmp006.readDieTempC();
//    Serial.print("Die Temperature: ");
//    Serial.print(diet);
//    Serial.println("*C");
//    prvPublishNextMessage("Die Temperature: %lf'\r\n", diet);
//
//    //  delay(4000); // 4 seconds per reading for 16 samples per reading
//}

/*
 * Copyright (c) 2016 Intel Corporation.  All rights reserved.
 * See the bottom of this file for the license terms.
 */

/*
   This sketch example demonstrates how the BMI160 on the
   Intel(R) Curie(TM) module can be used to read gyroscope data
*/

float convertRawGyro(int gRaw)
{
    // since we are using 250 degrees/seconds range
    // -250 maps to a raw value of -32768
    // +250 maps to a raw value of 32767

    float g = (gRaw * 250.0) / 32768.0;

    return g;
}

void setup()
{
    Serial.begin(9600); // initialize Serial communication
                        //  while (!Serial);    // wait for the serial port to open

    // initialize device
    Serial.println("Initializing IMU device...");
    //  BMI160.begin(BMI160GenClass::SPI_MODE, /* SS pin# = */10);
    BMI160.begin(BMI160GenClass::I2C_MODE);
    uint8_t dev_id = BMI160.getDeviceID();
    Serial.print("DEVICE ID: ");
    Serial.println(dev_id, HEX);

    // Set the accelerometer range to 250 degrees/second
    BMI160.setGyroRange(250);
    Serial.println("Initializing IMU device...done.");
}

void loop()
{
    int gxRaw, gyRaw, gzRaw; // raw gyro values
    float gx, gy, gz;

    // read raw gyro measurements from device
    BMI160.readGyro(gxRaw, gyRaw, gzRaw);

    // convert the raw gyro data to degrees/second
    gx = convertRawGyro((int)gxRaw);
    gy = convertRawGyro((int)gyRaw);
    gz = convertRawGyro((int)gzRaw);

    // display tab-separated gyro x/y/z values
    Serial.print("g:\t");
    Serial.print(gx);
    Serial.print("\t");
    Serial.print(gy);
    Serial.print("\t");
    Serial.print(gz);
    Serial.println();

    //  delay(500);
}

/*
   Copyright (c) 2016 Intel Corporation.  All rights reserved.
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

void SensorsLoop(void *context)
{
    setup();
    for (;;)
    {
        loop();
        sleep(1);
    }
}
