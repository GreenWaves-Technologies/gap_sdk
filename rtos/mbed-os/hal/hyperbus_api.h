/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MBED_HYPERBUS_API_H
#define MBED_HYPERBUS_API_H

#include "device.h"
#include "hal/buffer.h"

#if DEVICE_HYPERBUS

#define HYPERBUS_EVENT_ERROR       (1 << 1)
#define HYPERBUS_EVENT_COMPLETE    (1 << 2)
#define HYPERBUS_EVENT_ALL         (HYPERBUS_EVENT_ERROR | HYPERBUS_EVENT_COMPLETE)

#if DEVICE_HYPERBUS_ASYNCH
/** Asynch HYPERBUS HAL structure
 */
typedef struct {
    struct hyperbus_s hyperbus;        /**< Target specific HYPERBUS structure */
    struct buffer_s tx_buff; /**< Tx buffer */
    struct buffer_s rx_buff; /**< Rx buffer */
} hyperbus_t;

#else
/** Non-asynch HYPERBUS HAL structure
 */
typedef struct hyperbus_s hyperbus_t;

#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup hal_GeneralHYPERBUS HYPERBUS Configuration Functions
 * @{
 */

/** Initialize the HYPERBUS peripheral
 *
 * Configures the pins used by HYPERBUS, sets a default format and frequency, and enables the peripheral
 * @param[out] obj  The HYPERBUS object to initialize
 * @param[in]  ck   The pin to use for CLK
 * @param[in]  ckn  The pin to use for CLK NEG
 * @param[in]  rwds The pin to use for RWDS
 * @param[in, out]  dq0  The pin to use for DQ0
 * @param[in, out]  dq1  The pin to use for DQ1
 * @param[in, out]  dq2  The pin to use for DQ2
 * @param[in, out]  dq3  The pin to use for DQ3
 * @param[in, out]  dq4  The pin to use for DQ4
 * @param[in, out]  dq5  The pin to use for DQ5
 * @param[in, out]  dq6  The pin to use for DQ6
 * @param[in, out]  dq7  The pin to use for DQ7
 * @param[in, out]  csn  The pin to use for CSN0-CSN1
 */
void hyperbus_init(hyperbus_t *obj, PinName dq0, PinName dq1, PinName dq2, PinName dq3, PinName dq4, PinName dq5, PinName dq6, PinName dq7, PinName ck, PinName ckn, PinName rwds, PinName csn0, PinName csn1);

/** Release a HYPERBUS object
 *
 * This will require reference counting at the C++ level to be safe
 *
 * Return the pins owned by the HYPERBUS object to their reset state
 * Disable the HYPERBUS peripheral
 * Disable the HYPERBUS clock
 * @param[in] obj The HYPERBUS object to deinitialize
 */
void hyperbus_free(hyperbus_t *obj);

#if defined(__GAP8__)
/** Configure the HYPERBUS memrory timing
 *
 * Set the latency cycles and timing performence for particular clock frequency
 * @param[in,out] obj   The HYPERBUS object to configure
 * @param[in]  cshi     The value of chip select high between operations
 * @param[in]  css      The value of chip select setup to next CK rising edge
 * @param[in]  csh      The value of chip select hold after next rising edge
 * @param[in]  latency  The value of latency cycle for HyperbusRAM
 * @param[in]  rd_wr    The type of operation, read or write
 * @param[in]  device   The type of hyperbus, ram or flash
 */
void hyperbus_set_timing(hyperbus_t *obj, int cshi, int css, int csh, int latency, int rd_wr, char device);

/** Configure the HYPERBUS maximum transfer data length
 *
 * Set the maximum transfer data length for hyperbus ram or hyperbus flash
 * @param[in,out] obj         The HYPERBUS object to configure
 * @param[in]  max_length_en  The HYPERBUS maximum access data length enable
 * @param[in]  max_length     The HYPERBUS maximum access data length
 * @param[in]  rd_wr          The type of operation, read or write
 * @param[in]  device         The type of hyperbus, ram or flash
 */
void hyperbus_set_max_length(hyperbus_t *obj, int max_length_en, int max_length, int rd_wr, char device);

#else

/** Configure the HYPERBUS memrory timing
 *
 * Set the latency cycles and timing performence for particular clock frequency
 * @param  base     HYPERBUS peripheral address.
 * @param  latency0  The value of latency cycle for chip select 0
 * @param  latency1  The value of latency cycle for chip select 1
 * @param  recovery The value of recovery cycle for Hyperbus
 * @param  rwds_delay The value of rwds delay
 * @param  latency_autocheck_en  The value of additional latency cycle for HyperbusRAM
 * @param  csm         The type of chip select maximum cycle for HyperbusRAM
 */
void hyperbus_set_timing(hyperbus_t *obj, int latency0, int latency1, int recovery, int rwds_delay, int latency_autocheck_en , int csm);


/** Configure Hyperbus Bus device type
 *
 * set hyperbus bus device type (dt)
 * @param device   The device type
 * @param dt0      If hyperbus, select RAM or FLASH for channel 0
 * @param dt1      If hyperbus, select RAM or FLASH for channel 1
 *
 */
void hyperbus_set_device_type(hyperbus_t *obj, int device, int dt0, int dt1);

#endif

/** Set the HYPERBUS baud rate
 *
 * Actual frequency may differ from the desired frequency due to available dividers and bus clock
 * Configures the HYPERBUS peripheral's baud rate
 * @param[in,out] obj The HYPERBUS object to configure
 * @param[in]     hz  The baud rate in Hz
 */
void hyperbus_frequency(hyperbus_t *obj, int hz);

/**@}*/
/**
 * \defgroup SynchHYPERBUS Synchronous HYPERBUS Hardware Abstraction Layer
 * @{
 */

/** Write a 16-bit value in a address synchronous
 *
 *  The total number of bytes written will be the maximum of
 *  tx_length.
 *
 * @param[in] obj       The HYPERBUS peripheral to use for sending
 * @param[in] addr      The HYPERBUS memory write address
 * @param[in] value     The value to send
 * @param[in] reg_access Access to register or memory
 * @param[in] device    The type of hyperbus, ram or flash
 */
void hyperbus_write(hyperbus_t *obj, const int addr, unsigned short value, char reg_access, char device);


/** Read 16-bit in a address synchronous
 *
 *  The total number of bytes recieved will be the maximum of
 *  rx_length.
 *
 * @param[in] obj        The HYPERBUS peripheral to use for sending
 * @param[in] addr       The HYPERBUS memory read address
 * @param[in] reg_access The access to register or memory
 * @param[in] device     The type of hyperbus, ram or flash
 * @return Returns the 16-bit value received
 */
int hyperbus_read(hyperbus_t *obj, const int addr, char reg_access, char device);

/** Write buffer in a address synchronous
 *
 *  The total number of bytes written will be the maximum of
 *  tx_length.
 *
 * @param[in] obj       The HYPERBUS peripheral to use for sending
 * @param[in] addr      The HYPERBUS memory write address
 * @param[in] tx        The transmit buffer
 * @param[in] tx_length The number of bytes to transmit
 * @param[in] reg_access Access to register or memory
 * @param[in] device    The type of hyperbus, ram or flash
 * @returns
 *      The number of bytes written to the device.
 */
int hyperbus_block_write(hyperbus_t *obj, const int addr, const void *tx, size_t tx_length, char reg_access, char device);


/** Read buffer in a address synchronous
 *
 *  The total number of bytes recieved will be the maximum of
 *  rx_length.
 *
 * @param[in] obj       The HYPERBUS peripheral to use for sending
 * @param[in] addr      The HYPERBUS memory read address
 * @param[in] rx        The receive buffer
 * @param[in] rx_length The number of bytes to receive
 * @param[in] reg_access Access to register or memory
 * @param[in] device    The type of hyperbus, ram or flash
 * @returns
 *      The number of bytes read from the device.
 */
int hyperbus_block_read(hyperbus_t *obj, const int addr, void *rx, size_t rx_length, char reg_access, char device);


/** Checks if the specified HYPERBUS peripheral is in use
 *
 * @param[in] obj The HYPERBUS peripheral to check
 * @return non-zero if the peripheral is currently transmitting
 */
int  hyperbus_busy(hyperbus_t *obj);

/** Get the module number
 *
 * @param[in] obj The HYPERBUS peripheral to check
 * @return The module number
 */
uint8_t hyperbus_get_module(hyperbus_t *obj);

/**@}*/

#if DEVICE_HYPERBUS_ASYNCH
/**
 * \defgroup AsynchHYPERBUS Asynchronous HYPERBUS Hardware Abstraction Layer
 * @{
 */

/** Begin the HYPERBUS transfer. Buffer pointers and lengths are specified in tx_buff and rx_buff
 *
 * @param[in] obj       The HYPERBUS object that holds the transfer information
 * @param[in] addr      The HYPERBUS memory read/write address
 * @param[in] tx        The transmit buffer
 * @param[in] tx_length The number of bytes to transmit
 * @param[in] rx        The receive buffer
 * @param[in] rx_length The number of bytes to receive
 * @param[in] bit_width The bit width of buffer words
 * @param[in] device    The type of hyperbus, ram or flash
 * @param[in] event     The logical OR of events to be registered
 * @param[in] handler   HYPERBUS interrupt handler
 * @param[in] hint      A suggestion for how to use DMA with this transfer
 */
void hyperbus_transfer(hyperbus_t *obj, const int addr, const void *tx, size_t tx_length, void *rx, size_t rx_length, uint8_t bit_width, uint8_t device, uint32_t handler, uint32_t event, DMAUsage hint);

/** The asynchronous IRQ handler
 *
 * Reads the received values out of the RX FIFO, writes values into the TX FIFO and checks for transfer termination
 * conditions, such as buffer overflows or transfer complete.
 * @param[in] obj     The HYPERBUS object that holds the transfer information
 * @return Event flags if a transfer termination condition was met; otherwise 0.
 */
uint32_t hyperbus_irq_handler_asynch(hyperbus_t *obj);

/** Attempts to determine if the HYPERBUS peripheral is already in use
 *
 * @param[in] obj The HYPERBUS object to check for activity
 * @return Non-zero if the HYPERBUS port is active or zero if it is not.
 */
uint8_t hyperbus_active(hyperbus_t *obj);

/** Abort an HYPERBUS transfer
 *
 * @param obj The HYPERBUS peripheral to stop
 */
void hyperbus_abort_asynch(hyperbus_t *obj);

#endif

/**@}*/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HYPERBUS_DEVICE

#endif // MBED_HYPERBUS_API_H

/** @}*/
