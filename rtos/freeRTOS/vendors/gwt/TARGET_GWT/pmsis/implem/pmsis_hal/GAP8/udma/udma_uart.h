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

#include "targets/pmsis_targets.h"
#include "pmsis_hal/GAP8/udma/udma_core.h"
#include "pmsis_hal/GAP8/udma/udma_ctrl.h"

#define UART_PTR(id) ((uart_t*)UART_BASE)
#ifndef UDMA_UART_ID
#define UDMA_UART_ID(id) (id + UDMA_UART_BASE_ID)
#endif
/*!
 * @addtogroup uart_driver
 * @{
 */

/*! @brief Error codes for the UART driver. */
enum uart_status_e
{
    STATUS_UART_TX_BUSY,              /*!< Transmitter is busy. */
    STATUS_UART_RX_BUSY,              /*!< Receiver is busy. */
    STATUS_UART_TX_IDLE,              /*!< UART transmitter is idle. */
    STATUS_UART_RX_IDLE,              /*!< UART receiver is idle. */
    STATUS_UART_ERROR,               /*!< Error happened on UART. */
    STATUS_UART_PARITY_ERROR,        /*!< UART parity error. */
    Status_UART_BaudrateNotSupport, /*!< Baudrate is not supported in current clock source */
};

/*! @brief UART parity mode. */
typedef enum uart_parity_mode_e
{
    UART_PARITY_DISABLED = 0x0U, /*!< Parity disabled */
    UART_PARITY_ENABLED = 0x1U,    /*!< Parity enabled */
} pi_uart_parity_mode_t;

/*! @brief UART bit length. */
typedef enum uart_bit_length_e
{
    UART_5_BITS = 0x0U, /*!< 5 bits length */
    UART_6_BITS = 0x1U, /*!< 6 bits length */
    UART_7_BITS = 0x2U, /*!< 7 bits length */
    UART_8_BITS = 0x3U, /*!< 8 bits length */
} pi_uart_bit_length_t;

/*! @brief UART stop bit count. */
typedef enum uart_stop_bit_count_e
{
    UART_ONE_STOP_BIT = 0U, /*!< One stop bit */
    UART_TWO_STOP_BIT = 1U, /*!< Two stop bits */
} pi_uart_stop_bit_count_t;

/*!
 * @brief UART interrupt configuration structure, default settings all disabled.
 *
 * This structure contains the settings for all of the UART interrupt configurations.
 */
enum uart_interrupt_enable_e
{
    UART_RX_END_INTERRUPT_ENABLE = 0,   /*!< RX active edge interrupt. */
    UART_TX_END_INTERRUPT_ENABLE = 1    /*!< TX active edge interrupt. */
};

// FIXME: use builtins for mask!!!
static void uart_set_baud_rate(uart_t *base, uint32_t baudrate_bps,
        uint32_t src_clock_Hz)
{
    /* Calculate the baud rate modulo divisor, sbr*/
    uint16_t div = src_clock_Hz / baudrate_bps;

    base->SETUP = (base->SETUP & ~(UART_SETUP_CLKDIV_MASK)) | UART_SETUP_CLKDIV(div);
}

static void udma_uart_init(uint32_t uart_id, uint32_t stop_bit_count,
        uint32_t parity_mode, uint8_t enable_tx, uint8_t enable_rx,
        uint32_t baudrate_bps, uint32_t src_clock_Hz)
{
    udma_ctrl_cg_disable(UDMA_UART_ID(uart_id));
    // Set the config here
    UART_PTR(uart_id)->SETUP = UART_SETUP_BIT_LENGTH(UART_8_BITS) |
        UART_SETUP_STOP_BITS(stop_bit_count) |
        UART_SETUP_PARITY_ENA(parity_mode) |
        UART_SETUP_TX_ENA(enable_tx) |
        UART_SETUP_RX_ENA(enable_rx);

    uart_set_baud_rate(UART_PTR(uart_id), baudrate_bps, src_clock_Hz);

    return;
}

/*!
 * @brief Deinitializes a UART instance.
 *
 * This function waits for TX complete, disables TX and RX, and disables the UART clock.
 *
 * @param base UART peripheral base address.
 */
static void udma_uart_deinit(uint32_t uart_id)
{
    udma_ctrl_cg_enable(UDMA_UART_ID(uart_id));
    return;
}


static inline void udma_uart_enqueue_transfer(int uart_id, void *addr,
        uint32_t size, udma_channel_e channel)
{
    udma_enqueue_channel(&UART_PTR(uart_id)->udma_uart, (uint32_t)addr, size,
            UDMA_CFG_EN(1), channel);
}


/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets UART status flags.
 *
 * @param base UART peripheral base address.
 * @return UART status flags.
 */
static inline uint32_t uart_get_status_flags(uart_t *base)
{
    return base->STATUS;
}

/*!
 * @brief Get TX transfer status.
 *
 * @param uart_id ID of uart periph.
 *
 * @return TX status flag.
 */
static inline uint32_t uart_get_tx_status(uint32_t uart_id)
{
    return (UART_PTR(uart_id)->STATUS & 0x1);
}

/*!
 * @brief Get RX transfer status.
 *
 * @param uart_id ID of uart periph.
 *
 * @return RX status flag.
 */
static inline uint32_t uart_get_rx_status(uint32_t uart_id)
{
    return ((UART_PTR(uart_id)->STATUS >> 1 ) & 0x1);
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */


/* @} */


/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the UART handle.
 *
 * This function initializes the UART handle which can be used for other UART
 * transactional APIs. Usually, for a specified UART instance,
 * call this API once to get the initialized handle.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 */

//void uart_transfer_abort_send(uart_t *base, uart_handle_t *handle);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to know
 * how many bytes are not received yet.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
//void uart_transfer_abort_receive(uart_t *base, uart_handle_t *handle);

/* @} */

/*!
 * @brief Enables or disables the UART transmitter.
 *
 * This function enables or disables the UART transmitter.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void uart_enable_tx(uart_t *base, uint8_t enable)
{
    if (enable)
    {
        base->SETUP |= UART_SETUP_TX_ENA(1);
    }
    else
    {
        base->SETUP &= ~UART_SETUP_TX_ENA(1);
    }
}

/*!
 * @brief Enables or disables the UART receiver.
 *
 * This function enables or disables the UART receiver.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void uart_enable_rx(uart_t *base, uint8_t enable)
{
    if (enable)
    {
        base->SETUP |= UART_SETUP_RX_ENA(1);
    }
    else
    {
        base->SETUP &= ~UART_SETUP_RX_ENA(1);
    }
}

/*!
 * @brief Get the RX parity error state.
 *
 * This function test the STATUS register RX parity error bit
 * Clear parity error bit when STATUS register is red
 *
 * @param base UART peripheral base address.
 */
static inline  uint32_t uart_rx_parity_error(uart_t *base)
{
    return (base->STATUS & UART_STATUS_RX_PE_MASK);
}

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _GAP_UART_H_ */
