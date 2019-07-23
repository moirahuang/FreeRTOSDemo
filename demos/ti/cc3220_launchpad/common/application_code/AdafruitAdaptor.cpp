/*
 * AdafruitAdaptor.c
 *
 *  Created on: Jul 18, 2019
 *      Author: huanmoir
 */
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "iot_i2c.h"
#include "aws_hello_world.h"
#include "AdafruitAdaptor.h"
#include "Adafruit_TMP006.h"
#include "Serial.h"
#include "queue.h"
#include "FreeRTOSConfig.h"
}

extern void vStartMQTTEchoDemo(void);
SerialOutput Serial = SerialOutput();

TwoWire Wire = TwoWire();
//Adafruit_TMP006 tmp006;
Adafruit_TMP006 tmp006(0x41); // start with a diferent i2c address!

void setup(void)
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

void loop(void)
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

