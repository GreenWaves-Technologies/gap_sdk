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

#include "gap_mram.h"
#include "gap_pmu.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MRAM_PORb     (1 << 7)
#define MRAM_RETb     (1 << 6)
#define MRAM_RSTb     (1 << 5)
#define MRAM_NVR      (1 << 4)
#define MRAM_TMEN     (1 << 3)
#define MRAM_AREF     (1 << 2)
#define MRAM_DPD      (1 << 1)
#define MRAM_ECCBYPS  (1 << 0)

/*! @brief mram transfer state. */
enum _mram_transfer_states
{
    uMRAM_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    uMRAM_Busy,        /*!< Transfer queue is not finished. */
    uMRAM_Error        /*!< Transfer error. */
};

/*! @brief Typedef for interrupt handler. */
typedef void (*mram_isr_t)(MRAM_Type *base, mram_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for MRAM module.
 *
 * @param base MRAM peripheral base address.
 */
uint32_t MRAM_GetInstance(MRAM_Type *base);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to mram bases for each instance. */
static MRAM_Type *const s_mramBases[] = MRAM_BASE_PTRS;

/*! @brief Pointers to mram handles for each instance. */
static void *s_mramHandle[ARRAY_SIZE(s_mramBases)];

/*! @brief Pointers to mram IRQ number for each instance. */
/* static const IRQn_Type s_mramIrqs[] = MRAM_IRQS; */

/*! @brief Pointer to  IRQ handler for each instance. */
static mram_isr_t s_mramIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t MRAM_GetInstance(MRAM_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_mramBases); instance++)
    {
        if (s_mramBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_mramBases));

    return instance;
}

void MRAM_Init(MRAM_Type *base, const mram_config_t *Config, uint32_t srcClock_Hz)
{
    assert(Config);

    /* Enable MRAM UDMA clock. */
    UDMA_Init((UDMA_Type *)base);

    /* Configure baud rate. */
    MRAM_SetFrequency(base, Config->frequency, srcClock_Hz);

    /* Enable erase IRQ */
    base->IER = 0xF;
}

void MRAM_Deinit(MRAM_Type *base)
{
    /* Power Down */
    PMU_MramPowerOff();

    /* Disable all IRQ */
    base->IER = 0;

    base->CLK_DIV = MRAM_CLK_DIV_EN(0);

    /* Disable MRAM UDMA clock. */
    UDMA_Deinit((UDMA_Type *)base);
}

void MRAM_GetDefaultConfig(mram_config_t *Config)
{
    assert(Config);

    /* Default frequency at 25MHz. */
    Config->frequency  = 25000000;
}

void MRAM_SetFrequency(MRAM_Type *base, uint32_t frequency, uint32_t srcClock_Hz)
{
    assert(frequency);

    int index = 0;

    uint32_t clkDivider = (srcClock_Hz >> 1) / frequency;

    base->CLK_DIV = MRAM_CLK_DIV(clkDivider) | MRAM_CLK_DIV_EN(1);
}

void MRAM_PowerUp(MRAM_Type *base)
{
    /* Power Up */
    PMU_MramPowerOn();

    base->MODE = MRAM_MODE(MRAM_CMD_PWUP, MRAM_PORb | MRAM_RETb);

    /* tRSW >= 5us  */
    for(volatile int i = 0; i < 200; i++);

    base->MODE = MRAM_MODE(MRAM_CMD_PWUP, MRAM_PORb | MRAM_RETb | MRAM_RSTb);
}

static inline void MRAM_Wait(MRAM_Type *base, int mask)
{
    do {
        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    } while (!(base->ISR & mask));

    /* Clean IRQ status */
    base->ICR = mask;
}

int MRAM_ChipErase(MRAM_Type *base)
{
    base->ERASE_ADDR = (MRAM_BASE_ADDRESS >> 3);
    base->ERASE_SIZE = 1;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_ERASE_CHIP, mode);

    base->TRIGGER = 1;

    MRAM_Wait(base, MRAM_ISR_ERASE_DONE_MASK);

    return 0;
}

int MRAM_SectorErase(MRAM_Type *base, uint32_t address, uint32_t sector_size)
{
    base->ERASE_ADDR = (address >> 3);
    base->ERASE_SIZE = sector_size - 1;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_ERASE_SECTOR, mode);

    base->TRIGGER = 1;

    MRAM_Wait(base, MRAM_ISR_ERASE_DONE_MASK);

    return 0;
}

int MRAM_WordErase(MRAM_Type *base, uint32_t address, uint32_t length)
{
    base->ERASE_ADDR = (address >> 3);
    base->ERASE_SIZE = length - 1;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_ERASE_WORD, mode);

    base->TRIGGER = 1;

    MRAM_Wait(base, MRAM_ISR_ERASE_DONE_MASK);

    return 0;
}

int MRAM_ReferenceLineConfig(MRAM_Type *base, uint32_t ref_line, int aref, int tmen)
{
    volatile int mode = base->MODE & 0xFF;

    if (aref && tmen) {

        mode |= MRAM_AREF | MRAM_TMEN;

        /* Setting  AREF and TMEN to 1 */
        if (ref_line) {
            /* Setting AREF=1, TMEN=1 and MODE to REF LINE 1 */
            base->MODE = MRAM_MODE(MRAM_CMD_REF_LINE_P, mode);
        } else {
            /* Setting AREF=1, TMEN=1 and MODE to REF LINE 0 */
            base->MODE = MRAM_MODE(MRAM_CMD_REF_LINE_AP, mode);
        }
        base->TRIGGER = 1;

        MRAM_Wait(base, MRAM_ISR_REF_LINE_DONE_MASK);
    } else {
        /* Clear mode flag */
        mode &=  ~(MRAM_AREF | MRAM_TMEN);

        base->MODE = mode & 0xFF;
    }

    return 0;
}

int MRAM_TRIMConfig(MRAM_Type *base, uint32_t address, const char *data, int length)
{
    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_TRIM_CFG, mode);

    /* Destination Address*/
    base->TX_DADDR = (address >> 3);

    int status = MRAM_TransferBlocking(base, data, length, 0, 0);

    MRAM_Wait(base, MRAM_ISR_TRIM_CFG_DONE_MASK);

    return status;
}

int MRAM_Program(MRAM_Type *base, uint32_t address, const char *data, int length, int ecc_byp)
{
    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_NORMAL_TX, mode | (ecc_byp ? MRAM_ECCBYPS : 0));

    /* Destination Address*/
    base->TX_DADDR = (address >> 3);

    int status = MRAM_TransferBlocking(base, data, length, 0, 0);

    MRAM_Wait(base, MRAM_ISR_PROGRAM_DONE_MASK);

    return status;
}

int MRAM_Read(MRAM_Type *base, uint32_t address, char *data, int length, int ecc_byp)
{
    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_READ_RX, mode | (ecc_byp ? MRAM_ECCBYPS : 0));

    /* Destination Address*/
    base->RX_DADDR = (address >> 3);

    return MRAM_TransferBlocking(base, 0, 0, data, length);
}

status_t MRAM_TransferBlocking(MRAM_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length)
{
    status_t status = uStatus_Success;
    udma_req_info_t info;

    info.ctrl        = UDMA_CTRL_NORMAL;

    if (rx_length) {
        info.dataAddr    = (uint32_t) rx_buffer;
        info.dataSize    = (uint32_t) rx_length;
        info.configFlags = UDMA_CFG_EN(1);
        info.isTx        = 0;
    } else if (tx_length) {
        info.dataAddr    = (uint32_t) tx_buffer;
        info.dataSize    = (uint32_t) tx_length;
        info.configFlags = UDMA_CFG_EN(1);
        info.isTx        = 1;
    }

    status = UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);

    return status;
}

static status_t MRAM_TransferStart(MRAM_Type *base, mram_transfer_t *transfer)
{
    int32_t status = 0;

    mram_req_t *req = UDMA_FindAvailableRequest();

    /*Start  transfer*/
    if (transfer->rxDataSize) {

        req->info.dataAddr    = (uint32_t)  transfer->rxData;
        req->info.dataSize    = transfer->rxDataSize;
        req->info.isTx        = 0;
        req->info.channelId   = UDMA_EVENT_MRAM_RX;
    } else if (transfer->txDataSize) {

        req->info.dataAddr    = (uint32_t) transfer->txData;
        req->info.dataSize    = transfer->txDataSize;
        req->info.isTx        = 1;
        req->info.channelId   = UDMA_EVENT_MRAM_TX;
    }

    req->info.configFlags = 0;
    req->info.ctrl        = UDMA_CTRL_NORMAL;
    req->info.task        = 1;
    req->info.repeat.size = 0;

    return UDMA_SendRequest((UDMA_Type *)base, req);
}

status_t MRAM_TransferNonBlocking(MRAM_Type *base, mram_handle_t *handle, mram_transfer_t *transfer)
{
    assert(handle);

    /* Check that we're not busy.*/
    if (handle->state == uMRAM_Busy) {
        return uStatus_MRAM_Busy;
    }

    handle->state = uMRAM_Busy;

    s_mramIsr = MRAM_TransferHandleIRQ;

    /*Start transfer*/
    return MRAM_TransferStart(base, transfer);
}

void MRAM_TransferCreateHandle(MRAM_Type *base,
                              mram_handle_t *handle,
                              mram_transfer_callback_t callback,
                              void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    s_mramHandle[MRAM_GetInstance(base)] = handle;

    handle->callback = callback;
    handle->userData = userData;
}

void MRAM_TransferHandleIRQ(MRAM_Type *base, mram_handle_t *handle)
{
    assert(handle);

    status_t status = 0;
    if (handle->state == uMRAM_Error)
    {
        status = uStatus_MRAM_Error;
    }
    else
    {
        status = uStatus_Success;
    }

    handle->state = uMRAM_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

static void MRAM_CommonIRQHandler(MRAM_Type *base, void *param)
{
    s_mramIsr(base, (mram_handle_t *)param);
}

void MRAM_IRQHandler(void)
{
    assert(s_mramHandle[0]);
    MRAM_CommonIRQHandler(MRAM, s_mramHandle[0]);
}
