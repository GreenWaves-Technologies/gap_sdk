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

#include "gap_hyperbus.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Typedef for interrupt handler. */
typedef void (*hyperbus_isr_t)(HYPERBUS_Type *base, hyperbus_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for HYPERBUS module.
 *
 * @param base HYPERBUS peripheral base address.
 */
uint32_t HYPERBUS_GetInstance(HYPERBUS_Type *base);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to hyperbus handles for each instance. */
static void *g_hyperbusHandle[ARRAY_SIZE(s_hyperbusBases)];

/*! @brief Pointer to IRQ handler for each instance. */
static hyperbus_isr_t s_hyperbusIsr;

/* Indicate whether hyperbus is initialed */
uint8_t hyperbus_is_init = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t HYPERBUS_GetInstance(HYPERBUS_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_hyperbusBases); instance++)
    {
        if (s_hyperbusBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_hyperbusBases));

    return instance;
}

void HYPERBUS_Init(HYPERBUS_Type *base, hyperbus_config_t *config, uint32_t srcClock_Hz)
{
    assert(config);

    /* Clock Gating */
    UDMA_Init((UDMA_Type *)base);

    if (config->mode == uHYPERBUS_HYPERBUS_MODE) {

        /* Configure device */
        HYPERBUS_SetDevice(base, uHYPERBUS_HYPERBUS_MODE, config->dt0, config->dt1);

        /* Set memory base address, RAM has volume of 8 Mbytes / 2 = 4M*/
        HYPERBUS_SetMBR0(base, config->mbr0);
        /* Flash address. start at 0x01000000 = 16M*/
        HYPERBUS_SetMBR1(base, config->mbr1);
    } else {
        HYPERBUS_SetDevice(base, uHYPERBUS_OCTOSPI_MODE, config->dt0, config->dt1);
    }

    HYPERBUS_FrequencyConfig(base, config->baudRate, SystemCoreClock);

    HYPERBUS_TransferIRQEnable(base, 1);

    hyperbus_is_init = 1;
}

void HYPERBUS_GetDefaultConfig(hyperbus_config_t *config)
{
    assert(config);

    config->mode     = uHYPERBUS_HYPERBUS_MODE;
    config->baudRate = 50000000U;

#if (__HYPERBUS_CSN0_FOR_RAM__ == 1)
    config->mbr0     = uHYPERBUS_Ram_Address;
    config->mbr1     = uHYPERBUS_Flash_Address >> 24;
    config->dt0      = uHYPERBUS_Ram;
    config->dt1      = uHYPERBUS_Flash;
#else
    config->mbr0     = uHYPERBUS_Flash_Address;
    config->mbr1     = uHYPERBUS_Ram_Address >> 24;
    config->dt0      = uHYPERBUS_Flash;
    config->dt1      = uHYPERBUS_Ram;
#endif
}

void HYPERBUS_FrequencyConfig(HYPERBUS_Type *base, uint32_t baudRate, uint32_t srcClock_Hz)
{
    assert(baudRate);

    int index = 0;

    uint32_t clkDivider = (srcClock_Hz >> 1) / baudRate;

    /* Enable the clock and set divider */
    base->CLK_DIV = HYPERBUS_CLK_DIV(clkDivider);
}

void HYPERBUS_DeInit(HYPERBUS_Type *base)
{
    UDMA_Deinit((UDMA_Type *)base);
    hyperbus_is_init = 0;
}

static int HYPERBUS_TransferTX(HYPERBUS_Type *base, int addr, const uint16_t *tx, size_t tx_length, char reg_access, char device)
{
    int32_t status;

    hyperbus_req_t *TX = UDMA_FindAvailableRequest();

    TX->info.dataAddr = (uint32_t) tx;
    TX->info.dataSize  = tx_length;
    TX->info.isTx      = 1;
    TX->info.channelId = UDMA_EVENT_HYPERBUS_TX;
    TX->info.configFlags = UDMA_CFG_DATA_SIZE(16 >> 4);
    TX->info.task        = 1;

    if (tx_length > 1024) {
        TX->info.repeat.size       = 1024;
        TX->info.repeat.size_total = tx_length;
        TX->info.dataSize          = 1024;
    }

    /* Hyperbus */
    if(device == uHYPERBUS_Ram) {
        TX->info.u.hyperbus.ext_addr = (uHYPERBUS_Ram_Address + addr);
    } else {
        TX->info.u.hyperbus.ext_addr = (uHYPERBUS_Flash_Address + addr);
    }

    TX->info.u.hyperbus.ext_addr |= (reg_access << 31);

    /* Send request */
    status = UDMA_SendRequest((UDMA_Type*) base, TX);

    return status;
}

static int HYPERBUS_TransferRX(HYPERBUS_Type *base, int addr, uint16_t *rx, size_t rx_length, char reg_access, char device)
{
    int32_t status;

    hyperbus_req_t *RX = UDMA_FindAvailableRequest();

    RX->info.dataAddr = (uint32_t) rx;
    RX->info.dataSize  = rx_length;
    RX->info.isTx      = 0;
    RX->info.channelId = UDMA_EVENT_HYPERBUS_RX;
    RX->info.configFlags = UDMA_CFG_DATA_SIZE(16 >> 4);
    RX->info.task        = 1;

    if (rx_length > 1024) {
        RX->info.repeat.size       = 1024;
        RX->info.repeat.size_total = rx_length;
        RX->info.dataSize          = 1024;
    }

    /* Hyperbus */
    if(device == uHYPERBUS_Ram) {
        RX->info.u.hyperbus.ext_addr = (uHYPERBUS_Ram_Address + addr);
    } else {
        RX->info.u.hyperbus.ext_addr = (uHYPERBUS_Flash_Address + addr);
    }

    RX->info.u.hyperbus.ext_addr |= (reg_access << 31);

    /* Send request */
    status = UDMA_SendRequest((UDMA_Type*) base, RX);

    return status;
}

status_t HYPERBUS_TransferBlocking(HYPERBUS_Type *base, hyperbus_transfer_t *transfer)
{
    #define MAXIMUM_TRANSFER_LENGTH 1024

    status_t status = uStatus_Fail;
    udma_req_info_t info;

    int addr = transfer->addr;
    int len  = 0;
    int data = 0;

    if (transfer->txDataSize) {
        len       = transfer->txDataSize;
        data      = (int)transfer->txData;
        info.isTx = 1;
    } else if (transfer->rxDataSize) {
        len       = transfer->rxDataSize;
        data      = (int)transfer->rxData;
        info.isTx = 0;
    }

    int length = len;

    while (len > 0) {
        int size  = (len > MAXIMUM_TRANSFER_LENGTH) ? MAXIMUM_TRANSFER_LENGTH : len;

        info.dataAddr    = (uint32_t)data;
        info.dataSize    = size;
        info.configFlags = UDMA_CFG_DATA_SIZE(16 >> 4) | UDMA_CFG_EN(1);

        if(transfer->device == uHYPERBUS_Ram) {
            info.u.hyperbus.ext_addr = (uHYPERBUS_Ram_Address + addr);
        } else {
            info.u.hyperbus.ext_addr = (uHYPERBUS_Flash_Address + addr);
        }

        info.u.hyperbus.ext_addr |= (transfer->reg_access << 31);

        base->EXT_ADDR = info.u.hyperbus.ext_addr;

        status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);

        len  -= MAXIMUM_TRANSFER_LENGTH;
        data += MAXIMUM_TRANSFER_LENGTH;
        addr += MAXIMUM_TRANSFER_LENGTH;
    }

    return ((status == uStatus_Success) ? length : -1);
}

status_t HYPERBUS_TransferNonBlocking(HYPERBUS_Type *base, hyperbus_handle_t *handle, hyperbus_transfer_t *transfer)
{
    assert(handle);

    /* Check that we're not busy.*/
    if (handle->state == uHYPERBUS_Busy)
    {
        return uStatus_Fail;
    }

    handle->state = uHYPERBUS_Busy;

    s_hyperbusIsr = HYPERBUS_TransferHandleIRQ;

    /*Start transfer*/
    if(transfer->txDataSize) {
        HYPERBUS_TransferTX(base, transfer->addr, transfer->txData, transfer->txDataSize, transfer->reg_access, transfer->device);
    } else if(transfer->rxDataSize) {
        HYPERBUS_TransferRX(base, transfer->addr, transfer->rxData, transfer->rxDataSize, transfer->reg_access, transfer->device);
    }

    return uStatus_Success;
}

/*Transactional APIs*/

void HYPERBUS_TransferCreateHandle(HYPERBUS_Type *base,
                                     hyperbus_handle_t *handle,
                                     hyperbus_transfer_callback_t callback,
                                     void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    g_hyperbusHandle[HYPERBUS_GetInstance(base)] = handle;

    handle->callback = callback;
    handle->userData = userData;
}

void HYPERBUS_TransferHandleIRQ(HYPERBUS_Type *base, hyperbus_handle_t *handle)
{
    assert(handle);

    status_t status = 0;
    if (handle->state == uHYPERBUS_Error)
    {
        status = uStatus_Fail;
    }
    else
    {
        status = uStatus_Success;
    }

    handle->state = uHYPERBUS_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

static void HYPERBUS_CommonIRQHandler(HYPERBUS_Type *base, void *param)
{
    s_hyperbusIsr(base, (hyperbus_handle_t *)param);
}

#if defined(HYPERBUS0)
void HYPERBUS0_DriverIRQHandler(void)
{
    assert(g_hyperbusHandle[0]);
    HYPERBUS_CommonIRQHandler(HYPERBUS0, g_hyperbusHandle[0]);
}
#endif
