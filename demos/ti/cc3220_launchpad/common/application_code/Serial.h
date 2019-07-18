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
#include "uart.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

enum SerialConfig
{
    SERIAL_5N1 = UART_5N1,
    SERIAL_6N1 = UART_6N1,
    SERIAL_7N1 = UART_7N1,
    SERIAL_8N1 = UART_8N1,
    SERIAL_5N2 = UART_5N2,
    SERIAL_6N2 = UART_6N2,
    SERIAL_7N2 = UART_7N2,
    SERIAL_8N2 = UART_8N2,
    SERIAL_5E1 = UART_5E1,
    SERIAL_6E1 = UART_6E1,
    SERIAL_7E1 = UART_7E1,
    SERIAL_8E1 = UART_8E1,
    SERIAL_5E2 = UART_5E2,
    SERIAL_6E2 = UART_6E2,
    SERIAL_7E2 = UART_7E2,
    SERIAL_8E2 = UART_8E2,
    SERIAL_5O1 = UART_5O1,
    SERIAL_6O1 = UART_6O1,
    SERIAL_7O1 = UART_7O1,
    SERIAL_8O1 = UART_8O1,
    SERIAL_5O2 = UART_5O2,
    SERIAL_6O2 = UART_6O2,
    SERIAL_7O2 = UART_7O2,
    SERIAL_8O2 = UART_8O2,
};

enum SerialMode
{
    SERIAL_FULL = UART_FULL,
    SERIAL_RX_ONLY = UART_RX_ONLY,
    SERIAL_TX_ONLY = UART_TX_ONLY
};

class SerialOutput
{
public:
    SerialOutput();

    //    bool if(SerialOutput);

    int available();

    int availableForWrite();

    void begin(unsigned long baud)
    {
        begin(baud, SERIAL_8N1);
    }
    void begin(unsigned long baud, SerialConfig config);

    void end();

    bool find(char);
    bool find(char, size_t);

    bool findUntil(char, char);

    void flush();

    float parseFloat();
    //    float parseFloat(LookaheadMode);
    //    float parseFloat(LookaheadMode, char);

    long parseInt();
    //    long parseInt(LookaheadMode);
    //    long parseInt(LookaheadMode, char);

    int peek();

    //    size_t print(const __FlashStringHelper *);
    //    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = DEC);
    size_t print(float, int = DEC);
    //    size_t print(const Printable&);

    //    size_t println(const __FlashStringHelper *);
    //    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = DEC);
    size_t println(float, int = DEC);
    //    size_t println(const Printable&);
    size_t println();

    int read();

    size_t readBytes(char[], int);
    //    size_t readBytes(byte[], int);

    size_t readBytesUntil(char, char, int);
    //    size_t readBytesUntil(char, byte, int);

    char readString();

    char readStringUntil(char);

    void setTiemout(long);

    size_t write(int);
    size_t write(char);
    size_t write(char, int);

    void serialEvent();
};

extern SerialOutput Serial;
