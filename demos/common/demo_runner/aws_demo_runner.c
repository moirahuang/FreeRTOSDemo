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

#include "AdafruitAdaptor.h"
#include "aws_hello_world.h"

/* Demo declarations. */
extern void vStartMQTTEchoDemo( void );

/*-----------------------------------------------------------*/

void SensorsLoop( void * context )
{
    setup(); //breakpoint
    for (; ;) {
        loop();
        vTaskDelay( 500 );
    }

    vTaskDelete(NULL);
}

/**
 * @brief Runs demos in the system.
 */
void DEMO_RUNNER_RunDemos( void )
{
//
    ( void ) xTaskCreate( SensorsLoop,          /* The function that implements the demo task. */
                          "Arduino Wire Loop",  /* The name to assign to the task being created. */
                          1048,                  /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          NULL,                 /* The task parameter is not being used. */
                          0,                    /* The priority at which the task being created will run. */
                          NULL                  /* Not storing the task handle. */
                          );

}
