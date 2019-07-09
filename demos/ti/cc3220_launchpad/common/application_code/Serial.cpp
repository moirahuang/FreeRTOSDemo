/*
 * Serial.cpp
 *
 *  Created on: Jul 2, 2019
 *      Author: huanmoir
 */

extern "C"{
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
// Import I2C Driver definitions
#include <ti/drivers/I2C.h>
#include "WireAdaptor.h"
}
#include "Serial.h"
UART_Handle uartHandle;

extern void vStartMQTTEchoDemo( void );
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

size_t SerialOutput::print(char *print)
{
    configPRINT_STRING((print));
    vTaskDelay( 1);

    prvPublishNextMessage(print, 0.0);
    return sizeof(print);
}
size_t SerialOutput::print(float print)
{
    char buffer[100];

    sprintf(buffer, "%lf", print);
    configPRINT_STRING((buffer));

    prvPublishNextMessage(buffer, 0.0);

    vTaskDelay( 1);

    return sizeof(print);
}
//size_t SerialOutput::print(float, char);
//size_t SerialOutput::print(float, float);

void SerialOutput::println(char *print)
{
    configPRINT_STRING((print));
    configPRINT_STRING(("\r\n"));

    prvPublishNextMessage(print, 0.0);
    vTaskDelay( 1);
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




