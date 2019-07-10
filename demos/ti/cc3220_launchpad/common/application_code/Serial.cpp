/*
 * Serial.cpp
 *
 *  Created on: Jul 2, 2019
 *      Author: huanmoir
 */

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include "iot_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "aws_demo_runner.h"
//#include "aws_logging_task_dynamic_buffers.c"
#include "queue.h"
/* Logging includes. */
#include "FreeRTOSConfig.h"
#include "uart_term.h"
#include "aws_hello_world.h"
#include <stddef.h>
#include <string.h>
// Import I2C Driver definitions
#include <ti/drivers/I2C.h>
#include "WireAdaptor.h"
#define _OPEN_SYS_ITOA_EXT
#include <stdio.h>
#include <stdlib.h>
}
#include "Serial.h"
UART_Handle uartHandle;

extern void vStartMQTTEchoDemo(void);
SerialOutput::SerialOutput()
{
}

//    bool if(SerialOutput);

int SerialOutput::available()
{
    return 0;
}

int SerialOutput::availableForWrite()
{
    return 0;
}

void SerialOutput::begin(unsigned long baud, SerialConfig config)
{
}

void SerialOutput::end()
{
}

bool SerialOutput::find(char)
{
    return false;
}
bool SerialOutput::find(char, size_t)
{
    return false;
}

bool SerialOutput::findUntil(char, char)
{
    return false;
}

void SerialOutput::flush()
{
}

float SerialOutput::parseFloat()
{
    //documentation said return 0 if timeout
    return 0;
}
//    float parseFloat(LookaheadMode);
//    float parseFloat(LookaheadMode, char);

long SerialOutput::parseInt()
{
    return 0;
}
//    long parseInt(LookaheadMode);
//    long parseInt(LookaheadMode, char);

int SerialOutput::peek()
{
    //documentation said return -1 if no data available
    return -1;
}
size_t SerialOutput::print(const char str[])
{
    configPRINT_STRING((str));

    vTaskDelay(1);
    return sizeof(str);
}

size_t SerialOutput::print(char c)
{
    char buf[2] = {0};
    buf[0] = c;
    configPRINT_STRING((buf));
    vTaskDelay(1);

    return sizeof(c);
}

size_t SerialOutput::print(unsigned char b, int base)
{
  return print((unsigned long) b, base);
}

size_t SerialOutput::print(int n, int base)
{
  return print((long) n, base);
}

size_t SerialOutput::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

size_t SerialOutput::print(long n, int base)
{
    char buffer [sizeof(long)*8+1] = {0};
    if (base == 10)
    {
        sprintf(buffer, "%d\n", n);
    }
    else if (base == 8)
    {
        sprintf(buffer, "%o\n", n);
    }
    else if (base == 16)
    {
        sprintf(buffer, "%x\n", n);
    }
    else
    {
        sprintf("unsupported conversion", "%lf", n);
    }
    configPRINT_STRING((buffer));


    vTaskDelay(1);

    return sizeof(n);
}

size_t SerialOutput::print(unsigned long n, int base)
{
    return print((long) n, base);
}

size_t SerialOutput::print(double n, int base)
{
    return print((long) n, base);
}
size_t SerialOutput::print(float n, int base)
{
    char buffer [sizeof(float)*8+1] = {0};
    char digit[2];
    sprintf(digit, "%d", base);
    char command[] = "%0.";
    strcat(command, digit);
    strcat(command, "f\n");

    sprintf(buffer, command, n);

    configPRINT_STRING((buffer));

    vTaskDelay(1);

    return sizeof(n);
}
size_t SerialOutput::println(void)
{
    configPRINT_STRING(("\r\n"));
    return 0;
}
size_t SerialOutput::println(const char c[])
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t SerialOutput::println(char c)
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t SerialOutput::println(unsigned char b, int base)
{
  size_t n = print(b, base);
  n += println();
  return n;
}

size_t SerialOutput::println(int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t SerialOutput::println(unsigned int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t SerialOutput::println(float num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t SerialOutput::println(unsigned long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t SerialOutput::println(double num, int digits)
{
  size_t n = print(num, digits);
  n += println();
  return n;
}
int SerialOutput::read()
{
    return -1;
}

size_t SerialOutput::readBytes(char[], int)
{
    return 0;
}
//    size_t readBytes(byte[], int);

size_t SerialOutput::readBytesUntil(char, char, int)
{
    return 0;
}
//    size_t readBytesUntil(char, byte, int);

char SerialOutput::readString()
{
    char ret;
    return ret;
}

char SerialOutput::readStringUntil(char)
{
    char ret;
    return ret;
}

void SerialOutput::setTiemout(long)
{
}

size_t SerialOutput::write(int)
{
    return 0;
}
size_t SerialOutput::write(char)
{
    return 0;
}
size_t SerialOutput::write(char, int)
{
    return 0;
}

void serialEvent()
{
}
