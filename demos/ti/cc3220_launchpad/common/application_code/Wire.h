/*
 * Wire.h
 *
 *  Created on: Jun 6, 2019
 *      Author: huanmoir
 */

#ifndef APPLICATION_CODE_WIRE_H_
#define APPLICATION_CODE_WIRE_H_
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
void Wire_begin();
//void begin(uint8_t);
//void begin(int);
void Wire_end();
//void setClock(uint32_t);
void beginTransmission(uint8_t);
//void beginTransmission(int);
//uint8_t endTransmission(void);
//uint8_t endTransmission(uint8_t);
uint8_t Wire_requestFrom(uint8_t, uint8_t);
//uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
//uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
//uint8_t requestFrom(int, int);
//uint8_t requestFrom(int, int, int);
size_t Wire_write(uint8_t);
//virtual size_t write(const uint8_t *, size_t);
int Wire_available(void);
int Wire_read(void);
//virtual int peek(void);
//virtual void flush(void);
//void onReceive( void (*)(int) );
//void onRequest( void (*)(void) );
//
//inline size_t write(unsigned long n) { return write((uint8_t)n); }
//inline size_t write(long n) { return write((uint8_t)n); }
//inline size_t write(unsigned int n) { return write((uint8_t)n); }
//inline size_t write(int n) { return write((uint8_t)n); }

#endif /* APPLICATION_CODE_WIRE_H_ */
