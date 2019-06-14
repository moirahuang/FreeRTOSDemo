/*
 * masterWire.c
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */

// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

//#include <stdio.h>
//#include <Wire.h>
//void setup(void);
//void loop(void);
//int main()
//{
//    /* Call board init functions. */
//    setup();
//
//    /* Start logging task. */
//    loop();
//
//    return( 0 );
//}
//void setup() {
//  Wire_begin();        // join i2c bus (address optional for master)
//}
//
//void loop() {
//  Wire_requestFrom(8, 6);    // request 6 bytes from slave device #8
//
//  while (Wire_available()) { // slave may send less than requested
//    char c = Wire_read(); // receive a byte as character
////    Serial.print(c);         // print the character
//  }
//
////  delay(500);
//}
