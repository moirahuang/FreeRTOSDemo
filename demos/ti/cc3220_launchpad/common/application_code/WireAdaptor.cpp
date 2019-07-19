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
#include "WireAdaptor.h"
#include "Wire.h"
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
