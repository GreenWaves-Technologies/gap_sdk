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
#include "serial_api.h"

#if DEVICE_SERIAL

// math.h required for floating point operations for baud rate calculation
#include <math.h>
#include "mbed_assert.h"

#include <string.h>
#include "gap_uart.h"
#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"

extern uint32_t fc_handler_vector[];

static uint32_t serial_irq_ids[] = {0};
static uart_irq_handler irq_handler;
/* Array of UART peripheral base address. */
static UART_Type *const uart_addrs[] = UART_BASE_PTRS;

/* UART transfer states */
#define uUART_TxIdle    0
#define uUART_TxBusy    1
#define uUART_RxIdle    2
#define uUART_RxBusy    3

int stdio_uart_inited = 0;
serial_t stdio_uart;

void serial_init(serial_t *obj, PinName tx, PinName rx)
{
    uint32_t uart_tx = pinmap_peripheral(tx, PinMap_UART_TX);
    uint32_t uart_rx = pinmap_peripheral(rx, PinMap_UART_RX);
    obj->serial.index = pinmap_merge(uart_tx, uart_rx);
    MBED_ASSERT((int)obj->serial.index != NC);

    uart_config_t config;

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = 9600;
    config.enableTx = false;
    config.enableRx = false;

    UART_Init(uart_addrs[obj->serial.index], &config, SystemCoreClock);

    pinmap_pinout(tx, PinMap_UART_TX);
    pinmap_pinout(rx, PinMap_UART_RX);

    if ((int)tx != NC) {
        UART_EnableTx(uart_addrs[obj->serial.index], true);
        pin_mode(tx, PullUp);
    }
    if ((int)rx != NC) {
        UART_EnableRx(uart_addrs[obj->serial.index], true);
        pin_mode(rx, PullUp);
    }

    if (obj->serial.index == STDIO_UART) {
        stdio_uart_inited = 1;
        memcpy(&stdio_uart, obj, sizeof(serial_t));
    }

    obj->serial.txstate = uUART_TxIdle;
    obj->serial.rxstate = uUART_RxIdle;

    /* Zero the handle. */
    memset(&(obj->serial.uart_transfer_handle), 0, sizeof(obj->serial.uart_transfer_handle));
}

void serial_free(serial_t *obj)
{
    UART_Deinit(uart_addrs[obj->serial.index]);
    serial_irq_ids[obj->serial.index] = 0;
}

void serial_baud(serial_t *obj, int baudrate)
{
    MBED_ASSERT(baudrate);

    UART_SetBaudRate(uart_addrs[obj->serial.index], (uint32_t)baudrate, SystemCoreClock);
}

void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits)
{
    UART_Type *base = uart_addrs[obj->serial.index];
    uint32_t temp = base->SETUP;

    /* Set bit count and parity mode. */
    temp &= ~(UART_SETUP_STOP_BITS_MASK | UART_SETUP_BIT_LENGTH_MASK | UART_SETUP_PARITY_ENA_MASK |
              UART_SETUP_TX_ENA_MASK | UART_SETUP_RX_ENA_MASK);

    if (parity != ParityNone)
    {
        temp |= UART_SETUP_PARITY_ENA(1);
    }

    if(stop_bits <= 2) {
        temp |= UART_SETUP_STOP_BITS(stop_bits - 1);
    } else {
        /* do not support more than two stop bits*/
        MBED_ASSERT(0);
    }

    if(data_bits == 5) {
        temp |= UART_SETUP_BIT_LENGTH(uUART_5bits);
    } else if(data_bits == 6) {
        temp |= UART_SETUP_BIT_LENGTH(uUART_6bits);
    } else if(data_bits == 7) {
        temp |= UART_SETUP_BIT_LENGTH(uUART_7bits);
    } else if(data_bits == 8) {
        temp |= UART_SETUP_BIT_LENGTH(uUART_8bits);
    } else {
        /* do not support other data bits*/
        MBED_ASSERT(0);
    }

    base->SETUP = temp | UART_SETUP_TX_ENA(1) | UART_SETUP_RX_ENA(1);
}

/******************************************************************************
 * INTERRUPTS HANDLING
 ******************************************************************************/
/* Do no support in GAP8 */
static inline void uart_irq_rx(uint32_t index)
{
    /* UART_Type *base = uart_addrs[index]; */

    if (serial_irq_ids[index] != 0) {
        irq_handler(serial_irq_ids[index], RxIrq);
    }
}

static inline void uart_irq_tx(uint32_t index)
{
    /* UART_Type *base = uart_addrs[index]; */

    if (serial_irq_ids[index] != 0) {
        irq_handler(serial_irq_ids[index], TxIrq);
    }
}

void uart0_irq_rx()
{
    uart_irq_rx(0);
}

void uart0_irq_tx()
{
    uart_irq_tx(0);
}

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id)
{
    irq_handler = handler;
    serial_irq_ids[obj->serial.index] = id;
}

void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable)
{
    if (enable)
    {
        if (irq == RxIrq) {
            //fc_handler_vector[UDMA_EVENT_UART_RX] = (uint32_t)&uart0_irq_rx;
        } else if (irq == TxIrq) {
            //fc_handler_vector[UDMA_EVENT_UART_TX] = (uint32_t)&uart0_irq_tx;
        }
    }
    else
    {
        if (irq == RxIrq) {
            //fc_handler_vector[UDMA_EVENT_UART_RX] = (uint32_t)NULL;
        } else if (irq == TxIrq) {
            //fc_handler_vector[UDMA_EVENT_UART_TX] = (uint32_t)NULL;
        }
    }
}

int serial_getc(serial_t *obj)
{
    /* while (!serial_readable(obj)); */
    uint8_t data;
    data = UART_ReadByte(uart_addrs[obj->serial.index]);

    return data;
}

void serial_putc(serial_t *obj, int c)
{
    /* while (!serial_writable(obj)); */
    UART_WriteByte(uart_addrs[obj->serial.index], (uint8_t)c);
}

int serial_readable(serial_t *obj)
{
    return !UART_RXBusy(uart_addrs[obj->serial.index]);
}

int serial_writable(serial_t *obj)
{
    return !UART_TXBusy(uart_addrs[obj->serial.index]);
}

void serial_clear(serial_t *obj)
{
}

void serial_pinout_tx(PinName tx)
{
    pinmap_pinout(tx, PinMap_UART_TX);
}

void serial_break_set(serial_t *obj)
{
    UART_Type *base = uart_addrs[obj->serial.index];
    uint16_t pre_div = (base->SETUP & UART_SETUP_CLKDIV_MASK) >> UART_SETUP_CLKDIV_SHIFT;

    /* Use a low half baudrate to send 0 to supply a break, about 2 frame time */
    while (!serial_writable(obj));
    UART_SetBaudRate(base, (SystemCoreClock / pre_div) >> 1, SystemCoreClock);
    serial_putc(obj, 0);
}

void serial_break_clear(serial_t *obj)
{
    UART_Type *base = uart_addrs[obj->serial.index];
    uint16_t pre_div = (base->SETUP & UART_SETUP_CLKDIV_MASK) >> UART_SETUP_CLKDIV_SHIFT;

    /* Wait TX end */
    while (!serial_writable(obj));
    /* Baudrate recover */
    UART_SetBaudRate(base, (SystemCoreClock / pre_div) << 1, SystemCoreClock);
}

void serial_enable_event(serial_t *obj, int event, uint8_t enable)
{
    // Keep track of the requested events.
    if (enable) {
        obj->serial.events |= event;
    } else {
        obj->serial.events &= ~event;
    }
}

static void serial_tx_buffer_set(serial_t *obj, void *tx, int tx_length, uint8_t width) {
    // We only support byte buffers for now
    MBED_ASSERT(width == 8);


    // Exit if a transmit is already on-going
    if (serial_tx_active(obj)) {
        return;
    }

    obj->tx_buff.buffer = tx;
    obj->tx_buff.length = tx_length;
    obj->tx_buff.pos = 0;
}

int serial_tx_asynch(serial_t *obj, const void *tx, size_t tx_length, uint8_t tx_width, uint32_t handler, uint32_t event, DMAUsage hint)
{
    // Check that a buffer has indeed been set up
    MBED_ASSERT(tx != (void*)0);

    if (tx_length == 0) return 0;

    if (serial_tx_active(obj)) {
        return 0;
    }

    // Set up buffer
    serial_tx_buffer_set(obj, (void *)tx, tx_length, tx_width);

    // Set up events
    serial_enable_event(obj, SERIAL_EVENT_TX_ALL, false);
    serial_enable_event(obj, event, true);

    UART_TransferCreateHandle(uart_addrs[obj->serial.index], &obj->serial.uart_transfer_handle, (uart_transfer_callback_t)handler, (void *)0);

    obj->serial.txstate = uUART_TxBusy;

    /* Start the transfer */
    UART_TransferSendNonBlocking(uart_addrs[obj->serial.index], &obj->serial.uart_transfer_handle, (const uint8_t *)tx, tx_length);

    return 0;
}


static void serial_rx_buffer_set(serial_t *obj, void *rx, int rx_length, uint8_t width)
{
    // We only support byte buffers for now
    MBED_ASSERT(width == 8);

    if (serial_rx_active(obj)) return;

    obj->rx_buff.buffer = rx;
    obj->rx_buff.length = rx_length;
    obj->rx_buff.pos = 0;

    return;
}

/* Character match is currently not supported */
void serial_rx_asynch(serial_t *obj, void *rx, size_t rx_length, uint8_t rx_width, uint32_t handler, uint32_t event, uint8_t char_match, DMAUsage hint)
{
    MBED_ASSERT(rx != (void*)0);

    if (rx_length == 0) return;

    if (serial_rx_active(obj)) {
        return;
    }

    // Set up buffer
    serial_rx_buffer_set(obj,(void*) rx, rx_length, rx_width);

    // Set up events
    serial_enable_event(obj, SERIAL_EVENT_RX_ALL, false);
    serial_enable_event(obj, event, true);

    UART_TransferCreateHandle(uart_addrs[obj->serial.index], &obj->serial.uart_transfer_handle, (uart_transfer_callback_t)handler, (void *)1);

    obj->serial.rxstate = uUART_RxBusy;

    /* Start the transfer */
    UART_TransferReceiveNonBlocking(uart_addrs[obj->serial.index], &obj->serial.uart_transfer_handle, (const uint8_t *)rx, rx_length);
}

uint8_t serial_tx_active(serial_t *obj)
{
    if (obj->serial.txstate == uUART_TxIdle) {
        return 0;
    }

    return 1;
}

uint8_t serial_rx_active(serial_t *obj)
{
    if (obj->serial.rxstate == uUART_RxIdle) {
        return 0;
    }

    return 1;
}

int serial_irq_handler_asynch(serial_t *obj)
{
    int status = 0;

    uint32_t status_flags = UART_GetStatusFlags(uart_addrs[obj->serial.index]);

    if ((obj->serial.txstate != uUART_TxIdle) && !UART_TXBusy(uart_addrs[obj->serial.index])) {
        obj->serial.txstate = uUART_TxIdle;
        status |= SERIAL_EVENT_TX_COMPLETE;
    }

    if ((obj->serial.rxstate != uUART_RxIdle) && !UART_RXBusy(uart_addrs[obj->serial.index])) {
        obj->serial.rxstate = uUART_RxIdle;
        status |= SERIAL_EVENT_RX_COMPLETE;
    }

    if (status_flags & UART_STATUS_RX_PE_MASK) {
        status |= SERIAL_EVENT_RX_PARITY_ERROR;
    }

    return status & obj->serial.events;
}

void serial_tx_abort_asynch(serial_t *obj)
{
    if (stdio_uart_inited)
    {
        UART_TransferAbortSend(uart_addrs[obj->serial.index], &obj->serial.uart_transfer_handle);
    }
}

void serial_rx_abort_asynch(serial_t *obj)
{
    if (stdio_uart_inited)
    {
        UART_TransferAbortReceive(uart_addrs[obj->serial.index], &obj->serial.uart_transfer_handle);
    }
}

#if ((defined (__RISCV_ARCH_GAP__ ) && (__RISCV_ARCH_GAP__ == 1)))
int serial_read_remain_size(const serial_t *obj)
{
    return UART_RXRemainBytes(uart_addrs[obj->serial.index]);
}

int serial_write_remain_size(const serial_t *obj)
{
    return UART_TXRemainBytes(uart_addrs[obj->serial.index]);
}
#endif

#endif
