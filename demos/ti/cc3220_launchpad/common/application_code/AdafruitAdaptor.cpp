
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
#include <BMI160Gen.h>
#include <Adafruit_TMP006.h>
#include "Serial.h"
#include "Wire.h"
}
#include <BMP280.h>
SerialOutput Serial = SerialOutput();
TwoWire Wire = TwoWire();

/** ---------------------------
 * TMP006 DEMO RUNNER CODE
 */
Adafruit_TMP006 tmp006(0x41); // start with a different i2c address!

void adasetup(void)
{
    vStartMQTTEchoDemo();
    Serial.begin(9600);
    Serial.println("Adafruit TMP006 example");

    // you can also use tmp006.begin(TMP006_CFG_1SAMPLE) or 2SAMPLE/4SAMPLE/8SAMPLE to have
    // lower precision, higher rate sampling. default is TMP006_CFG_16SAMPLE which takes
    // 4 seconds per reading (16 samples)
    if (!tmp006.begin())
    {
        Serial.println("No sensor found");
        while (1)
            ;
    }

    Serial.println("Send s to enter sleep mode, or w to wake up.  Measurements are not updated while asleep!");
}

void adaloop(void)
{
    // Grab temperature measurements and print them.
    double objt = tmp006.readObjTempC();
    Serial.print("Obj Temperature: ");
    Serial.print(objt);
    Serial.println("*C");
    prvPublishNextMessage("Obj Temperature: %lf'\r\n", objt);
    double diet = tmp006.readDieTempC();
    Serial.print("Die Temperature: ");
    Serial.print(diet);
    Serial.println("*C");
    prvPublishNextMessage("Die Temperature: %lf'\r\n", diet);

    //  delay(4000); // 4 seconds per reading for 16 samples per reading
}

/** ---------------------------
 * BMI180 DEMO RUNNER CODE
 */
float convertRawGyro(int gRaw)
{
    // since we are using 250 degrees/seconds range
    // -250 maps to a raw value of -32768
    // +250 maps to a raw value of 32767

    float g = (gRaw * 250.0) / 32768.0;

    return g;
}

void bmisetup()
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

void bmiloop()
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

/** ---------------------------
 * BMP280 DEMO RUNNER CODE
 */
#include "BMP280.h"
#include "Wire.h"
#define P0 1013.25
BMP280 bmp;
void bmpsetup()
{
  Serial.begin(9600);
  vStartMQTTEchoDemo();

  if(!bmp.begin()){
    Serial.println("BMP init failed!");
    while(1);
  }
  else Serial.println("BMP init success!");

  bmp.setOversampling(4);

}
void bmploop()
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
        prvPublishNextMessage("Temperature: %lf\r\n", T);
        Serial.print("P = \t");Serial.print(P,2); Serial.print(" mBar\t");
        prvPublishNextMessage("Pressure: %lf\r\n", P);
        Serial.print("A = \t");Serial.print(A,2); Serial.println(" m");
        prvPublishNextMessage("Altitude: %lf\r\n", A);

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

void SensorsLoop(void *context)
{
//UNCOMMENT TO RUN TMP006 DEMO
//    adasetup();
//    for (;;)
//    {
//       adaloop();
//       sleep(1);
//    }

//UNCOMMENT TO RUN BMI180 DEMO
//    bmisetup();
//    int i = 0;
//    for (;;)
//    {
//       bmiloop();
//       sleep(1);
//    }

//UNCOMMENT TO RUN BMP280 DEMO
    bmpsetup();
    for (;;)
    {
        bmploop();
        sleep(1);
    }
}
