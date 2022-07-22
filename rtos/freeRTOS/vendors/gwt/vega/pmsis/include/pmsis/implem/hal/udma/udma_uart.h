/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#pragma once

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"


/* UART udma configuration. */
static inline void uart_udma_channel_set(uint32_t device_id, uint32_t l2_buf,
                                         uint32_t size, uint32_t cfg,
                                         udma_channel_e channel)
{
    hal_udma_enqueue_channel((udma_core_cmd_t *) &(uart(device_id)->udma),
                             l2_buf, size, cfg, channel);
}

/* STATUS register. */
static inline uint32_t uart_status_get(uint32_t device_id)
{
    return GAP_READ(&(uart(device_id)->status), 0);
}

/* SETUP register. */
static inline void uart_setup_set(uint32_t device_id, uint32_t setup)
{
    GAP_WRITE(&(uart(device_id)->setup), 0, setup);
}

static inline uint32_t uart_setup_get(uint32_t device_id)
{
    return GAP_READ(&(uart(device_id)->setup), 0);
}

/* ERROR register. */
static inline uint32_t uart_error_get(uint32_t device_id)
{
    return GAP_READ(&(uart(device_id)->error), 0);
}

/* IRQ_EN register. */
static inline void uart_irq_en_set(uint32_t device_id, uint32_t irq_en)
{
    GAP_WRITE(&(uart(device_id)->irq_en), 0, irq_en);
}

static inline uint32_t uart_irq_en_get(uint32_t device_id)
{
    return GAP_READ(&(uart(device_id)->irq_en), 0);
}

/* VALID register. */
static inline void uart_valid_set(uint32_t device_id, uint32_t valid)
{
    GAP_WRITE(&(uart(device_id)->valid), 0, valid);
}

static inline uint32_t uart_valid_get(uint32_t device_id)
{
    return GAP_READ(&(uart(device_id)->valid), 0);
}

/* DATA register. */
static inline void uart_data_set(uint32_t device_id, uint32_t data)
{
    GAP_WRITE(&(uart(device_id)->data), 0, data);
}

static inline uint32_t uart_data_get(uint32_t device_id)
{
    return GAP_READ(&(uart(device_id)->data), 0);
}


/** STATUS. */
static inline uint32_t hal_udma_uart_status_get(uint32_t device_id)
{
    return uart_status_get(device_id);
}

static inline uint32_t hal_udma_uart_tx_status_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_TX_BUSY_MASK);
}

static inline uint32_t hal_udma_uart_rx_status_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_RX_BUSY_MASK);
}


/** SETUP. */
static inline uint32_t hal_udma_uart_setup_get(uint32_t device_id)
{
    return uart_setup_get(device_id);
}

static inline void hal_udma_uart_setup_mask_set(uint32_t device_id,
                                                uint32_t setup)
{
    uart_setup_set(device_id, setup);
}

static inline void hal_udma_uart_setup_set(uint32_t device_id, uint8_t parity_ena,
                                           uint8_t bit_length, uint8_t stop_bits,
                                           uint8_t clean_fifo, uint8_t polling_en,
                                           uint8_t tx_ena, uint8_t rx_ena,
                                           uint16_t clkdiv)
{
    uart_setup_t setup = { .word = 0 };
    setup.field.parity_ena = parity_ena;
    setup.field.bit_length = bit_length;
    setup.field.stop_bits = stop_bits;
    setup.field.clean_fifo = clean_fifo;
    setup.field.polling_en = polling_en;
    setup.field.tx_ena = tx_ena;
    setup.field.rx_ena = rx_ena;
    setup.field.clkdiv = clkdiv;
    uart_setup_set(device_id, setup.word);
}

static inline void hal_udma_uart_tx_enable(uint32_t device_id)
{
    uart_setup_t setup = { .word = 0 };
    setup.word = uart_setup_get(device_id);
    setup.field.tx_ena = 1;
    uart_setup_set(device_id, setup.word);
}

static inline void hal_udma_uart_tx_disable(uint32_t device_id)
{
    uart_setup_t setup = { .word = 0 };
    setup.word = uart_setup_get(device_id);
    setup.field.tx_ena = 0;
    uart_setup_set(device_id, setup.word);
}

static inline void hal_udma_uart_rx_enable(uint32_t device_id)
{
    uart_setup_t setup = { .word = 0 };
    setup.word = uart_setup_get(device_id);
    setup.field.rx_ena = 1;
    uart_setup_set(device_id, setup.word);
}

static inline void hal_udma_uart_rx_disable(uint32_t device_id)
{
    uart_setup_t setup = { .word = 0 };
    setup.word = uart_setup_get(device_id);
    setup.field.rx_ena = 0;
    uart_setup_set(device_id, setup.word);
}

static inline void hal_udma_uart_rx_fifo_clean(uint32_t device_id)
{
    uart_setup_t setup = { .word = 0 };
    setup.word = uart_setup_get(device_id);
    setup.field.clean_fifo = 1;
    uart_setup_set(device_id, setup.word);
    hal_compiler_barrier();
    setup.field.clean_fifo = 0;
    uart_setup_set(device_id, setup.word);
}

static inline void hal_udma_uart_clk_div_set(uint32_t device_id, uint16_t clkdiv)
{
    uart_setup_t setup = { .word = 0 };
    setup.word = uart_setup_get(device_id);
    setup.field.clkdiv = clkdiv;
    uart_setup_set(device_id, setup.word);
}


/** ERROR. */
static inline uint32_t hal_udma_uart_error_get(uint32_t device_id)
{
    return uart_error_get(device_id);
}


/** IRQ_EN. */
static inline void hal_udma_uart_irq_enable(uint32_t device_id)
{
    //uint32_t irq = UART_IRQ_EN_RX_MASK | UART_IRQ_EN_ERROR_MASK;
    uint32_t irq = UART_IRQ_EN_ERROR_MASK;
    uart_irq_en_set(device_id, irq);
}

static inline void hal_udma_uart_irq_disable(uint32_t device_id)
{
    uart_irq_en_set(device_id, 0);
}


/** Polling method. */
/** VALID. */
static inline uint32_t hal_udma_uart_valid_get(uint32_t device_id)
{
    return uart_valid_get(device_id);
}


/** DATA. */
static inline uint32_t hal_udma_uart_data_get(uint32_t device_id)
{
    return uart_data_get(device_id);
}


/*! UDMA. */
static inline void hal_udma_uart_enqueue(uint32_t device_id, uint32_t l2_buf,
                                         uint32_t size, uint32_t cfg,
                                         udma_channel_e channel)
{
    uart_udma_channel_set(device_id, l2_buf, size, cfg, channel);
}

static inline void hal_udma_uart_rx_clear(uint32_t device_id)
{
    hal_udma_channel_clear((udma_core_cmd_t *) &(uart(device_id)->udma), RX_CHANNEL);
}

static inline void hal_udma_uart_tx_clear(uint32_t device_id)
{
    hal_udma_channel_clear((udma_core_cmd_t *) &(uart(device_id)->udma), TX_CHANNEL);
}
