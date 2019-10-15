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

#ifndef MBED_HYPERBUS_H
#define MBED_HYPERBUS_H

#include "platform/platform.h"

#if defined (DEVICE_HYPERBUS) || defined(DOXYGEN_ONLY)

#include "platform/PlatformMutex.h"
#include "hal/hyperbus_api.h"
#include "platform/SingletonPtr.h"
#include "platform/NonCopyable.h"

#if DEVICE_HYPERBUS_ASYNCH
#include "platform/CThunk.h"
#include "hal/dma_api.h"
#include "platform/CircularBuffer.h"
#include "platform/FunctionPointer.h"
#include "platform/Transaction.h"
#endif

namespace mbed {
/** \addtogroup drivers */

/** A HYPERBUS bus Master, used for communicating with HYPERBUS bus memory devices
 *
 * The default format is set 16 bits and a clock frequency of 50MHz
 *
 * Most HYPERBUS devices will also require Chip Select and Reset signals.
 *
 * @note Synchronization level: Thread safe
 *
 * Example:
 * @code
 * // Write two btyes to a HYPERBUS memory, and record the response
 *
 * #include "mbed.h"
 *
 * // hardware ssel (where applicable)
 * // dq0, dq1, dq2, dq3, dq4, dq5, dq6, dq7, ck, ckn, rwd, ssel0, ssel1
 * //HYPERBUS device(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
 *
 * #define CR0     0x1000
 *
 * int main() {
 *     device.lock();
 *     ssel0 = 0;
 *     device.write(CR0, 0x8010, uHYPERBUS_Reg_Access);
 *     ssel0 = 1;
 *
 *     ssel0 = 0;
 *     int reg = device.read(CR0, uHYPERBUS_Reg_Access);
 *     ssel0 = 1;
 *     device.unlock();
 *
 * }
 * @endcode
 * @ingroup drivers
 */
class HYPERBUS : private NonCopyable<HYPERBUS> {

public:

    /** Create a HYPERBUS master connected to the specified pins
     *
     * Configures the pins used by HYPERBUS, sets a default format and frequency, and enables the peripheral
     * @param[out] obj  The HYPERBUS object to initialize
     * @param[in]  ck   The pin to use for CLK
     * @param[in]  ckn  The pin to use for CLK NEG
     * @param[in]  rwds The pin to use for RWDS
     * @param[in]  dq0  The pin to use for DQ0
     * @param[in]  dq1  The pin to use for DQ1
     * @param[in]  dq2  The pin to use for DQ2
     * @param[in]  dq3  The pin to use for DQ3
     * @param[in]  dq4  The pin to use for DQ4
     * @param[in]  dq5  The pin to use for DQ5
     * @param[in]  dq6  The pin to use for DQ6
     * @param[in]  dq7  The pin to use for DQ7
     * @param[in]  ssel0 The pin to use for CSN0
     * @param[in]  ssel1 The pin to use for CSN0
     */
    HYPERBUS(PinName dq0, PinName dq1, PinName dq2, PinName dq3,
          PinName dq4, PinName dq5, PinName dq6, PinName dq7,
          PinName ck, PinName ckn, PinName rwds, PinName ssel0,
          PinName ssel1=NC);


#if defined(__GAP8__)
    /** Configure the data max transmission length for reading and write to special device
     *
     * @param device       Flash or RAM
     * @param max_length   Maximum RD WR length
     * @param enable       Maximum length transmission enable or not
     *
     */
    void set_max_length(char device, int max_length, int enable);

    /** Configure the HYPERBUS memrory timing
     *
     * Set the latency cycles and timing performence for particular clock frequency
     * @param  device       Flash or RAM
     * @param  cshi     The value of chip select high between operations
     * @param  css      The value of chip select setup to next CK rising edge
     * @param  csh      The value of chip select hold after next rising edge
     * @param  latency  The value of latency cycle for HyperbusRAM
     */
    void set_timing(char device, int cshi, int css, int csh, char latency);
#else
    /** Configure the HYPERBUS BUS device type
     *
     * @param device       HYPERBUS or OCTOSPI
     * @param dt0          Device type 0
     * @param dt1          Device type 1
     *
     */
    void set_device_type(char device, int dt0, int dt1);

    /** Configure the HYPERBUS memrory timing
     *
     * Set the latency cycles and timing performence for particular clock frequency
     * @param  latency0    The value of latency cycle for chip select 0
     * @param  latency1    The value of latency cycle for chip select 1
     * @param  recovery    The value of recovery cycle for Hyperbus
     * @param  rwds_delay  The value of rwds delay
     * @param  latency_autocheck_en  The value of additional latency cycle for HyperbusRAM
     * @param  csm         The type of chip select maximum cycle for HyperbusRAM
     */
    void set_timing(int latency0, int latency1, int recovery, int rwds_delay, int latency_autocheck_en , int csm);
#endif

    /** Set the hyper bus clock frequency
     *
     *  @param hz CLK frequency in hz (default = 50MHz)
     */
    void frequency(int hz = 50000000);

    /** Write to the HYPERBUS Slave and return the status
     *
     *  Here we use explicit transfer, so write just write something to HYPERBUS slave.
     *
     * @param addr       The HYPERBUS memory write address
     * @param value      The value to send
     * @param reg_access The access to register or memory
     *
     */
    virtual int write(const int addr, unsigned short value, char reg_access);

    /** Read from HYPERBUS Slave and return the value
     *
     *  Here we use explicit transfer, so read just read something from HYPERBUS slave.
     *
     * @param addr       The HYPERBUS memory write address
     * @param reg_access The access to register or memory
     *
     *  @returns
     *    Response from the HYPERBUS slave
     */
    virtual int read(const int addr, char reg_access);

    /** Write to the HYPERBUS Slave and return the status
     *
     *  Here we use explicit transfer, so write just write something to HYPERBUS slave.
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default HYPERBUS value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param reg_access Access to register or memory
     *
     */
    virtual int write_block(const int addr, const char *tx_buffer, int tx_length, char reg_access);

    /** Read from HYPERBUS Slave and return the status
     *
     *  Here we use explicit transfer, so read just read something from HYPERBUS slave.
     *
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param reg_access Access to register or memory
     *
     *  @returns
     *    Response from the HYPERBUS slave
     */
    virtual int read_block(const int addr, char *rx_buffer, int rx_length, char reg_access);

    /** Acquire exclusive access to this HYPERBUS bus
     */
    virtual void lock(void);

    /** Release exclusive access to this HYPERBUS bus
     */
    virtual void unlock(void);


#if DEVICE_HYPERBUS_ASYNCH

    /** Start non-blocking HYPERBUS transfer using 8bit buffers.
     *
     * This function locks the deep sleep until any event has occurred
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default HYPERBUS value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param callback  The event callback function
     * @param event     The logical OR of events to modify. Look at hyperbus hal header file for HYPERBUS events.
     * @return Zero if the transfer has started, or -1 if HYPERBUS peripheral is busy
     */
    template<typename Type>
    int transfer(const int addr, const Type *tx_buffer, int tx_length, Type *rx_buffer, int rx_length, const event_callback_t& callback, int event = HYPERBUS_EVENT_COMPLETE) {
        if (hyperbus_active(&_hyperbus)) {
            return queue_transfer(addr, tx_buffer, tx_length, rx_buffer, rx_length, sizeof(Type)*8, _device, callback, event);
        }
        start_transfer(addr, tx_buffer, tx_length, rx_buffer, rx_length, sizeof(Type)*8, _device, callback, event);
        return 0;
    }

    /** Abort the on-going HYPERBUS transfer, and continue with transfer's in the queue if any.
     */
    void abort_transfer();

    /** Clear the transaction buffer
     */
    void clear_transfer_buffer();

    /** Clear the transaction buffer and abort on-going transfer.
     */
    void abort_all_transfers();

    /** Configure DMA usage suggestion for non-blocking transfers
     *
     *  @param usage The usage DMA hint for peripheral
     *  @return Zero if the usage was set, -1 if a transaction is on-going
    */
    int set_dma_usage(DMAUsage usage);

protected:
    /** HYPERBUS IRQ handler
     *
    */
    void irq_handler_asynch(void);

    /** Common transfer method
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default HYPERBUS value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param bit_width The buffers element width
     * @param callback  The event callback function
     * @param event     The logical OR of events to modify
     * @return Zero if the transfer has started or was added to the queue, or -1 if HYPERBUS peripheral is busy/buffer is full
    */
    int transfer(const int addr, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, const event_callback_t& callback, int event);

    /**
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default HYPERBUS value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param bit_width The buffers element width
     * @param device    The type of hyperbus, ram or flash
     * @param callback  The event callback function
     * @param event     The logical OR of events to modify
     * @return Zero if a transfer was added to the queue, or -1 if the queue is full
    */
    int queue_transfer(const int addr, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, unsigned char device, const event_callback_t& callback, int event);

    /** Configures a callback, hyperbus peripheral and initiate a new transfer
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default HYPERBUS value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param bit_width The buffers element width
     * @param device    The type of hyperbus, ram or flash
     * @param callback  The event callback function
     * @param event     The logical OR of events to modify
    */
    void start_transfer(const int addr, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, unsigned char device, const event_callback_t& callback, int event);

private:
    /** Lock deep sleep only if it is not yet locked */
    void lock_deep_sleep();

    /** Unlock deep sleep in case it is locked */
    void unlock_deep_sleep();


#if TRANSACTION_QUEUE_SIZE_HYPERBUS

    /** Start a new transaction
     *
     *  @param data Transaction data
    */
    void start_transaction(transaction_t *data);

    /** Dequeue a transaction
     *
    */
    void dequeue_transaction();
    static CircularBuffer<Transaction<HYPERBUS>, TRANSACTION_QUEUE_SIZE_HYPERBUS> _transaction_buffer;
#endif

#endif

public:
    virtual ~HYPERBUS() {
        hyperbus_free(&_hyperbus);
    }

protected:
    hyperbus_t _hyperbus;

#if DEVICE_HYPERBUS_ASYNCH
    CThunk<HYPERBUS> _irq;
    event_callback_t _callback;
    DMAUsage _usage;
    bool _deep_sleep_locked;
#endif

    void aquire(void);
    static HYPERBUS *_owner;
    static SingletonPtr<PlatformMutex> _mutex;
    int _device;
#if defined(__GAP8__)
    int _max_length_enable;
    int _max_length;
    int _cshi;
    int _css;
    int _csh;
    char _latency;
#else
    int _dt0;
    int _dt1;
    int _latency0;
    int _latency1;
    int _recovery;
    int _rwds_delay;
    int _latency_autocheck_en;
    int _csm;
#endif
    int _hz;

private:
    /* Private acquire function without locking/unlocking
     * Implemented in order to avoid duplicate locking and boost performance
     */
    void _acquire(void);
};

} // namespace mbed

#endif

#endif
