/*
 * Copyright (c) 2018 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <stdarg.h>
#include "platform/FileHandle.h"
#include "drivers/UARTSerial.h"

/**
 * Macros for setting console flow control.
 */
#define CONSOLE_FLOWCONTROL_RTS     1
#define CONSOLE_FLOWCONTROL_CTS     2
#define CONSOLE_FLOWCONTROL_RTSCTS  3
#define mbed_console_concat_(x) CONSOLE_FLOWCONTROL_##x
#define mbed_console_concat(x) mbed_console_concat_(x)
#define CONSOLE_FLOWCONTROL mbed_console_concat(MBED_CONF_TARGET_CONSOLE_UART_FLOW_CONTROL)

#define SERIAL_CONSOLE_BAUD_RATE 115200

mbed::FileHandle *mbed::mbed_override_console(int)
{
    static mbed::UARTSerial console(STDIO_UART_TX, STDIO_UART_RX,
                                    SERIAL_CONSOLE_BAUD_RATE);
#if CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
    mbed::console.set_flow_control(SerialBase::RTS, STDIO_UART_RTS, NC);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
    mbed::console.set_flow_control(SerialBase::CTS, NC, STDIO_UART_CTS);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
    mbed::console.set_flow_control(SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
#endif
    return &console;
}
