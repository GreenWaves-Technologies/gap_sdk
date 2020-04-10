/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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
#ifndef _PMSIS_UDMA_UART_H_
#define _PMSIS_UDMA_UART_H_

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"


/*! UART udma configuration. */
static inline void uart_udma_channel_set(uint32_t device_id, uint32_t l2_buf,
                                         uint32_t size, uint32_t cfg,
                                         udma_channel_e channel)
{
    udma_enqueue_channel(&(uart(device_id)->udma), l2_buf, size, cfg, channel);
}

/*! Status Register. */
static inline uint32_t uart_status_get(uint32_t device_id)
{
    return hal_read32(&(uart(device_id)->status));
}


/*! Setup Register. */
static inline void uart_setup_set(uint32_t device_id, uint32_t setup)
{
    hal_write32(&(uart(device_id)->setup), setup);
}

static inline uint32_t uart_setup_get(uint32_t device_id)
{
    return hal_read32(&(uart(device_id)->setup));
}

/*! SETUP. */
/* Parity setup. */
static inline void hal_uart_parity_enable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) | UART_SETUP_PARITY_ENA(1)));
}

static inline void hal_uart_parity_disable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_PARITY_ENA(1)));
}

/* Word size. */
static inline void hal_uart_bit_length_set(uint32_t device_id, uint8_t bit_length)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_BIT_LENGTH_MASK)
                              | UART_SETUP_BIT_LENGTH(bit_length));
}

/* Stop bits. */
static inline void hal_uart_stop_bits_set(uint32_t device_id, uint8_t stop_bits)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_STOP_BITS_MASK)
                              | UART_SETUP_STOP_BITS(stop_bits));
}

/* TX enable. */
static inline void hal_uart_tx_enable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) | UART_SETUP_TX_ENA(1)));
}

static inline void hal_uart_tx_disable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_TX_ENA(1)));
}

/* RX enable. */
static inline void hal_uart_rx_enable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) | UART_SETUP_RX_ENA(1)));
}

static inline void hal_uart_rx_disable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_RX_ENA(1)));
}

/* Clock divider setup. */
static inline void hal_uart_clkdiv_set(uint32_t device_id, uint16_t clk_div)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_CLKDIV_MASK)
                              | UART_SETUP_CLKDIV(clk_div));
}

/* Setup UART. */
static inline void hal_uart_setup_set(uint32_t device_id, uint16_t clk_div,
                                      uint8_t rx_ena, uint8_t tx_ena,
                                      uint8_t stop_bits, uint8_t bit_length,
                                      uint8_t parity_ena)
{
    uint32_t setup = UART_SETUP_CLKDIV(clk_div) |
                     UART_SETUP_RX_ENA(rx_ena) |
                     UART_SETUP_TX_ENA(tx_ena) |
                     UART_SETUP_STOP_BITS(stop_bits) |
                     UART_SETUP_BIT_LENGTH(bit_length) |
                     UART_SETUP_PARITY_ENA(parity_ena);
    uart_setup_set(device_id, setup);
}


/*! STATUS.  */
static inline uint32_t hal_uart_tx_status_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_TX_BUSY_MASK);
}

static inline uint32_t hal_uart_rx_status_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_RX_BUSY_MASK);
}

static inline uint32_t hal_uart_rx_parity_error_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_RX_PE_MASK);
}


/*! UDMA. */
static inline void hal_uart_enqueue(uint32_t device_id, uint32_t l2_buf,
                                    uint32_t size, uint32_t cfg, udma_channel_e channel)
{
    cfg |= UDMA_CORE_RX_CFG_EN(1);
    uart_udma_channel_set(device_id, l2_buf, size, cfg, channel);
}

static inline void hal_uart_rx_clear(uint32_t device_id)
{
    udma_channel_clear(&(uart(device_id)->udma), RX_CHANNEL);
}

static inline void hal_uart_tx_clear(uint32_t device_id)
{
    udma_channel_clear(&(uart(device_id)->udma), TX_CHANNEL);
}

#endif
