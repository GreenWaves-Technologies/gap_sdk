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

#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "cmsis.h"
#include "PortNames.h"
#include "PeripheralNames.h"
#include "PinNames.h"
#include "gap_common.h"
#include "gap_fc_event.h"
#include "spi_regs.h"
#include "spi_multi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gpio_irq_s {
    uint32_t port;
    uint32_t pin;
    uint32_t ch;
};

struct port_s {
    PortName port;
    uint32_t mask;
};

struct pwmout_s {
    PWMName pwm_name;
    uint8_t prescale;
    uint8_t clock_src;
};

struct serial_s {
    int index;
#if DEVICE_SERIAL_ASYNCH
    uint8_t txstate;
    uint8_t rxstate;
    uint32_t events;
    uart_handle_t uart_transfer_handle;
    uint32_t uart_dma_handle;
    uint32_t uartDmaTx;
    uint32_t uartDmaRx;
#endif
};

struct i2c_s {
    uint32_t instance;
    uint8_t next_repeated_start;
};

struct i2s_s {
    uint32_t instance;
};

struct hyperbus_s {
    uint32_t instance;
    status_t status;
#if DEVICE_HYPERBUS_ASYNCH
    hyperbus_handle_t hyperbus_handle;
    uint32_t hyperbusDmaMasterRx;
    uint32_t hyperbusDmaMasterTx;
#endif
};

#if DEVICE_QSPI
#define qspi_s spi_s
#endif

#include "gpio_object.h"

#ifdef __cplusplus
}
#endif

#endif
