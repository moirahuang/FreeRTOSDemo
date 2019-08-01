
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
#include <BMP280.h>
#include "Serial.h"
#include "Wire.h"
}
#include <BMP280.h>
//extern void vStartMQTTEchoDemo(void);
SerialOutput Serial = SerialOutput();

TwoWire Wire = TwoWire();
//////Adafruit_TMP006 tmp006;
////Adafruit_TMP006 tmp006(0x41); // start with a different i2c address!
////
////void setup(void)
////{
////    vStartMQTTEchoDemo();
////    Serial.begin(9600);
////    Serial.println("Adafruit TMP006 example");
////
////    // you can also use tmp006.begin(TMP006_CFG_1SAMPLE) or 2SAMPLE/4SAMPLE/8SAMPLE to have
////    // lower precision, higher rate sampling. default is TMP006_CFG_16SAMPLE which takes
////    // 4 seconds per reading (16 samples)
////    if (!tmp006.begin())
////    {
////        Serial.println("No sensor found");
////        while (1)
////            ;
////    }
////
////    Serial.println("Send s to enter sleep mode, or w to wake up.  Measurements are not updated while asleep!");
////}
////
////void loop(void)
////{
////    // Grab temperature measurements and print them.
////    double objt = tmp006.readObjTempC();
////    Serial.print("Obj Temperature: ");
////    Serial.print(objt);
////    Serial.println("*C");
////    prvPublishNextMessage("Obj Temperature: %lf'\r\n", objt);
////    double diet = tmp006.readDieTempC();
////    Serial.print("Die Temperature: ");
////    Serial.print(diet);
////    Serial.println("*C");
////    prvPublishNextMessage("Die Temperature: %lf'\r\n", diet);
////
////    //  delay(4000); // 4 seconds per reading for 16 samples per reading
////}
//
///*
// * Copyright (c) 2016 Intel Corporation.  All rights reserved.
// * See the bottom of this file for the license terms.
// */
//
///*
//   This sketch example demonstrates how the BMI160 on the
//   Intel(R) Curie(TM) module can be used to read gyroscope data
//*/
//
////float convertRawGyro(int gRaw)
////{
////    // since we are using 250 degrees/seconds range
////    // -250 maps to a raw value of -32768
////    // +250 maps to a raw value of 32767
////
////    float g = (gRaw * 250.0) / 32768.0;
////
////    return g;
////}
////
////void setup()
////{
////    Serial.begin(9600); // initialize Serial communication
////                        //  while (!Serial);    // wait for the serial port to open
////
////    // initialize device
////    Serial.println("Initializing IMU device...");
////    //  BMI160.begin(BMI160GenClass::SPI_MODE, /* SS pin# = */10);
////    BMI160.begin(BMI160GenClass::I2C_MODE);
////    uint8_t dev_id = BMI160.getDeviceID();
////    Serial.print("DEVICE ID: ");
////    Serial.println(dev_id, HEX);
////
////    // Set the accelerometer range to 250 degrees/second
////    BMI160.setGyroRange(250);
////    Serial.println("Initializing IMU device...done.");
////}
////
////void loop()
////{
////    int gxRaw, gyRaw, gzRaw; // raw gyro values
////    float gx, gy, gz;
////
////    // read raw gyro measurements from device
////    BMI160.readGyro(gxRaw, gyRaw, gzRaw);
////
////    // convert the raw gyro data to degrees/second
////    gx = convertRawGyro((int)gxRaw);
////    gy = convertRawGyro((int)gyRaw);
////    gz = convertRawGyro((int)gzRaw);
////
////    // display tab-separated gyro x/y/z values
////    Serial.print("g:\t");
////    Serial.print(gx);
////    Serial.print("\t");
////    Serial.print(gy);
////    Serial.print("\t");
////    Serial.print(gz);
////    Serial.println();
////
////    //  delay(500);
////}
#include "BMP280.h"
#include "Wire.h"
#define P0 1013.25
BMP280 bmp;
void setup()
{
  Serial.begin(9600);
  if(!bmp.begin()){
    Serial.println("BMP init failed!");
    while(1);
  }
  else Serial.println("BMP init success!");

  bmp.setOversampling(4);

}
void loop()
{
  double T,P;
  char result = bmp.startMeasurment();

  if(result!=0){
//    delay(result);
    result = bmp.getTemperatureAndPressure(T,P);

      if(result!=0)
      {
        double A = bmp.altitude(P,P0);

        Serial.print("T = \t");Serial.print(T,2); Serial.print(" degC\t");
        Serial.print("P = \t");Serial.print(P,2); Serial.print(" mBar\t");
        Serial.print("A = \t");Serial.print(A,2); Serial.println(" m");

      }
      else {
        Serial.println("Error.");
      }
  }
  else {
    Serial.println("Error.");
  }

//  delay(100);
}

//
//#include <Wire.h>
////#include <SPI.h>
//#include <Adafruit_BMP280.h>
//
//#define BMP_SCK  (13)
//#define BMP_MISO (12)
//#define BMP_MOSI (11)
//#define BMP_CS   (10)
//
//Adafruit_BMP280 bmp; // I2C
////Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
////Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
//
//void setup() {
//  Serial.begin(9600);
//  Serial.println("BMP280 test");
//
//  if (!bmp.begin()) {
//    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
//    while (1);
//  }
//
//  /* Default settings from datasheet. */
//  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
//                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
//                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
//                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
//                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
//}
//
//void loop() {
//    Serial.print("Temperature = ");
//    Serial.print(bmp.readTemperature());
//    Serial.println(" *C");
//
//    Serial.print("Pressure = ");
//    Serial.print(bmp.readPressure());
//    Serial.println(" Pa");
//
//    Serial.print("Approx altitude = ");
//    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
//    Serial.println(" m");
//
//    Serial.println();
//    sleep(1);
//}
void SensorsLoop(void *context)
{
    setup();
    for (;;)
    {
        loop();
        sleep(1);
    }
}
