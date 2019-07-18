/*
 * Wire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */
/* Standard includes. */

//MOIRA'S IMPLEMENTATION OF WIRE API, IN BETWEEN ARDUINO AND AFR I2C
extern "C"
{
#include <stdlib.h>
#include "iot_i2c.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "aws_demo_runner.h"
#include "WireAdaptor.h"
#include "aws_hello_world.h"
#include "Wire.h"
#include "Adafruit_TMP006.h"
#include "Serial.h"
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
  Serial.print("Object Temperature: "); //can take out serial, can just use configprintf
  Serial.print(objt);
  Serial.println("*C");
//  prvPublishNextMessage("Object Temperature: %lf'\r\n", objt); //have screen w code, diagram, console, mqtt screen
  double diet = tmp006.readDieTempC();
  Serial.print("Die Temperature: ");
  Serial.print(diet);
  Serial.println("*C");
//  prvPublishNextMessage("Die Temperature: %lf'\r\n", diet);

  //  delay(4000); // 4 seconds per reading for 16 samples per reading
}

void Wire_begin(void)
{
  Wire.begin();
}

void Wire_end(void)
{
  Wire.end();
}

void Wire_setClock(uint32_t speed)
{
  Wire.setClock(speed);
}

void Wire_beginTransmission(int addr)
{
  Wire.beginTransmission(addr);
}

uint8_t Wire_endTransmission(void)
{
  return Wire.endTransmission();
}

size_t Wire_write(uint8_t val)
{
  return Wire.write(val);
}

int Wire_read(void)
{
  return Wire.read();
}

uint8_t Wire_requestFrom(uint8_t addr, uint8_t num)
{
  return Wire.requestFrom(addr, num);
}

int Wire_available(void)
{
  return Wire.available();
}
