/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 Modifications copyright (C) 2018 GreenWaves Technologies

 - Change #including drivers/UARTSerial.h
 - Change write function to add support for RISC-V GCC compiler.
 - Change default_console to put static variables inside functions instead of global variables.
 - Change exit function to support for RISC-V GCC compiler.
 */
#include <time.h>
#include "platform/platform.h"
#include "platform/FilePath.h"
#include "hal/serial_api.h"
#include "hal/us_ticker_api.h"
#include "platform/mbed_toolchain.h"
#include "platform/mbed_semihost_api.h"
#include "platform/mbed_interface.h"
#include "platform/SingletonPtr.h"
#include "platform/PlatformMutex.h"
#include "platform/mbed_error.h"
#include "platform/mbed_stats.h"
#include "platform/mbed_critical.h"
#include "platform/mbed_poll.h"
#include "platform/PlatformMutex.h"
#include "drivers/UARTSerial.h"
#include "us_ticker_api.h"
#include "lp_ticker_api.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef SSIZE_MAX
#define SSIZE_MAX INT_MAX
#endif
#include <stdio.h>
#include <errno.h>
#include "platform/mbed_retarget.h"

static SingletonPtr<PlatformMutex> _mutex;

#if defined(__ARMCC_VERSION)
#   if __ARMCC_VERSION >= 6010050
#      include <arm_compat.h>
#   endif
#   include <rt_sys.h>
#   include <rt_misc.h>
#   include <stdint.h>
#   define PREFIX(x)    _sys##x
#   define OPEN_MAX     _SYS_OPEN
#   ifdef __MICROLIB
#       if __ARMCC_VERSION >= 6010050
asm(" .global __use_full_stdio\n");
#       else
#           pragma import(__use_full_stdio)
#       endif
#   endif

#elif defined(__ICCARM__)
#   include <yfuns.h>
#   define PREFIX(x)        _##x
#   define OPEN_MAX         16

#   define STDIN_FILENO     0
#   define STDOUT_FILENO    1
#   define STDERR_FILENO    2

#else
#   include <sys/syslimits.h>
#   define PREFIX(x)    x
#endif

#define FILE_HANDLE_RESERVED    ((FileHandle*)0xFFFFFFFF)

/**
 * Macros for setting console flow control.
 */
#define CONSOLE_FLOWCONTROL_RTS     1
#define CONSOLE_FLOWCONTROL_CTS     2
#define CONSOLE_FLOWCONTROL_RTSCTS  3
#define mbed_console_concat_(x) CONSOLE_FLOWCONTROL_##x
#define mbed_console_concat(x) mbed_console_concat_(x)
#define CONSOLE_FLOWCONTROL mbed_console_concat(MBED_CONF_TARGET_CONSOLE_UART_FLOW_CONTROL)

using namespace mbed;

#if defined(__MICROLIB) && (__ARMCC_VERSION>5030000)
// Before version 5.03, we were using a patched version of microlib with proper names
extern const char __stdin_name[]  = ":tt";
extern const char __stdout_name[] = ":tt";
extern const char __stderr_name[] = ":tt";

#else
extern const char __stdin_name[]  = "/stdin";
extern const char __stdout_name[] = "/stdout";
extern const char __stderr_name[] = "/stderr";
#endif

unsigned char *mbed_heap_start = 0;
uint32_t mbed_heap_size = 0;

/* newlib has the filehandle field in the FILE struct as a short, so
 * we can't just return a Filehandle* from _open and instead have to
 * put it in a filehandles array and return the index into that array
 */
static FileHandle *filehandles[OPEN_MAX] = { FILE_HANDLE_RESERVED, FILE_HANDLE_RESERVED, FILE_HANDLE_RESERVED };
static char stdio_in_prev[OPEN_MAX];
static char stdio_out_prev[OPEN_MAX];
static SingletonPtr<PlatformMutex> filehandle_mutex;

namespace mbed {
void mbed_set_unbuffered_stream(std::FILE *_file);

void remove_filehandle(FileHandle *file)
{
    filehandle_mutex->lock();
    /* Remove all open filehandles for this */
    for (unsigned int fh_i = 0; fh_i < sizeof(filehandles) / sizeof(*filehandles); fh_i++) {
        if (filehandles[fh_i] == file) {
            filehandles[fh_i] = NULL;
        }
    }
    filehandle_mutex->unlock();
}
}

#if DEVICE_SERIAL
extern int stdio_uart_inited;
extern serial_t stdio_uart;

/* Private FileHandle to implement backwards-compatible functionality of
 * direct HAL serial access for default stdin/stdout/stderr.
 * This is not a particularly well-behaved FileHandle for a stream, which
 * is why it's not public. People should be using UARTSerial.
 */
class DirectSerial : public FileHandle {
public:
    DirectSerial(PinName tx, PinName rx, int baud);
    virtual ssize_t write(const void *buffer, size_t size);
    virtual ssize_t read(void *buffer, size_t size);
    virtual off_t seek(off_t offset, int whence = SEEK_SET)
    {
        return -ESPIPE;
    }
    virtual off_t size()
    {
        return -EINVAL;
    }
    virtual int isatty()
    {
        return true;
    }
    virtual int close()
    {
        return 0;
    }
    virtual short poll(short events) const;
};

DirectSerial::DirectSerial(PinName tx, PinName rx, int baud)
{
    if (stdio_uart_inited) {
        return;
    }
    serial_init(&stdio_uart, tx, rx);
    serial_baud(&stdio_uart, baud);
#if   CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
    serial_set_flow_control(&stdio_uart, FlowControlRTS, STDIO_UART_RTS, NC);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
    serial_set_flow_control(&stdio_uart, FlowControlCTS, NC, STDIO_UART_CTS);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
    serial_set_flow_control(&stdio_uart, FlowControlRTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
#endif
}

#if ((defined (__RISCV_ARCH_GAP__ ) && (__RISCV_ARCH_GAP__ == 1)))
ssize_t DirectSerial::write(const void *buffer, size_t size)
{
    const unsigned char *buf = static_cast<const unsigned char *>(buffer);
    #ifdef PRINTF_UART
    {
        for (size_t i = 0; i < size; i++) {
            serial_putc(&stdio_uart, buf[i]);
        }
    }
    #else
    {
        unsigned char *buf_fix  = (unsigned char *)((unsigned int)buf);
        buf_fix[size] = 0;
        puts((const char *)buf_fix);

    }
    #endif

    return size;
}
#else
ssize_t DirectSerial::write(const void *buffer, size_t size)
{
    const unsigned char *buf = static_cast<const unsigned char *>(buffer);
    for (size_t i = 0; i < size; i++) {
        serial_putc(&stdio_uart, buf[i]);
    }
    return size;
}
#endif

ssize_t DirectSerial::read(void *buffer, size_t size)
{
    unsigned char *buf = static_cast<unsigned char *>(buffer);
    if (size == 0) {
        return 0;
    }
    buf[0] = serial_getc(&stdio_uart);
    return 1;
}

short DirectSerial::poll(short events) const
{
    short revents = 0;
    if ((events & POLLIN) && serial_readable(&stdio_uart)) {
        revents |= POLLIN;
    }
    if ((events & POLLOUT) && serial_writable(&stdio_uart)) {
        revents |= POLLOUT;
    }
    return revents;
}
#endif

class Sink : public FileHandle {
public:
    virtual ssize_t write(const void *buffer, size_t size);
    virtual ssize_t read(void *buffer, size_t size);
    virtual off_t seek(off_t offset, int whence = SEEK_SET)
    {
        return ESPIPE;
    }
    virtual off_t size()
    {
        return -EINVAL;
    }
    virtual int isatty()
    {
        return true;
    }
    virtual int close()
    {
        return 0;
    }
};

ssize_t Sink::write(const void *buffer, size_t size)
{
    // Just swallow the data - this is historical non-DEVICE_SERIAL behaviour
    return size;
}

ssize_t Sink::read(void *buffer, size_t size)
{
    // Produce 1 zero byte - historical behaviour returned 1 without touching
    // the buffer
    unsigned char *buf = static_cast<unsigned char *>(buffer);
    buf[0] = 0;
    return 1;
}


MBED_WEAK FileHandle *mbed::mbed_target_override_console(int fd)
{
    return NULL;
}

MBED_WEAK FileHandle *mbed::mbed_override_console(int fd)
{
    return NULL;
}

#if DEVICE_SERIAL
#  if MBED_CONF_PLATFORM_STDIO_BUFFERED_SERIAL
    static UARTSerial console(STDIO_UART_TX, STDIO_UART_RX, MBED_CONF_PLATFORM_STDIO_BAUD_RATE);
#   if   CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
    console.set_flow_control(SerialBase::RTS, STDIO_UART_RTS, NC);
#   elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
    console.set_flow_control(SerialBase::CTS, NC, STDIO_UART_CTS);
#   elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
    console.set_flow_control(SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
#   endif
#  else
    static DirectSerial console(STDIO_UART_TX, STDIO_UART_RX, MBED_CONF_PLATFORM_STDIO_BAUD_RATE);
#  endif
#else // DEVICE_SERIAL
    static Sink console;
#endif

static FileHandle* default_console()
{
    return &console;
}

/* Locate the default console for stdout, stdin, stderr */
static FileHandle *get_console(int fd)
{
    FileHandle *fh = mbed_override_console(fd);
    if (fh) {
        return fh;
    }
    fh = mbed_target_override_console(fd);
    if (fh) {
        return fh;
    }
    return default_console();
}

/* Deal with the fact C library may not _open descriptors 0, 1, 2 - auto bind */
FileHandle *mbed::mbed_file_handle(int fd)
{
    if (fd >= OPEN_MAX) {
        return NULL;
    }
    FileHandle *fh = filehandles[fd];
    if (fh == FILE_HANDLE_RESERVED && fd < 3) {
        filehandles[fd] = fh = get_console(fd);
    }
    return fh;
}

/**
 * Sets errno when file opening fails.
 * Wipes out the filehandle too.
 *
 * @param error is a negative error code returned from an mbed function and
 *              will be negated to store a positive error code in errno
 */
static int handle_open_errors(int error, unsigned filehandle_idx)
{
    errno = -error;
    // Free file handle
    filehandles[filehandle_idx] = NULL;
    return -1;
}

static inline int openflags_to_posix(int openflags)
{
    int posix = openflags;
#ifdef __ARMCC_VERSION
    if (openflags & OPEN_PLUS) {
        posix = O_RDWR;
    } else if (openflags & OPEN_W) {
        posix = O_WRONLY;
    } else if (openflags & OPEN_A) {
        posix = O_WRONLY | O_APPEND;
    } else {
        posix = O_RDONLY;
    }
    /* a, w, a+, w+ all create if file does not already exist */
    if (openflags & (OPEN_A | OPEN_W)) {
        posix |= O_CREAT;
    }
    /* w and w+ truncate */
    if (openflags & OPEN_W) {
        posix |= O_TRUNC;
    }
#elif defined(__ICCARM__)
    switch (openflags & _LLIO_RDWRMASK) {
        case _LLIO_RDONLY:
            posix = O_RDONLY;
            break;
        case _LLIO_WRONLY:
            posix = O_WRONLY;
            break;
        case _LLIO_RDWR  :
            posix = O_RDWR  ;
            break;
    }
    if (openflags & _LLIO_CREAT) {
        posix |= O_CREAT;
    }
    if (openflags & _LLIO_APPEND) {
        posix |= O_APPEND;
    }
    if (openflags & _LLIO_TRUNC) {
        posix |= O_TRUNC;
    }
#elif defined(TOOLCHAIN_GCC)
    posix &= ~O_BINARY;
#endif
    return posix;
}

static int reserve_filehandle()
{
    // find the first empty slot in filehandles, after the slots reserved for stdin/stdout/stderr
    filehandle_mutex->lock();
    int fh_i;
    for (fh_i = 3; fh_i < OPEN_MAX; fh_i++) {
        /* Take a next free filehandle slot available. */
        if (filehandles[fh_i] == NULL) {
            break;
        }
    }
    if (fh_i >= OPEN_MAX) {
        /* Too many file handles have been opened */
        errno = EMFILE;
        filehandle_mutex->unlock();
        return -1;
    }
    filehandles[fh_i] = FILE_HANDLE_RESERVED;
    filehandle_mutex->unlock();

    return fh_i;
}

int mbed::bind_to_fd(FileHandle *fh)
{
    int fildes = reserve_filehandle();
    if (fildes < 0) {
        return fildes;
    }

    filehandles[fildes] = fh;
    stdio_in_prev[fildes] = 0;
    stdio_out_prev[fildes] = 0;

    return fildes;
}

static int unbind_from_fd(int fd, FileHandle *fh)
{
    if (filehandles[fd] == fh) {
        filehandles[fd] = NULL;
        return 0;
    } else {
        errno = EBADF;
        return -1;
    }
}

#ifndef __IAR_SYSTEMS_ICC__
/* IAR provides fdopen itself */
extern "C" std::FILE *fdopen(int fildes, const char *mode)
{
    // This is to avoid scanf and the bloat it brings.
    char buf[1 + sizeof fildes]; /* @(integer) */
    MBED_STATIC_ASSERT(sizeof buf == 5, "Integers should be 4 bytes.");
    buf[0] = '@';
    memcpy(buf + 1, &fildes, sizeof fildes);

    std::FILE *stream = std::fopen(buf, mode);
    /* newlib-nano doesn't appear to ever call _isatty itself, so
     * happily fully buffers an interactive stream. Deal with that here.
     */
    if (stream && isatty(fildes)) {
        mbed_set_unbuffered_stream(stream);
    }
    return stream;
}
#endif

namespace mbed {
std::FILE *fdopen(FileHandle *fh, const char *mode)
{
    // First reserve the integer file descriptor
    int fd = bind_to_fd(fh);
    if (!fd) {
        return NULL;
    }
    // Then bind that to the C stream. If successful, C library
    // takes ownership and responsibility to close.
    std::FILE *stream = ::fdopen(fd, mode);
    if (!stream) {
        unbind_from_fd(fd, fh);
    }
    return stream;
}
}

/* @brief   standard c library fopen() retargeting function.
 *
 * This function is invoked by the standard c library retargeting to handle fopen()
 *
 * @return
 *  On success, a valid FILEHANDLE is returned.
 *  On failure, -1 is returned and errno is set to an appropriate value e.g.
 *   ENOENT     file not found (default errno setting)
 *   EMFILE     the maximum number of open files was exceeded.
 *
 * */
extern "C" FILEHANDLE PREFIX(_open)(const char *name, int openflags)
{
#if defined(__MICROLIB) && (__ARMCC_VERSION>5030000)
#if !defined(MBED_CONF_RTOS_PRESENT)
    // valid only for mbed 2
    // for ulib, this is invoked after RAM init, prior c++
    // used as hook, as post stack/heap is not active there
    extern void mbed_copy_nvic(void);
    extern void mbed_sdk_init(void);

    static int mbed_sdk_inited = 0;
    if (!mbed_sdk_inited) {
        mbed_copy_nvic();
        mbed_sdk_init();
        mbed_sdk_inited = 1;
    }
#endif
    // Before version 5.03, we were using a patched version of microlib with proper names
    // This is the workaround that the microlib author suggested us
    static int n = 0;
    if (std::strcmp(name, ":tt") == 0 && n < 3) {
        return n++;
    }
#else
    /* Use the posix convention that stdin,out,err are filehandles 0,1,2.
     */
    if (std::strcmp(name, __stdin_name) == 0) {
        mbed_file_handle(STDIN_FILENO);
        return STDIN_FILENO;
    } else if (std::strcmp(name, __stdout_name) == 0) {
        mbed_file_handle(STDOUT_FILENO);
        return STDOUT_FILENO;
    } else if (std::strcmp(name, __stderr_name) == 0) {
        mbed_file_handle(STDERR_FILENO);
        return STDERR_FILENO;
    }
#endif
#ifndef __IAR_SYSTEMS_ICC__
    /* FILENAME: "@(integer)" gives an already-allocated descriptor */
    if (name[0] == '@') {
        int fd;
        memcpy(&fd, name + 1, sizeof fd);
        return fd;
    }
#endif
    return open(name, openflags_to_posix(openflags));
}

extern "C" int open(const char *name, int oflag, ...)
{
    int fildes = reserve_filehandle();
    if (fildes < 0) {
        return fildes;
    }

    FileHandle *res = NULL;
    FilePath path(name);

    if (!path.exists()) {
        /* The first part of the filename (between first 2 '/') is not a
         * registered mount point in the namespace.
         */
        return handle_open_errors(-ENODEV, fildes);
    }

    if (path.isFile()) {
        res = path.file();
    } else {
        FileSystemHandle *fs = path.fileSystem();
        if (fs == NULL) {
            return handle_open_errors(-ENODEV, fildes);
        }
        int err = fs->open(&res, path.fileName(), oflag);
        if (err) {
            return handle_open_errors(err, fildes);
        }
    }

    filehandles[fildes] = res;
    stdio_in_prev[fildes] = 0;
    stdio_out_prev[fildes] = 0;

    return fildes;
}

extern "C" int PREFIX(_close)(FILEHANDLE fh)
{
    return close(fh);
}

extern "C" int close(int fildes)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    filehandles[fildes] = NULL;
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    int err = fhc->close();
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

static bool convert_crlf(int fd)
{
#if MBED_CONF_PLATFORM_STDIO_CONVERT_TTY_NEWLINES
    return isatty(fd);
#elif MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES
    return fd < 3 && isatty(fd);
#else
    return false;
#endif
}

#if defined(__ICCARM__)
extern "C" size_t    __write(int        fh, const unsigned char *buffer, size_t length)
{
#else
extern "C" int PREFIX(_write)(FILEHANDLE fh, const unsigned char *buffer, unsigned int length, int mode)
{
#endif

#if defined(MBED_TRAP_ERRORS_ENABLED) && MBED_TRAP_ERRORS_ENABLED && defined(MBED_CONF_RTOS_PRESENT)
    if (core_util_is_isr_active() || !core_util_are_interrupts_enabled()) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_PROHIBITED_IN_ISR_CONTEXT), "Error - writing to a file in an ISR or critical section\r\n", fh);
    }
#endif

    if (length > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }

    ssize_t slength = length;
    ssize_t written = 0;

    if (convert_crlf(fh)) {
        // local prev is previous in buffer during seek
        // stdio_out_prev[fh] is last thing actually written
        char prev = stdio_out_prev[fh];
        // Seek for '\n' without preceding '\r'; if found flush
        // preceding and insert '\r'. Continue until end of input.
        for (ssize_t cur = 0; cur < slength; cur++) {
            if (buffer[cur] == '\n' && prev != '\r') {
                ssize_t r;
                // flush stuff preceding the \n
                if (cur > written) {
                    r = write(fh, buffer + written, cur - written);
                    if (r < 0) {
                        return -1;
                    }
                    written += r;
                    if (written < cur) {
                        // For some reason, didn't write all - give up now
                        goto finish;
                    }
                    stdio_out_prev[fh] = prev;
                }
                // insert a \r now, leaving the \n still to be written
                r = write(fh, "\r", 1);
                if (r < 0) {
                    return -1;
                }
                if (r < 1) {
                    goto finish;
                }
                stdio_out_prev[fh] = '\r';
            }
            prev = buffer[cur];
        }
    }

    // Flush remaining from conversion, or the whole thing if no conversion
    if (written < slength) {
        ssize_t r = write(fh, buffer + written, slength - written);
        if (r < 0) {
            return -1;
        }
        written += r;
        if (written > 0) {
            stdio_out_prev[fh] = buffer[written - 1];
        }
    }

finish:
#ifdef __ARMCC_VERSION
    if (written >= 0) {
        return slength - written;
    } else {
        return written;
    }
#else
    return written;
#endif
}

extern "C" ssize_t write(int fildes, const void *buf, size_t length)
{

    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    ssize_t ret = fhc->write(buf, length);
    if (ret < 0) {
        errno = -ret;
        return -1;
    } else {
        return ret;
    }
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
extern "C" void PREFIX(_exit)(int return_code)
{
    while (1) {}
}

extern "C" void _ttywrch(int ch)
{
    char c = ch;
    write(STDOUT_FILENO, &c, 1);
}
#endif

#if defined(__ICCARM__)
extern "C" size_t    __read(int        fh, unsigned char *buffer, size_t       length)
{
#else
extern "C" int PREFIX(_read)(FILEHANDLE fh, unsigned char *buffer, unsigned int length, int mode)
{
#endif

#if defined(MBED_TRAP_ERRORS_ENABLED) && MBED_TRAP_ERRORS_ENABLED && defined(MBED_CONF_RTOS_PRESENT)
    if (core_util_is_isr_active() || !core_util_are_interrupts_enabled()) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_PROHIBITED_IN_ISR_CONTEXT), "Error - reading from a file in an ISR or critical section\r\n", fh);
    }
#endif

    if (length > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }

    ssize_t bytes_read = 0;

    if (convert_crlf(fh)) {
        while (true) {
            char c;
            ssize_t r = read(fh, &c, 1);
            if (r < 0) {
                return -1;
            }
            if (r == 0) {
                return bytes_read;
            }
            if ((c == '\r' && stdio_in_prev[fh] != '\n') ||
                    (c == '\n' && stdio_in_prev[fh] != '\r')) {
                stdio_in_prev[fh] = c;
                *buffer = '\n';
                break;
            } else if ((c == '\r' && stdio_in_prev[fh] == '\n') ||
                       (c == '\n' && stdio_in_prev[fh] == '\r')) {
                stdio_in_prev[fh] = c;
                continue;
            } else {
                stdio_in_prev[fh] = c;
                *buffer = c;
                break;
            }
        }
        bytes_read = 1;
    } else {
        bytes_read = read(fh, buffer, length);
    }

#ifdef __ARMCC_VERSION
    if (bytes_read < 0) {
        return -1;
    } else if (bytes_read == 0) {
        return 0x80000000 | length; // weird EOF indication
    } else {
        return (ssize_t)length - bytes_read;
    }
#else
    return bytes_read;
#endif
}

extern "C" ssize_t read(int fildes, void *buf, size_t length)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    ssize_t ret = fhc->read(buf, length);
    if (ret < 0) {
        errno = -ret;
        return -1;
    } else {
        return ret;
    }
}


#ifdef __ARMCC_VERSION
extern "C" int PREFIX(_istty)(FILEHANDLE fh)
#else
extern "C" int _isatty(FILEHANDLE fh)
#endif
{
    return isatty(fh);
}

extern "C" int isatty(int fildes)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return 0;
    }

    int tty = fhc->isatty();
    if (tty < 0) {
        errno = -tty;
        return 0;
    } else {
        return tty;
    }
}

extern "C"
#if defined(__ARMCC_VERSION)
int _sys_seek(FILEHANDLE fh, long offset)
#elif defined(__ICCARM__)
long __lseek(int fh, long offset, int whence)
#else
int _lseek(FILEHANDLE fh, int offset, int whence)
#endif
{
#if defined(__ARMCC_VERSION)
    int whence = SEEK_SET;
#endif

    off_t off = lseek(fh, offset, whence);
    // Assuming INT_MAX = LONG_MAX, so we don't care about prototype difference
    if (off > INT_MAX) {
        errno = EOVERFLOW;
        return -1;
    }
    return off;
}

extern "C" off_t lseek(int fildes, off_t offset, int whence)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    off_t off = fhc->seek(offset, whence);
    if (off < 0) {
        errno = -off;
        return -1;
    }
    return off;
}

extern "C" int ftruncate(int fildes, off_t length)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    int err = fhc->truncate(length);
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

#ifdef __ARMCC_VERSION
extern "C" int PREFIX(_ensure)(FILEHANDLE fh)
{
    return fsync(fh);
}
#endif

extern "C" int fsync(int fildes)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    int err = fhc->sync();
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

#ifdef __ARMCC_VERSION
extern "C" long PREFIX(_flen)(FILEHANDLE fh)
{
    FileHandle *fhc = mbed_file_handle(fh);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    off_t size = fhc->size();
    if (size < 0) {
        errno = -size;
        return -1;
    }
    if (size > LONG_MAX) {
        errno = EOVERFLOW;
        return -1;
    }
    return size;
}

// Do not compile this code for TFM secure target
#if !defined(COMPONENT_SPE) || !defined(TARGET_TFM)

#if !defined(__MICROLIB)
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm(".global __use_two_region_memory\n\t");
__asm(".global __use_no_semihosting\n\t");
#else
#pragma import(__use_two_region_memory)
#endif
#endif

// Through weak-reference, we can check if ARM_LIB_HEAP is defined at run-time.
// If ARM_LIB_HEAP is defined, we can fix heap allocation.
extern MBED_WEAK uint32_t   Image$$ARM_LIB_HEAP$$ZI$$Base[];
extern MBED_WEAK uint32_t   Image$$ARM_LIB_HEAP$$ZI$$Length[];
extern MBED_WEAK uint32_t   Image$$ARM_LIB_HEAP$$ZI$$Limit[];

// Heap here is considered starting after ZI ends to Stack start
extern uint32_t               Image$$ARM_LIB_STACK$$ZI$$Base[];
extern uint32_t               Image$$RW_IRAM1$$ZI$$Limit[];

extern "C" MBED_WEAK __value_in_regs struct __initial_stackheap _mbed_user_setup_stackheap(uint32_t R0, uint32_t R1, uint32_t R2, uint32_t R3)
{
    // Define heap by assuming one-region
    uint32_t heap_base  = (uint32_t)Image$$RW_IRAM1$$ZI$$Limit;
    uint32_t heap_limit = (uint32_t)Image$$ARM_LIB_STACK$$ZI$$Base;
    struct __initial_stackheap r;

    // Fix heap if ARM_LIB_HEAP is defined
    if (Image$$ARM_LIB_HEAP$$ZI$$Length) {
        heap_base = (uint32_t) Image$$ARM_LIB_HEAP$$ZI$$Base;
        heap_limit = (uint32_t) Image$$ARM_LIB_HEAP$$ZI$$Limit;
    }

    // Ensure heap_base is 8-byte aligned
    heap_base = (heap_base + 7) & ~0x7;
    r.heap_base = heap_base;
    r.heap_limit = heap_limit;

    return r;
}

#if !defined(__MICROLIB)
extern "C" __value_in_regs struct __argc_argv $Super$$__rt_lib_init(unsigned heapbase, unsigned heaptop);

extern "C" __value_in_regs struct __argc_argv $Sub$$__rt_lib_init(unsigned heapbase, unsigned heaptop)
{
    // Define heap by assuming one-region
    uint32_t heap_base  = (uint32_t)Image$$RW_IRAM1$$ZI$$Limit;
    uint32_t heap_limit = (uint32_t)Image$$ARM_LIB_STACK$$ZI$$Base;

    // Fix heap if ARM_LIB_HEAP is defined
    if (Image$$ARM_LIB_HEAP$$ZI$$Length) {
        heap_base = (uint32_t) Image$$ARM_LIB_HEAP$$ZI$$Base;
        heap_limit = (uint32_t) Image$$ARM_LIB_HEAP$$ZI$$Limit;
    }

    return $Super$$__rt_lib_init((unsigned)heap_base, (unsigned)heap_limit);
}
#endif

extern "C" __value_in_regs struct __initial_stackheap __user_setup_stackheap(uint32_t R0, uint32_t R1, uint32_t R2, uint32_t R3)
{
    return _mbed_user_setup_stackheap(R0, R1, R2, R3);
}

#endif // !defined(COMPONENT_SPE) || !defined(TARGET_TFM)

#endif


#if !defined(__ARMCC_VERSION) && !defined(__ICCARM__)
extern "C" int _fstat(int fh, struct stat *st)
{
    return fstat(fh, st);
}
#endif

extern "C" int fstat(int fildes, struct stat *st)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    st->st_mode = fhc->isatty() ? S_IFCHR : S_IFREG;
    st->st_size = fhc->size();
    return 0;
}

extern "C" int fcntl(int fildes, int cmd, ...)
{
    FileHandle *fhc = mbed_file_handle(fildes);
    if (fhc == NULL) {
        errno = EBADF;
        return -1;
    }

    switch (cmd) {
        case F_GETFL: {
            int flags = 0;
            if (fhc->is_blocking()) {
                flags |= O_NONBLOCK;
            }
            return flags;
        }
        case F_SETFL: {
            va_list ap;
            va_start(ap, cmd);
            int flags = va_arg(ap, int);
            va_end(ap);
            int ret = fhc->set_blocking(flags & O_NONBLOCK);
            if (ret < 0) {
                errno = -ret;
                return -1;
            }
            return 0;
        }

        default: {
            errno = EINVAL;
            return -1;
        }
    }
}

extern "C" int poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
    if (nfds > OPEN_MAX) {
        errno = EINVAL;
        return -1;
    }

    struct mbed::pollfh fhs[OPEN_MAX];
    for (nfds_t n = 0; n < nfds; n++) {
        // Underlying FileHandle poll returns POLLNVAL if given NULL, so
        // we don't need to take special action.
        fhs[n].fh = mbed_file_handle(fds[n].fd);
        fhs[n].events = fds[n].events;
    }
    int ret = poll(fhs, nfds, timeout);
    for (nfds_t n = 0; n < nfds; n++) {
        fds[n].revents = fhs[n].revents;
    }
    return ret;
}

namespace std {
extern "C" int remove(const char *path)
{
    FilePath fp(path);
    FileSystemHandle *fs = fp.fileSystem();
    if (fs == NULL) {
        errno = ENODEV;
        return -1;
    }

    int err = fs->remove(fp.fileName());
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

extern "C" int rename(const char *oldname, const char *newname)
{
    FilePath fpOld(oldname);
    FilePath fpNew(newname);
    FileSystemHandle *fsOld = fpOld.fileSystem();
    FileSystemHandle *fsNew = fpNew.fileSystem();

    if (fsOld == NULL) {
        errno = ENODEV;
        return -1;
    }

    /* rename only if both files are on the same FS */
    if (fsOld != fsNew) {
        errno = EXDEV;
        return -1;
    }

    int err = fsOld->rename(fpOld.fileName(), fpNew.fileName());
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

extern "C" char *tmpnam(char *s)
{
    errno = EBADF;
    return NULL;
}

extern "C" FILE *tmpfile()
{
    errno = EBADF;
    return NULL;
}
} // namespace std

#ifdef __ARMCC_VERSION
extern "C" char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}
#endif

extern "C" DIR *opendir(const char *path)
{
    FilePath fp(path);
    FileSystemHandle *fs = fp.fileSystem();
    if (fs == NULL) {
        errno = ENODEV;
        return NULL;
    }

    DirHandle *dir;
    int err = fs->open(&dir, fp.fileName());
    if (err < 0) {
        errno = -err;
        return NULL;
    }

    return dir;
}

extern "C" struct dirent *readdir(DIR *dir)
{
    static struct dirent ent;
    int err = dir->read(&ent);
    if (err < 1) {
        if (err < 0) {
            errno = -err;
        }
        return NULL;
    }

    return &ent;
}

extern "C" int closedir(DIR *dir)
{
    int err = dir->close();
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

extern "C" void rewinddir(DIR *dir)
{
    dir->rewind();
}

extern "C" off_t telldir(DIR *dir)
{
    return dir->tell();
}

extern "C" void seekdir(DIR *dir, off_t off)
{
    dir->seek(off);
}

extern "C" int mkdir(const char *path, mode_t mode)
{
    FilePath fp(path);
    FileSystemHandle *fs = fp.fileSystem();
    if (fs == NULL) {
        errno = ENODEV;
        return -1;
    }

    int err = fs->mkdir(fp.fileName(), mode);
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

extern "C" int stat(const char *path, struct stat *st)
{
    FilePath fp(path);
    FileSystemHandle *fs = fp.fileSystem();
    if (fs == NULL) {
        errno = ENODEV;
        return -1;
    }

    int err = fs->stat(fp.fileName(), st);
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

extern "C" int statvfs(const char *path, struct statvfs *buf)
{
    FilePath fp(path);
    FileSystemHandle *fs = fp.fileSystem();
    if (fs == NULL) {
        errno = ENODEV;
        return -1;
    }

    int err = fs->statvfs(fp.fileName(), buf);
    if (err < 0) {
        errno = -err;
        return -1;
    } else {
        return 0;
    }
}

#if defined(TOOLCHAIN_GCC)
/* prevents the exception handling name demangling code getting pulled in */
#include "mbed_error.h"
namespace __gnu_cxx {
void __verbose_terminate_handler()
{
    MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_CLIB_EXCEPTION), "Exception", 0);
}
}
extern "C" WEAK void __cxa_pure_virtual(void);
extern "C" WEAK void __cxa_pure_virtual(void)
{
    exit(1);
}

#endif

#if ((defined (__RISCV_ARCH_GAP__ ) && (__RISCV_ARCH_GAP__ == 1)))
extern "C" void _exit(int return_code);
#else
// Provide implementation of _sbrk (low-level dynamic memory allocation
// routine) for GCC_ARM which compares new heap pointer with MSP instead of
// SP.  This make it compatible with RTX RTOS thread stacks.
#if defined(TOOLCHAIN_GCC_ARM)

extern "C" uint32_t         __end__;
extern "C" uint32_t         __HeapLimit;

// Turn off the errno macro and use actual global variable instead.
#undef errno
extern "C" int errno;

// Weak attribute allows user to override, e.g. to use external RAM for dynamic memory.
extern "C" WEAK caddr_t _sbrk(int incr)
{
    static uint32_t heap = (uint32_t) &__end__;
    uint32_t prev_heap = heap;
    uint32_t new_heap = heap + incr;

    /* __HeapLimit is end of heap section */
    if (new_heap > (uint32_t) &__HeapLimit) {
        errno = ENOMEM;
        return (caddr_t) - 1;
    }

    heap = new_heap;
    return (caddr_t) prev_heap;
}
#endif

#if defined(TOOLCHAIN_GCC_ARM)
extern "C" void _exit(int return_code)
{
#else
namespace std {
extern "C" void exit(int return_code)
{
#endif

#if DEVICE_STDIO_MESSAGES
#if MBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT
    fflush(stdout);
    fflush(stderr);
    fsync(STDOUT_FILENO);
    fsync(STDERR_FILENO);
#endif
#endif

#if DEVICE_SEMIHOST
    if (mbed_interface_connected()) {
        semihost_exit();
    }
#endif
    if (return_code) {
        mbed_die();
    }

    while (1);
}

#if !defined(TOOLCHAIN_GCC_ARM)
} //namespace std
#endif
#endif

#if defined(TOOLCHAIN_ARM) || defined(TOOLCHAIN_GCC)

// This series of function disable the registration of global destructors
// in a dynamic table which will be called when the application exit.
// In mbed, program never exit properly, it dies.
// More informations about this topic for ARMCC here:
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/6449.html
extern "C" {
    int __aeabi_atexit(void *object, void (*dtor)(void * /*this*/), void *handle)
    {
        return 1;
    }

    int __cxa_atexit(void (*dtor)(void * /*this*/), void *object, void *handle)
    {
        return 1;
    }

    void __cxa_finalize(void *handle)
    {
    }

} // end of extern "C"

#endif


#if defined(TOOLCHAIN_GCC)

/*
 * Depending on how newlib is  configured, it is often not enough to define
 * __aeabi_atexit, __cxa_atexit and __cxa_finalize in order to override the
 * behavior regarding the registration of handlers with atexit.
 *
 * To overcome this limitation, exit and atexit are overriden here.
 */
extern "C" {

    /**
     * @brief Retarget of exit for GCC.
     * @details Unlike the standard version, this function doesn't call any function
     * registered with atexit before calling _exit.
     */
    void __wrap_exit(int return_code)
    {
        _exit(return_code);
    }

    /**
     * @brief Retarget atexit from GCC.
     * @details This function will always fail and never register any handler to be
     * called at exit.
     */
    int __wrap_atexit(void (*func)())
    {
        return 1;
    }

}

#endif



namespace mbed {

void mbed_set_unbuffered_stream(std::FILE *_file)
{
#if defined (__ICCARM__)
    char buf[2];
    std::setvbuf(_file, buf, _IONBF, NULL);
#else
    setbuf(_file, NULL);
#endif
}

} // namespace mbed

#if defined (__ICCARM__)
// Stub out locks when an rtos is not present
extern "C" WEAK void __iar_system_Mtxinit(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_system_Mtxdst(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_system_Mtxlock(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_system_Mtxunlock(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_file_Mtxinit(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_file_Mtxdst(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_file_Mtxlock(__iar_Rmtx *mutex) {}
extern "C" WEAK void __iar_file_Mtxunlock(__iar_Rmtx *mutex) {}
#if defined(__IAR_SYSTEMS_ICC__ ) && (__VER__ >= 8000000)
#pragma section="__iar_tls$$DATA"
extern "C" WEAK void *__aeabi_read_tp(void)
{
    // Thread Local storage is not supported, using main thread memory for errno
    return __section_begin("__iar_tls$$DATA");
}
#endif
#elif defined(__CC_ARM)
// Do nothing
#elif defined (__GNUC__)
struct _reent;
// Stub out locks when an rtos is not present
extern "C" WEAK void __rtos_malloc_lock(struct _reent *_r) {}
extern "C" WEAK void __rtos_malloc_unlock(struct _reent *_r) {}
extern "C" WEAK void __rtos_env_lock(struct _reent *_r) {}
extern "C" WEAK void __rtos_env_unlock(struct _reent *_r) {}

extern "C" void __malloc_lock(struct _reent *_r)
{
    __rtos_malloc_lock(_r);
}

extern "C" void __malloc_unlock(struct _reent *_r)
{
    __rtos_malloc_unlock(_r);
}

extern "C" void __env_lock(struct _reent *_r)
{
    __rtos_env_lock(_r);
}

extern "C" void __env_unlock(struct _reent *_r)
{
    __rtos_env_unlock(_r);
}

#endif

#if defined (__GNUC__) || defined (__ARMCC_VERSION)

#define CXA_GUARD_INIT_DONE             (1 << 0)
#define CXA_GUARD_INIT_IN_PROGRESS      (1 << 1)
#define CXA_GUARD_MASK                  (CXA_GUARD_INIT_DONE | CXA_GUARD_INIT_IN_PROGRESS)

extern "C" int __cxa_guard_acquire(int *guard_object_p)
{
    uint8_t *guard_object = (uint8_t *)guard_object_p;
    if ((core_util_atomic_load_u8(guard_object) & CXA_GUARD_MASK) == CXA_GUARD_INIT_DONE) {
        return 0;
    }
    singleton_lock();
    uint8_t guard = *guard_object;
    if ((guard & CXA_GUARD_MASK) == CXA_GUARD_INIT_DONE) {
        singleton_unlock();
        return 0;
    }
    MBED_ASSERT((guard & CXA_GUARD_MASK) == 0);
    core_util_atomic_store_u8(guard_object, guard | CXA_GUARD_INIT_IN_PROGRESS);
    return 1;
}

extern "C" void __cxa_guard_release(int *guard_object_p)
{
    uint8_t *guard_object = (uint8_t *)guard_object_p;
    uint8_t guard = *guard_object;
    MBED_ASSERT((guard & CXA_GUARD_MASK) == CXA_GUARD_INIT_IN_PROGRESS);
    core_util_atomic_store_u8(guard_object, (guard & ~CXA_GUARD_MASK) | CXA_GUARD_INIT_DONE);
    singleton_unlock();
}

extern "C" void __cxa_guard_abort(int *guard_object_p)
{
    uint8_t *guard_object = (uint8_t *)guard_object_p;
    uint8_t guard = *guard_object;
    MBED_ASSERT((guard & CXA_GUARD_MASK) == CXA_GUARD_INIT_IN_PROGRESS);
    core_util_atomic_store_u8(guard_object, guard & ~CXA_GUARD_INIT_IN_PROGRESS);
    singleton_unlock();
}

#endif

#if defined(MBED_MEM_TRACING_ENABLED) && (defined(__ARMCC_VERSION) || defined(__ICCARM__))

// If the memory tracing is enabled, the wrappers in mbed_alloc_wrappers.cpp
// provide the implementation for these. Note: this needs to use the wrappers
// instead of malloc()/free() as the caller address would point to wrappers,
// not the caller of "new" or "delete".
extern "C" void *malloc_wrapper(size_t size, const void *caller);
extern "C" void free_wrapper(void *ptr, const void *caller);

void *operator new (std::size_t count)
{
    void *buffer = malloc_wrapper(count, MBED_CALLER_ADDR());
    if (NULL == buffer) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_OUT_OF_MEMORY), "Operator new out of memory\r\n", count);
    }
    return buffer;
}

void *operator new[](std::size_t count)
{
    void *buffer = malloc_wrapper(count, MBED_CALLER_ADDR());
    if (NULL == buffer) {
        error("Operator new[] out of memory\r\n");
    }
    return buffer;
}

void *operator new (std::size_t count, const std::nothrow_t &tag)
{
    return malloc_wrapper(count, MBED_CALLER_ADDR());
}

void *operator new[](std::size_t count, const std::nothrow_t &tag)
{
    return malloc_wrapper(count, MBED_CALLER_ADDR());
}

void operator delete (void *ptr)
{
    free_wrapper(ptr, MBED_CALLER_ADDR());
}
void operator delete[](void *ptr)
{
    free_wrapper(ptr, MBED_CALLER_ADDR());
}

#elif defined(MBED_MEM_TRACING_ENABLED) && defined(__GNUC__)

#include <reent.h>

extern "C" void *malloc_wrapper(struct _reent *r, size_t size, void *caller);
extern "C" void free_wrapper(struct _reent *r, void *ptr, void *caller);

void *operator new (std::size_t count)
{
    void *buffer = malloc_wrapper(_REENT, count, MBED_CALLER_ADDR());
    if (NULL == buffer) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_OUT_OF_MEMORY), "Operator new out of memory\r\n", count);
    }
    return buffer;
}

void *operator new[](std::size_t count)
{
    void *buffer = malloc_wrapper(_REENT, count, MBED_CALLER_ADDR());
    if (NULL == buffer) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_OUT_OF_MEMORY), "Operator new out of memory\r\n", count);
    }
    return buffer;
}

void *operator new (std::size_t count, const std::nothrow_t &tag)
{
    return malloc_wrapper(_REENT, count, MBED_CALLER_ADDR());
}

void *operator new[](std::size_t count, const std::nothrow_t &tag)
{
    return malloc_wrapper(_REENT, count, MBED_CALLER_ADDR());
}

void operator delete (void *ptr)
{
    free_wrapper(_REENT, ptr, MBED_CALLER_ADDR());
}

void operator delete[](void *ptr)
{
    free_wrapper(_REENT, ptr, MBED_CALLER_ADDR());
}

#else

void *operator new (std::size_t count)
{
    void *buffer = malloc(count);
    if (NULL == buffer) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_OUT_OF_MEMORY), "Operator new out of memory\r\n", count);
    }
    return buffer;
}

void *operator new[](std::size_t count)
{
    void *buffer = malloc(count);
    if (NULL == buffer) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_OUT_OF_MEMORY), "Operator new[] out of memory\r\n", count);
    }
    return buffer;
}

void *operator new (std::size_t count, const std::nothrow_t &tag)
{
    return malloc(count);
}

void *operator new[](std::size_t count, const std::nothrow_t &tag)
{
    return malloc(count);
}

void operator delete (void *ptr)
{
    free(ptr);
}
void operator delete[](void *ptr)
{
    free(ptr);
}

#endif

/* @brief   standard c library clock() function.
 *
 * This function returns the number of clock ticks elapsed since the start of the program.
 *
 * @note Synchronization level: Thread safe
 *
 * @return
 *  the number of clock ticks elapsed since the start of the program.
 *
 * */
extern "C" clock_t clock()
{
    _mutex->lock();
    clock_t t = ticker_read(get_us_ticker_data());
    t /= 1000000 / CLOCKS_PER_SEC; // convert to processor time
    _mutex->unlock();
    return t;
}

// temporary - Default to 1MHz at 32 bits if target does not have us_ticker_get_info
MBED_WEAK const ticker_info_t *us_ticker_get_info()
{
    static const ticker_info_t info = {
        1000000,
        32
    };
    return &info;
}

// temporary - Default to 1MHz at 32 bits if target does not have lp_ticker_get_info
MBED_WEAK const ticker_info_t *lp_ticker_get_info()
{
    static const ticker_info_t info = {
        1000000,
        32
    };
    return &info;
}
