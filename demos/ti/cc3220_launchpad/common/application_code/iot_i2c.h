/*
 * Amazon FreeRTOS V1.2.3
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_i2c.h
 * @brief File for the APIs of I2C called by application layer.
 */
#include <stddef.h>
#include <stdint.h>
#ifndef _IOT_I2C_H_
#define _IOT_I2C_H_

/**
 * @defgroup iot_i2c I2C Abstraction APIs.
 * @{
 */

/**
 * The speeds supported by I2C bus.
 */
#define IOT_I2C_STANDARD_MODE_BPS (100000)
#define IOT_I2C_FAST_MODE_BPS (400000)
#define IOT_I2C_FAST_MODE_PLUS_BPS (1000000)
#define IOT_I2C_HIGH_SPEED_BPS (3400000)

/**
 * The return codes for the functions in I2C.
 */
#define IOT_I2C_SUCCESS (0)
#define IOT_I2C_INVALID_VALUE (1)
#define IOT_I2C_BUSY (2)
#define IOT_I2C_WRITE_FAILED (3)
#define IOT_I2C_READ_FAILED (4)
#define IOT_I2C_NACK (5)
#define IOT_I2C_BUS_TIMEOUT (6)
#define IOT_I2C_NOTHING_TO_CANCEL (7)
#define IOT_I2C_FUNCTION_NOT_SUPPORTED (8)

/**
 * @brief I2C Bus status
 */
typedef enum
{
    eI2CBusIdle = 0,
    eI2cBusBusy = IOT_I2C_BUSY,
} IotI2CBusStatus_t;

/**
 * @brief I2C operation status.
 */
typedef enum
{
    eI2CCompleted = IOT_I2C_SUCCESS,
    eI2CNackFromSlave = IOT_I2C_NACK,
    eI2CMasterTimeout = IOT_I2C_BUS_TIMEOUT,

} IotI2COperationStatus_t;

/**
 * @brief I2C bus configuration
 */
typedef struct IotI2CConfig
{
    uint32_t ulMasterTimeout; /**<! Master timeout value in msec, to relinquish the bus if slave does not respond */
    uint32_t ulBusFreq;       /**<! Bus frequency/baud rate */
} IotI2CConfig_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eI2CSendNoStopFlag,    /**<! Set flag to not send stop after transaction */
                           /**<! Default is always stop for every transaction */
                           /**<! Flag will auto reset to stop after one transaction if you set no stop */
    eI2CSetSlaveAddrWrite, /**<! Master sends the start condition followed by slave address to Write. */
    eI2CSetSlaveAddrRead,  /**<! Master sends the start condition followed by slave address to Read.  */
    eI2CSetMasterConfig,   /**<! Sets the I2C bus frequency and timeout using the struct IotI2CIoctlConfig_t,
                              *   default speed is Standard, fast, fast-plus and High speed. */
    eI2CGetMasterConfig,   /**<! Gets the I2C bus frequency and timeout set for the I2C master. */
    eI2CGetBusState,       /**<! Get the current I2C bus status. Returns eI2CBusIdle or eI2CBusy */
    eI2CBusReset,          /**<! Master resets the bus by sending 9 clock signals. */
    eI2CGetTxNoOfbytes,    /**<! Get the number of bytes sent in write operation. */
    eI2CGetRxNoOfbytes,    /**<! Get the number of bytes received in read operation. */
} IotI2CIoctlRequest_t;

/**
 * @brief The I2C descriptor type defined in the source file.
 */
struct IotI2CDescriptor;

/**
 * @brief IotI2CHandle_t is the handle type returned by calling iot_i2c_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotI2CDescriptor *IotI2CHandle_t;

/**
 * @brief The callback function for completion of I2C operation.
 *
 * @param[out] xOpStatus    I2C asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef void (*IotI2CCallback_t)(IotI2COperationStatus_t xOpStatus, void *pvUserContext);

/**
 * @brief Initiates the I2C bus as master.
 *
 * @lI2CInstance The instance of I2C to initialize.
 *
 * @return The handle to the I2C port if SUCCESS else NULL.
 */
IotI2CHandle_t iot_i2c_open(int32_t lI2CInstance);

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * @param[in] pxI2CPeripheral The I2C peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_i2c_set_callback(IotI2CHandle_t const pxI2CPeripheral,
                          IotI2CCallback_t xCallback,
                          void *pvUserContext);

/**
 * @brief Starts the I2C master read operation in blocking mode.
 *        You write_sync the address of the register you want to read from first to read data
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pucBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return ACK = IOT_I2C_SUCCESS,
 *         NACK = IOT_I2C_NACK,
 *         Driver_Error = IOT_I2C_READ_FAILED,
 */
int32_t iot_i2c_read_sync(IotI2CHandle_t const pxI2CPeripheral,
                          uint8_t *const pucBuffer,
                          size_t xBytes);

/**
 * @brief Starts the I2C master write operation in blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pucBuffer The transmit buffer containing the data to be written.
 *                     First byte is always treated as the register address and the following bytes are treated as data to be written.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return ACK = IOT_I2C_SUCCESS,
 *         NACK = IOT_I2C_NACK,
 *         Driver_Error = IOT_I2C_WRITE_FAILED,
 */
int32_t iot_i2c_write_sync(IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t *const pucBuffer,
                           size_t xBytes);

/**
 * @brief Starts the I2C master read operation in non-blocking mode.
 *        You write_sync the address of the register you want to read from first to read data
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pucBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE
 *         BUSY = IOT_I2C_BUSY
 */
int32_t iot_i2c_read_async(IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t *const pucBuffer,
                           size_t xBytes);

/**
 * @brief Starts the I2C master write operation in non-blocking mode.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pucBuffer The transmit buffer containing the data to be written.
 *                     First byte is always treated as the register address and the following bytes are treated as data to be written.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE,
 *         BUSY = IOT_I2C_BUSY
 */
int32_t iot_i2c_write_async(IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t *const pucBuffer,
                            size_t xBytes);

/**
 * @brief Configures the I2C master with user configuration.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] xI2CRequest Should be one of I2C_Ioctl_Request_t.
 * @param[in,out] pvBuffer The configuration values for the IOCTL request.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_FUNCTION_NOT_SUPPORTED,
 *         Busy(For eGetBusState, eSetSlaveAddrWrite/Read, eSendStop) = IOT_I2C_BUSY,
 */
int32_t iot_i2c_ioctl(IotI2CHandle_t const pxI2CPeripheral,
                      IotI2CIoctlRequest_t xI2CRequest,
                      void *const pvBuffer);

/**
 * @brief Stops the ongoing operation and de-initializes the I2C peripheral.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *         FAILED = IOT_I2C_INVALID_VALUE,
 */
int32_t iot_i2c_close(IotI2CHandle_t const pxI2CPeripheral);

/**
 * @brief This function is used to cancel the current operation in progress
 * if possible.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return SUCCESS = IOT_I2C_SUCCESS,
 *          FAILED = IOT_I2C_FUNCTION_INVALID_VALUE
 *          or IOT_I2C_FUNCTION_NOT_SUPPORTED, IOT_I2C_NOTHING_TO_CANCEL
 */
int32_t iot_i2c_cancel(IotI2CHandle_t const pxI2CPeripheral);

/**
 * @}
 */
/* end of group iot_i2c */

#endif /* ifndef _IOT_I2C_H_ */

/**
 * @brief This function is used to set the frequency before the handle is opened
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *            pvBuffer The frequency value to be set
 *
 */
void setFrequency(IotI2CHandle_t const pxI2CPeripheral,
                  void *const pvBuffer);
