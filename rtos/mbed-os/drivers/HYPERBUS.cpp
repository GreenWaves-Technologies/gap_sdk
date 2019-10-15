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
#include "drivers/HYPERBUS.h"
#include "platform/mbed_critical.h"

#if DEVICE_HYPERBUS_ASYNCH
#include "platform/mbed_power_mgmt.h"
#endif

#if DEVICE_HYPERBUS

namespace mbed {

#if DEVICE_HYPERBUS_ASYNCH && TRANSACTION_QUEUE_SIZE_HYPERBUS
CircularBuffer<Transaction<HYPERBUS>, TRANSACTION_QUEUE_SIZE_HYPERBUS> HYPERBUS::_transaction_buffer;
#endif

HYPERBUS::HYPERBUS(PinName dq0, PinName dq1, PinName dq2, PinName dq3,
             PinName dq4, PinName dq5, PinName dq6, PinName dq7,
             PinName ck, PinName ckn, PinName rwds, PinName ssel0,
             PinName ssel1) :
        _hyperbus(),
#if DEVICE_HYPERBUS_ASYNCH
        _irq(this),
        _usage(DMA_USAGE_NEVER),
        _deep_sleep_locked(false),
#endif
#if defined(__GAP8__)
        _device(0),
        _max_length_enable(0),
        _max_length(1024),
        _latency(0),
#else
        _device(1), // HYPERBUS Mode
        _dt0(0),    // HYPERBUS RAM
        _dt1(1),    // HYPERBUS FLASH
        _latency0(6), // HYPERBUS RAM Default latency
        _latency1(0), // HYPERBUS FLASH Default latency
        _recovery(6),  //
        _rwds_delay(1),// Set RWDS delay according to frequecny (0 - 7)
        _latency_autocheck_en(1), // HYPERBUS RAM auto check latency enable
        _csm(666),                // Maximum CS low cycle according to frequency for HYPERBUS RAM
#endif
        _hz(50000000) {
    // No lock needed in the constructor

    hyperbus_init(&_hyperbus, dq0, dq1, dq2, dq3, dq4, dq5, dq6, dq7, ck, ckn, rwds, ssel0, ssel1);
    _acquire();
}

#if defined(__GAP8__)
void HYPERBUS::set_max_length(char device, int max_length, int enable) {
    lock();

    _device = device;
    if(enable) {
        _max_length_enable = enable;
        _max_length = max_length;
    }
    else {
        _max_length_enable = 0;
        _max_length = -1;
    }
    // If changing format while you are the owner then just
    // update format, but if owner is changed then even frequency should be
    // updated which is done by acquire.
    if (_owner == this) {
        if(enable){
            hyperbus_set_max_length(&_hyperbus, _max_length_enable, _max_length, uHYPERBUS_Read, device);
            hyperbus_set_max_length(&_hyperbus, _max_length_enable, _max_length, uHYPERBUS_Write, device);
        }
    } else {
        _acquire();
    }
    unlock();
}

void HYPERBUS::set_timing(char device, int cshi, int css, int csh, char latency) {
    lock();

    _cshi = cshi;
    _css  = css;
    _csh  = csh;
    _latency  = latency;
    _device  = device;
    // If changing format while you are the owner then just
    // update format, but if owner is changed then even frequency should be
    // updated which is done by acquire.
    if (_owner == this) {
        hyperbus_set_timing(&_hyperbus, _cshi, _css, _csh, _latency, uHYPERBUS_Read, _device);
        hyperbus_set_timing(&_hyperbus, _cshi, _css, _csh, _latency, uHYPERBUS_Write, _device);
    } else {
        _acquire();
    }
    unlock();
}
#else
void HYPERBUS::set_device_type(char device, int dt0, int dt1) {
    lock();

    _device = device;
    _dt0    = dt0;
    _dt1    = dt1;

    // If changing format while you are the owner then just
    // update format, but if owner is changed then even frequency should be
    // updated which is done by acquire.
    if (_owner == this) {
        hyperbus_set_device_type(&_hyperbus, _device, _dt0, _dt1);
    } else {
        _acquire();
    }
    unlock();
}

void HYPERBUS::set_timing(int latency0, int latency1, int recovery, int rwds_delay, int latency_autocheck_en , int csm) {
    lock();

    _latency0 = latency0;
    _latency1 = latency1;
    _recovery = recovery;
    _rwds_delay  = rwds_delay;
    _latency_autocheck_en  = latency_autocheck_en;
    // If changing format while you are the owner then just
    // update format, but if owner is changed then even frequency should be
    // updated which is done by acquire.
    if (_owner == this) {
        hyperbus_set_timing(&_hyperbus, latency0, latency1, recovery, rwds_delay, latency_autocheck_en, csm);
    } else {
        _acquire();
    }
    unlock();
}
#endif

void HYPERBUS::frequency(int hz) {
    lock();
    _hz = hz;
    // If changing format while you are the owner then just
    // update frequency, but if owner is changed then even frequency should be
    // updated which is done by acquire.
    if (_owner == this) {
        hyperbus_frequency(&_hyperbus, _hz);
    } else {
        _acquire();
    }
    unlock();
}

HYPERBUS* HYPERBUS::_owner = NULL;
SingletonPtr<PlatformMutex> HYPERBUS::_mutex;

// ignore the fact there are multiple physical hyperbuss, and always update if it wasn't us last
void HYPERBUS::aquire() {
    lock();
     if (_owner != this) {
#if defined(__GAP8__)
         hyperbus_set_max_length(&_hyperbus, _max_length_enable, _max_length, 0, _device);
         hyperbus_set_max_length(&_hyperbus, _max_length_enable, _max_length, 1, _device);
         hyperbus_set_timing(&_hyperbus, _cshi, _css, _csh, _latency, 0, _device);
         hyperbus_set_timing(&_hyperbus, _cshi, _css, _csh, _latency, 1, _device);
#else
         hyperbus_set_device_type(&_hyperbus, _device, _dt0, _dt1);
         hyperbus_set_timing(&_hyperbus, _latency0, _latency1, _recovery, _rwds_delay, _latency_autocheck_en, _csm);
#endif
         hyperbus_frequency(&_hyperbus, _hz);
         _owner = this;
    }
    unlock();
}

// Note: Private function with no locking
void HYPERBUS::_acquire() {
     if (_owner != this) {
#if defined(__GAP8__)
         hyperbus_set_max_length(&_hyperbus, _max_length_enable, _max_length, 0, _device);
         hyperbus_set_max_length(&_hyperbus, _max_length_enable, _max_length, 1, _device);
         hyperbus_set_timing(&_hyperbus, _cshi, _css, _csh, _latency, 0, _device);
         hyperbus_set_timing(&_hyperbus, _cshi, _css, _csh, _latency, 1, _device);
#else
         hyperbus_set_device_type(&_hyperbus, _device, _dt0, _dt1);
         hyperbus_set_timing(&_hyperbus, _latency0, _latency1, _recovery, _rwds_delay, _latency_autocheck_en, _csm);
#endif
         hyperbus_frequency(&_hyperbus, _hz);
         _owner = this;
    }
}

int HYPERBUS::write(const int addr, unsigned short value, char reg_access) {
    lock();
    _acquire();
    hyperbus_write(&_hyperbus, addr, value, reg_access, _device);
    unlock();
    return 0;
}

int HYPERBUS::read(const int addr, char reg_access) {
    lock();
    _acquire();
    unsigned short val = hyperbus_read(&_hyperbus, addr, reg_access, _device);
    unlock();
    return val;
}

int HYPERBUS::write_block(const int addr, const char *tx_buffer, int tx_length, char reg_access) {
    lock();
    _acquire();
    hyperbus_block_write(&_hyperbus, addr, (void *)tx_buffer, tx_length, reg_access, _device);
    unlock();
    return 0;
}

int HYPERBUS::read_block(const int addr, char *rx_buffer, int rx_length, char reg_access) {
    lock();
    _acquire();
    hyperbus_block_read(&_hyperbus, addr,  (void *)rx_buffer, rx_length, reg_access, _device);
    unlock();
    return 0;
}

void HYPERBUS::lock() {
    _mutex->lock();
}

void HYPERBUS::unlock() {
    _mutex->unlock();
}

#if DEVICE_HYPERBUS_ASYNCH

int HYPERBUS::transfer(const int addr, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, const event_callback_t& callback, int event)
{
    if (hyperbus_active(&_hyperbus)) {
        return queue_transfer(addr, tx_buffer, tx_length, rx_buffer, rx_length, bit_width, _device, callback, event);
    }
    start_transfer(addr, tx_buffer, tx_length, rx_buffer, rx_length, bit_width, _device, callback, event);
    return 0;
}

void HYPERBUS::abort_transfer()
{
    hyperbus_abort_asynch(&_hyperbus);
    unlock_deep_sleep();
#if TRANSACTION_QUEUE_SIZE_HYPERBUS
    dequeue_transaction();
#endif
}


void HYPERBUS::clear_transfer_buffer()
{
#if TRANSACTION_QUEUE_SIZE_HYPERBUS
    _transaction_buffer.reset();
#endif
}

void HYPERBUS::abort_all_transfers()
{
    clear_transfer_buffer();
    abort_transfer();
}

int HYPERBUS::set_dma_usage(DMAUsage usage)
{
    if (hyperbus_active(&_hyperbus)) {
        return -1;
    }
    _usage = usage;
    return  0;
}

int HYPERBUS::queue_transfer(const int addr, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, unsigned char device, const event_callback_t& callback, int event)
{
#if TRANSACTION_QUEUE_SIZE_HYPERBUS
    transaction_t t;

    t.addr = addr;
    t.tx_buffer = const_cast<void *>(tx_buffer);
    t.tx_length = tx_length;
    t.rx_buffer = rx_buffer;
    t.rx_length = rx_length;
    t.event = event;
    t.callback = callback;
    t.width = bit_width;
    t.device = device;
    Transaction<HYPERBUS> transaction(this, t);
    if (_transaction_buffer.full()) {
        return -1; // the buffer is full
    } else {
        core_util_critical_section_enter();
        _transaction_buffer.push(transaction);
        if (!hyperbus_active(&_hyperbus)) {
            dequeue_transaction();
        }
        core_util_critical_section_exit();
        return 0;
    }
#else
    return -1;
#endif
}

void HYPERBUS::start_transfer(const int addr, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, unsigned char device, const event_callback_t& callback, int event)
{
    lock_deep_sleep();
    _acquire();
    _callback = callback;
    _irq.callback(&HYPERBUS::irq_handler_asynch);
    hyperbus_transfer(&_hyperbus, addr, tx_buffer, tx_length, rx_buffer, rx_length, bit_width, device, _irq.entry(), event , _usage);
}

void HYPERBUS::lock_deep_sleep()
{
    if (_deep_sleep_locked == false) {
        sleep_manager_lock_deep_sleep();
        _deep_sleep_locked = true;
    }
}

void HYPERBUS::unlock_deep_sleep()
{
    if (_deep_sleep_locked == true) {
        sleep_manager_unlock_deep_sleep();
        _deep_sleep_locked = false;
    }
}

#if TRANSACTION_QUEUE_SIZE_HYPERBUS

void HYPERBUS::start_transaction(transaction_t *data)
{
    start_transfer(data->addr, data->tx_buffer, data->tx_length, data->rx_buffer, data->rx_length, data->width, data->device, data->callback, data->event);
}

void HYPERBUS::dequeue_transaction()
{
    Transaction<HYPERBUS> t;
    if (_transaction_buffer.pop(t)) {
        HYPERBUS* obj = t.get_object();
        transaction_t* data = t.get_transaction();
        obj->start_transaction(data);
    }
}

#endif

void HYPERBUS::irq_handler_asynch(void)
{
    int event = hyperbus_irq_handler_asynch(&_hyperbus);
    if (_callback && (event & HYPERBUS_EVENT_ALL)) {
        unlock_deep_sleep();
        _callback.call(event & HYPERBUS_EVENT_ALL);
    }
#if TRANSACTION_QUEUE_SIZE_HYPERBUS
    if (event & (HYPERBUS_EVENT_ALL | HYPERBUS_EVENT_INTERNAL_TRANSFER_COMPLETE)) {
        // HYPERBUS peripheral is free (event happened), dequeue transaction
        dequeue_transaction();
    }
#endif
}

#endif

} // namespace mbed

#endif
