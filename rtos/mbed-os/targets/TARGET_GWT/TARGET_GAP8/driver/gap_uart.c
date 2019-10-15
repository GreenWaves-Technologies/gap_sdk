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

#include "gap_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_TX_BUFFER_SIZE     16

uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];
uint8_t uart_rx_buffer[1];
/* Indicate whether uart is initialed */
uint8_t uart_is_init = 0;


/* Maximum write datasize in byte is 65536 */
uart_transfer_t TX_INFO = {
    .dataAddr  = (uint32_t)uart_tx_buffer,
    .dataSize  = 1,
    .isTx      = 1,
    .channelId = UDMA_EVENT_UART_TX,
    .configFlags = 0,
    .task = 0,
    .repeat.size = 0,
    .ctrl = 0
};

/* Maximum read datasize in byte is 65536 */
uart_transfer_t RX_INFO = {
    .dataAddr  = (uint32_t)uart_rx_buffer,
    .dataSize  = 1,
    .isTx      = 0,
    .channelId = UDMA_EVENT_UART_RX,
    .configFlags = 0,
    .task = 0,
    .repeat.size = 0,
    .ctrl = 0
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/
#if (defined(UART))
#define UART_HANDLE_ARRAY_SIZE 1
#else /* UART */
#error No UART instance.
#endif /* UART */

static uart_handle_t *s_uartHandle[UART_HANDLE_ARRAY_SIZE];
/* Array of UART peripheral base address. */
static UART_Type *const s_uartBases[] = UART_BASE_PTRS;

/* UART ISR for transactional APIs. */
static uart_isr_t s_uartIsr;
/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t UART_GetInstance(UART_Type *base)
{
    uint32_t instance;
    uint32_t uartArrayCount = (sizeof(s_uartBases) / sizeof(s_uartBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < uartArrayCount; instance++)
    {
        if (s_uartBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < uartArrayCount);

    return instance;
}

void UART_GetDefaultConfig(uart_config_t *config)
{
    assert(config);

    config->baudRate_Bps = 9600U;
    config->parityMode   = uUART_ParityDisabled;

    config->stopBitCount = uUART_OneStopBit;

    config->enableTx = false;
    config->enableRx = false;
}


status_t UART_Init(UART_Type *base, const uart_config_t *config, uint32_t srcClock_Hz)
{
    UDMA_Init((UDMA_Type *)base);

    /* 8N1*/
    base->SETUP = UART_SETUP_BIT_LENGTH(uUART_8bits) |
        UART_SETUP_STOP_BITS(config->stopBitCount) |
        UART_SETUP_PARITY_ENA(config->parityMode) |
        UART_SETUP_TX_ENA(config->enableTx) |
        UART_SETUP_RX_ENA(config->enableRx);


    UART_SetBaudRate(base, config->baudRate_Bps, srcClock_Hz);

    uart_is_init = 1;
    return 0;
}

status_t UART_SetBaudRate(UART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz)
{
    assert(baudRate_Bps);

    /* Calculate the baud rate modulo divisor, sbr*/
    uint16_t div = srcClock_Hz / baudRate_Bps;

    base->SETUP = (base->SETUP & ~(UART_SETUP_CLKDIV_MASK)) | UART_SETUP_CLKDIV(div);

    return 0;
}

void UART_Deinit(UART_Type *base)
{
    /* Wait TX end */
    while (UART_TXBusy(base));

    /* Disable TX RX */
    base->SETUP = 0;

    UDMA_Deinit((UDMA_Type *)base);

    uart_is_init = 0;
}

uint32_t UART_GetStatusFlags(UART_Type *base)
{
    return base->STATUS;
}

void UART_WriteByte(UART_Type *base, uint8_t data)
{
    /* setup next transfer */
    uart_tx_buffer[0] = data;

    UART_TransferSendBlocking(base, uart_tx_buffer, 1);
}

uint8_t UART_ReadByte(UART_Type *base)
{
    UART_TransferReceiveBlocking(base, uart_rx_buffer, 1);
    return uart_rx_buffer[0];
}

status_t UART_TransferSendBlocking(UART_Type *base, const uint8_t *tx, size_t tx_length)
{
    status_t status;
    udma_req_info_t info = TX_INFO;

    info.dataAddr = (uint32_t)tx;
    info.dataSize = tx_length;
    info.configFlags |= UDMA_CFG_EN(1);

    status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);

    return status;
}

status_t UART_TransferReceiveBlocking(UART_Type *base, const uint8_t *rx, size_t rx_length)
{
    status_t status;
    udma_req_info_t info = RX_INFO;

    info.dataAddr = (uint32_t)rx;
    info.dataSize = rx_length;
    info.configFlags |= UDMA_CFG_EN(1);

    status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);

    return status;
}

status_t UART_TransferSendNonBlocking(UART_Type *base, uart_handle_t *handle, const uint8_t *tx, size_t tx_length)
{
    assert(handle);
    assert(tx);
    assert(tx_length);

    status_t status;

    /* Return error if current TX busy. */
    if (uUART_TxBusy == handle->txState)
    {
        status = uStatus_UART_TxBusy;
    }
    else
    {
        uart_req_t *TX = UDMA_FindAvailableRequest();

        TX->info = TX_INFO;
        TX->info.dataAddr = (uint32_t)tx;
        TX->info.dataSize = tx_length;
        TX->info.ctrl = 0;
        TX->info.task = 1;

        handle->txData = (uint8_t *)tx;
        handle->txDataSize = tx_length;
        handle->txDataSizeAll = tx_length;
        handle->txState = uUART_TxBusy;

        UDMA_SendRequest((UDMA_Type*)base, TX);

        status = uStatus_Success;
    }

    return status;
}


status_t UART_TransferReceiveNonBlocking(UART_Type *base, uart_handle_t *handle, const uint8_t *rx, size_t rx_length)
{
    assert(handle);
    assert(rx);
    assert(rx_length);

    status_t status;

    if (uUART_RxBusy == handle->rxState)
    {
        status = uStatus_UART_RxBusy;
    }
    else
    {
        uart_req_t *RX = UDMA_FindAvailableRequest();

        RX->info = RX_INFO;
        RX->info.dataAddr = (uint32_t)rx;
        RX->info.dataSize = rx_length;
        RX->info.ctrl = 0;
        RX->info.task = 1;

        handle->rxData = (uint8_t *)rx;
        handle->rxDataSize = rx_length;
        handle->rxDataSizeAll = rx_length;
        handle->rxState = uUART_RxBusy;

        UDMA_SendRequest((UDMA_Type*)base, RX);

        status = uStatus_Success;
    }

    return status;
}

void UART_TransferCreateHandle(UART_Type *base,
                               uart_handle_t *handle,
                               uart_transfer_callback_t callback,
                               void *userData)
{
    assert(handle);

    uint32_t instance;

    /* Set the TX/RX state. */
    if ((int*)userData)
        handle->rxState = uUART_RxIdle;
    else
        handle->txState = uUART_TxIdle;

    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Get instance from peripheral base address. */
    instance = UART_GetInstance(base);

    /* Save the handle in global variables to support the double weak mechanism. */
    s_uartHandle[instance] = handle;

    s_uartIsr = UART_TransferHandleIRQ;
}

void UART_TransferAbortSend(UART_Type *base, uart_handle_t *handle)
{
    assert(handle);

    handle->txDataSize = 0;
    handle->txState = uUART_TxIdle;

    /* Abort from UDMA channel */
    UDMA_AbortSend((UDMA_Type *)base);
}

void UART_TransferAbortReceive(UART_Type *base, uart_handle_t *handle)
{
    assert(handle);

    handle->rxDataSize = 0U;
    handle->rxState = uUART_RxIdle;

    /* Abort from UDMA channel */
    UDMA_AbortReceive((UDMA_Type *)base);
}

void UART_TransferHandleIRQ(UART_Type *base, uart_handle_t *handle)
{
    assert(handle);

    /* If RX parity error */
    /* Automatically clear parity error flag, otherwise the RX does not work. */
    if (UART_RXParityError(base))
    {
        handle->rxState = uUART_RxParityError;
        handle->rxDataSize = 0U;
        /* Trigger callback. */
        if (handle->callback)
        {
            handle->callback(base, handle, uStatus_UART_ParityError, handle->userData);
        }
    }

    /* If all the data are red to memory, RX finished. */
    if (handle->rxDataSize && !UART_RXBusy(base))
    {
        handle->rxDataSize = 0;
        handle->rxState = uUART_RxIdle;

        if (handle->callback)
        {
            handle->callback(base, handle, uStatus_UART_RxIdle, handle->userData);
        }
    }

    /* If all the data are written to device, TX finished. */
    if (handle->txDataSize && !UART_TXBusy(base))
    {
        handle->txDataSize = 0;
        handle->txState = uUART_TxIdle;

        /* Trigger callback. */
        if (handle->callback)
        {
            handle->callback(base, handle, uStatus_UART_TxIdle, handle->userData);
        }
    }

}

#if defined(UART)
void UART_DriverIRQHandler(void)
{
    s_uartIsr(UART, s_uartHandle[0]);
}
#endif
