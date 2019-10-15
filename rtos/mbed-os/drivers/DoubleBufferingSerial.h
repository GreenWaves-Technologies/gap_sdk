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

#ifndef MBED_DOUBLEBUFFERINGSERIAL_H
#define MBED_DOUBLEBUFFERINGSERIAL_H

#include "platform/platform.h"

#if (DEVICE_SERIAL) || defined(DOXYGEN_ONLY)

#include "FileHandle.h"
#include "SerialBase.h"
#include "InterruptIn.h"
#include "PlatformMutex.h"
#include "serial_api.h"
#include "platform/NonCopyable.h"

#ifndef MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE
#define MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE  256
#endif

#ifndef MBED_CONF_DRIVERS_UART_SERIAL_TXBUF_SIZE
#define MBED_CONF_DRIVERS_UART_SERIAL_TXBUF_SIZE  256
#endif

namespace mbed {

/** \addtogroup drivers */

/** Class providing buffered UART communication functionality using separate local buffer for send and receive channels
 *
 * @ingroup drivers
 */

class DoubleBufferingSerial : private SerialBase, public FileHandle, private NonCopyable<DoubleBufferingSerial> {

public:

    /** Create a DoubleBufferingSerial port, connected to the specified transmit and receive pins, with a particular baud rate.
     *  @param tx Transmit pin
     *  @param rx Receive pin
     *  @param baud The baud rate of the serial port (optional, defaults to MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE)
     */
    DoubleBufferingSerial(PinName tx, PinName rx, int baud = MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE);
    virtual ~DoubleBufferingSerial();

    /** Equivalent to POSIX poll(). Derived from FileHandle.
     *  Provides a mechanism to multiplex input/output over a set of file handles.
     */
    virtual short poll(short events) const;

    /* Resolve ambiguities versus our private SerialBase
     * (for writable, spelling differs, but just in case)
     */
    using FileHandle::readable;
    using FileHandle::writable;

    /** Write the contents of a buffer to a file
     *
     *  Follows POSIX semantics:
     *
     * * if blocking, block until all data is written
     * * if no data can be written, and non-blocking set, return -EAGAIN
     * * if some data can be written, and non-blocking set, write partial
     *
     *  @param buffer   The buffer to write from
     *  @param length   The number of bytes to write
     *  @return         The number of bytes written, negative error on failure
     */
    virtual ssize_t write(const void *buffer, size_t length);

    /** Read the contents of a file into a buffer
     *
     *  Follows POSIX semantics:
     *
     *  * if no data is available, and non-blocking set return -EAGAIN
     *  * if no data is available, and blocking set, wait until data is available
     *  * If any data is available, call returns immediately
     *
     *  @param buffer   The buffer to read in to
     *  @param length   The number of bytes to read
     *  @return         The number of bytes read, 0 at end of file, negative error on failure
     */
    virtual ssize_t read(void *buffer, size_t length);

    /** Close a file
     *
     *  @return         0 on success, negative error code on failure
     */
    virtual int close();

    /** Check if the file in an interactive terminal device
     *
     *  @return         True if the file is a terminal
     *  @return         False if the file is not a terminal
     *  @return         Negative error code on failure
     */
    virtual int isatty();

    /** Move the file position to a given offset from from a given location
     *
     * Not valid for a device type FileHandle like DoubleBufferingSerial.
     * In case of DoubleBufferingSerial, returns ESPIPE
     *
     *  @param offset   The offset from whence to move to
     *  @param whence   The start of where to seek
     *      SEEK_SET to start from beginning of file,
     *      SEEK_CUR to start from current position in file,
     *      SEEK_END to start from end of file
     *  @return         The new offset of the file, negative error code on failure
     */
    virtual off_t seek(off_t offset, int whence);

    /** Flush any buffers associated with the file
     *
     *  @return         0 on success, negative error code on failure
     */
    virtual int sync();

    /** Set blocking or non-blocking mode
     *  The default is blocking.
     *
     *  @param blocking true for blocking mode, false for non-blocking mode.
     */
    virtual int set_blocking(bool blocking)
    {
        _blocking = blocking;
        return 0;
    }

    /** Check current blocking or non-blocking mode for file operations.
     *
     *  @return             true for blocking mode, false for non-blocking mode.
     */
    virtual bool is_blocking() const
    {
        return _blocking;
    }

    /** Register a callback on state change of the file.
     *
     *  The specified callback will be called on state changes such as when
     *  the file can be written to or read from.
     *
     *  The callback may be called in an interrupt context and should not
     *  perform expensive operations.
     *
     *  Note! This is not intended as an attach-like asynchronous api, but rather
     *  as a building block for constructing  such functionality.
     *
     *  The exact timing of when the registered function
     *  is called is not guaranteed and susceptible to change. It should be used
     *  as a cue to make read/write/poll calls to find the current state.
     *
     *  @param func     Function to call on state change
     */
    virtual void sigio(Callback<void()> func);

    /** Setup interrupt handler for DCD line
     *
     *  If DCD line is connected, an IRQ handler will be setup.
     *  Does nothing if DCD is NC, i.e., not connected.
     *
     *  @param dcd_pin         Pin-name for DCD
     *  @param active_high     a boolean set to true if DCD polarity is active low
     */
    void set_data_carrier_detect(PinName dcd_pin, bool active_high = false);

    /** Set the baud rate
     *
     *  @param baud   The baud rate
     */
    void set_baud(int baud);

    // Expose private SerialBase::Parity as DoubleBufferingSerial::Parity
    using SerialBase::Parity;
    // In C++11, we wouldn't need to also have using directives for each value
    using SerialBase::None;
    using SerialBase::Odd;
    using SerialBase::Even;
    using SerialBase::Forced1;
    using SerialBase::Forced0;

    /** Set the transmission format used by the serial port
     *
     *  @param bits The number of bits in a word (5-8; default = 8)
     *  @param parity The parity used (None, Odd, Even, Forced1, Forced0; default = None)
     *  @param stop_bits The number of stop bits (1 or 2; default = 1)
     */
    void set_format(int bits = 8, Parity parity = DoubleBufferingSerial::None, int stop_bits = 1);
private:

    void wait_ms(uint32_t millisec);

    /** SerialBase lock override */
    virtual void lock(void);

    /** SerialBase unlock override */
    virtual void unlock(void);

    /** Acquire mutex */
    virtual void api_lock(void);

    /** Release mutex */
    virtual void api_unlock(void);

    /** Software serial buffers
     *  By default buffer size is 256 for TX and 256 for RX. Configurable through mbed_app.json
     */
    char _rxbuf[MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE];
    char _txbuf[MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE];

    /** INdicate which half part is in tranfering */
    int read_size;
    int write_size;

    PlatformMutex _mutex;

    Callback<void()> _sigio_cb;

    bool _blocking;
    bool _tx_irq_enabled;
    bool _rx_irq_enabled;
    InterruptIn *_dcd_irq;

    /** Device Hanged up
     *  Determines if the device hanged up on us.
     *
     *  @return True, if hanged up
     */
    bool hup() const;

    /** ISRs for serial
     *  Routines to handle interrupts on serial pins.
     *  Copies data into Local Buffer.
     *  Reports the state change to File handle.
     */
    void tx_irq(int);
    void rx_irq(int);

    void wake(void);

    void dcd_irq(void);

};
} //namespace mbed

#endif //(DEVICE_SERIAL) || defined(DOXYGEN_ONLY)
#endif //MBED_DOUBLEBUFFERINGSERIAL_H
