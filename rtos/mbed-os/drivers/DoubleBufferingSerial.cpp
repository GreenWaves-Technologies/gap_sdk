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
#if (DEVICE_SERIAL)

#include <errno.h>
#include "DoubleBufferingSerial.h"
#include "platform/mbed_poll.h"

#if MBED_CONF_RTOS_PRESENT
#include "rtos/Thread.h"
#else
#include "platform/mbed_wait_api.h"
#endif

namespace mbed {

DoubleBufferingSerial::DoubleBufferingSerial(PinName tx, PinName rx, int baud) :
    SerialBase(tx, rx, baud),
    _blocking(true),
    _tx_irq_enabled(false),
    _rx_irq_enabled(true),
    _dcd_irq(NULL)
{
    /* Start non-blocking read in serial device. */
    SerialBase::read((uint8_t *)_rxbuf, MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE, callback(this, &DoubleBufferingSerial::rx_irq), SERIAL_EVENT_RX_ALL, 0);

    read_size  = 0;
    write_size = 0;
}

DoubleBufferingSerial::~DoubleBufferingSerial()
{
    delete _dcd_irq;
}

void DoubleBufferingSerial::dcd_irq()
{
    wake();
}

void DoubleBufferingSerial::set_baud(int baud)
{
    SerialBase::baud(baud);
}

void DoubleBufferingSerial::set_data_carrier_detect(PinName dcd_pin, bool active_high)
{
    delete _dcd_irq;
    _dcd_irq = NULL;

    if (dcd_pin != NC) {
        _dcd_irq = new InterruptIn(dcd_pin);
        if (active_high) {
            _dcd_irq->fall(callback(this, &DoubleBufferingSerial::dcd_irq));
        } else {
            _dcd_irq->rise(callback(this, &DoubleBufferingSerial::dcd_irq));
        }
    }
}

void DoubleBufferingSerial::set_format(int bits, Parity parity, int stop_bits)
{
    api_lock();
    SerialBase::format(bits, parity, stop_bits);
    api_unlock();
}

int DoubleBufferingSerial::close()
{
    /* Does not let us pass a file descriptor. So how to close ?
     * Also, does it make sense to close a device type file descriptor*/
    return 0;
}

int DoubleBufferingSerial::isatty()
{
    return 1;

}

off_t DoubleBufferingSerial::seek(off_t offset, int whence)
{
    /*XXX lseek can be done theoratically, but is it sane to mark positions on a dynamically growing/shrinking
     * buffer system (from an interrupt context) */
    return -ESPIPE;
}

int DoubleBufferingSerial::sync()
{
    // api_lock();

    // while (!_txbuf.empty()) {
    //     api_unlock();
    //     // Doing better than wait would require TxIRQ to also do wake() when becoming empty. Worth it?
    //     wait_ms(1);
    //     api_lock();
    // }

    // api_unlock();

    return 0;
}

void DoubleBufferingSerial::sigio(Callback<void()> func)
{
    core_util_critical_section_enter();
    _sigio_cb = func;
    if (_sigio_cb) {
        short current_events = poll(0x7FFF);
        if (current_events) {
            _sigio_cb();
        }
    }
    core_util_critical_section_exit();
}

ssize_t DoubleBufferingSerial::write(const void *buffer, size_t length)
{
    size_t data_written = 0;
    const char *buf_ptr = static_cast<const char *>(buffer);

    if (length == 0) {
        return 0;
    }

    api_lock();
    while (data_written < length) {
        SerialBase::_base_putc(buf_ptr[data_written++]);
    }
    api_unlock();

    return data_written != 0 ? (ssize_t) data_written : (ssize_t) - EAGAIN;
}

ssize_t DoubleBufferingSerial::read(void *buffer, size_t length)
{
    size_t data_read = 0;

    char *ptr = static_cast<char *>(buffer);

    if (length == 0) {
        return 0;
    }

    api_lock();
    while (data_read < length) {
        ptr[data_read++] = _rxbuf[read_size++];

        if (read_size == MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE)
            read_size = 0;
    }
    api_unlock();

    return data_read;
}

bool DoubleBufferingSerial::hup() const
{
    return _dcd_irq && _dcd_irq->read() != 0;
}

void DoubleBufferingSerial::wake()
{
    if (_sigio_cb) {
        _sigio_cb();
    }
}

short DoubleBufferingSerial::poll(short events) const
{
    short revents = 0;

    /* Check the Serial if space available for writing out */
    if (events & POLLIN) {
        int read_available_size = MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE - SerialBase::get_read_remain_size();

        if (read_available_size != read_size) {
            revents |= POLLIN;
        }
    }

    if ((events & POLLOUT) && (SerialBase::get_write_remain_size() == 0)) {
        revents |= POLLOUT;
    }

    /*TODO Handle other event types */

    return revents;
}

void DoubleBufferingSerial::lock()
{
    // This is the override for SerialBase.
    // No lock required as we only use SerialBase from interrupt or from
    // inside our own critical section.
}

void DoubleBufferingSerial::unlock()
{
    // This is the override for SerialBase.
}

void DoubleBufferingSerial::api_lock(void)
{
    _mutex.lock();
}

void DoubleBufferingSerial::api_unlock(void)
{
    _mutex.unlock();
}

void DoubleBufferingSerial::rx_irq(int event)
{
    SerialBase::read((uint8_t *)_rxbuf, MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE, callback(this, &DoubleBufferingSerial::rx_irq), SERIAL_EVENT_RX_ALL, 0);

    /* Report the File handler that data is ready to be read from the buffer. */
    // wake();
}

// Also called from write to start transfer
void DoubleBufferingSerial::tx_irq(int event)
{
    /* Report the File handler that data is ready to be read from the buffer. */
    // wake();
}

void DoubleBufferingSerial::wait_ms(uint32_t millisec)
{
    /* wait_ms implementation for RTOS spins until exact microseconds - we
     * want to just sleep until next tick.
     */
#if MBED_CONF_RTOS_PRESENT
    rtos::Thread::wait(millisec);
#else
    ::wait_ms(millisec);
#endif
}
} //namespace mbed

#endif //(DEVICE_SERIAL)
