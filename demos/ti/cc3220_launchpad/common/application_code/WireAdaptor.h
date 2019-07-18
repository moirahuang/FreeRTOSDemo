/*
TwoWire.h - TWI/I2C library for Arduino & Wiring
Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
*/

#include <inttypes.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C"
{
#endif
    void Wire_begin(void);
    // void begin(uint8_t);
    // void begin(int);
    void Wire_end(void);
    void Wire_setClock(uint32_t);
    void Wire_beginTransmission(int);
    uint8_t Wire_endTransmission(void);
    //uint8_t Wire_endTransmission(uint8_t);
    uint8_t Wire_requestFrom(uint8_t, uint8_t);
    // uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    // uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
    // uint8_t requestFrom(int, int);
    // uint8_t requestFrom(int, int, int);
    size_t Wire_write(uint8_t);
    // virtual size_t write(const uint8_t *, size_t);
    int Wire_available(void);
    int Wire_read(void);
    // virtual int peek(void);
    // virtual void flush(void);
    // void onReceive( void (*)(int) );
    // void onRequest( void (*)(void) );
    //
    // inline size_t write(unsigned long n) { return write((uint8_t)n); }
    // inline size_t write(long n) { return write((uint8_t)n); }
    // inline size_t write(unsigned int n) { return write((uint8_t)n); }
    // inline size_t write(int n) { return write((uint8_t)n); }
    // using Print::write;

    //add wire struct w pointers to functions

    //if written in c++ need to use externc for include files other issues etc
    //
    void setup(void);
    void loop(void);
#ifdef __cplusplus
}
#endif
