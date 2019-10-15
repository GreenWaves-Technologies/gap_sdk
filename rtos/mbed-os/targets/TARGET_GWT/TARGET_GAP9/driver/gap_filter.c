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

#include "gap_filter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief filter transfer state. */
enum _filter_transfer_states
{
    uFILTER_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    uFILTER_Busy,        /*!< Transfer queue is not finished. */
    uFILTER_Error        /*!< Transfer error. */
};

/*! @brief Typedef for interrupt handler. */
typedef void (*filter_isr_t)(FILTER_Type *base, filter_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for FILTER module.
 *
 * @param base FILTER peripheral base address.
 */
uint32_t FILTER_GetInstance(FILTER_Type *base);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to filter bases for each instance. */
static FILTER_Type *const s_filterBases[] = FILTER_BASE_PTRS;

/*! @brief Pointers to filter handles for each instance. */
static void *s_filterHandle[ARRAY_SIZE(s_filterBases)];

/*! @brief Pointers to filter IRQ number for each instance. */
/* static const IRQn_Type s_filterIrqs[] = FILTER_IRQS; */

/*! @brief Pointer to  IRQ handler for each instance. */
static filter_isr_t s_filterIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t FILTER_GetInstance(FILTER_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_filterBases); instance++)
    {
        if (s_filterBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_filterBases));

    return instance;
}

void FILTER_Init(FILTER_Type *base, const filter_config_t *Config, uint32_t srcClock_Hz)
{
    assert(Config);

    /* Enable FILTER UDMA clock. */
    UDMA_Init((UDMA_Type *)base);
}

void FILTER_Deinit(FILTER_Type *base)
{
    /* Disable FILTER UDMA clock. */
    UDMA_Deinit((UDMA_Type *)base);
}

void FILTER_GetDefaultConfig(filter_config_t *Config)
{
    assert(Config);

    /* Default frequency at 25MHz. */
    Config->frequency  = 25000000;
}

static inline void FILTER_Wait(FILTER_Type *base)
{
    do {
        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    } while (!(base->STATUS & FILTER_STATUS_DONE_MASK));

    /* Write 1 to clear DONE bit */
    base->STATUS = FILTER_STATUS_DONE(1);
}

int FILTER_Threshold(FILTER_Type *base, filter_threshold_config_t *config)
{
    uint32_t loc_window_size = (1 << config->window_size);
    uint32_t trans_length    = config->size - loc_window_size + 1;

    base->TX_CH0_ADDR = (volatile int)config->input;
    base->TX_CH0_LEN0 = loc_window_size - 1;   //window size
    base->TX_CH0_LEN1 = trans_length - 1;      //number of windows
    base->TX_CH0_CFG  = FILTER_CH_CFG_MODE(config->mode) | FILTER_CH_CFG_SIZE(config->in_format);
    base->AU_CFG      = FILTER_AU_CFG_SHIFT(config->window_size) | FILTER_AU_CFG_MODE(config->operation);
    base->BINCU_SETUP = FILTER_CH_CFG_SIZE(config->out_format);
    base->BINCU_TH    = config->threshold;
    base->BINCU_CNT   = FILTER_BINCU_CNT_EN(1) | FILTER_BINCU_CNT(0x1000);
    base->FILT        = uFILTER_L2_OFF_OFF_ON;  // uFILTER_L2_OFF_OFF_ON
    base->FILT_CMD    = FILTER_FILT_CMD_START(1);

    FILTER_Wait(base);

    /* Reset count */
    base->BINCU_CNT   = 0;

    return (base->BINCU_VAL);
}

int FILTER_Binarization(FILTER_Type *base, filter_binarization_config_t *config)
{
    uint32_t trans_size = config->size - 1;

    base->TX_CH0_ADDR  = (volatile int)config->input;
    base->TX_CH0_LEN0  = trans_size;
    base->TX_CH0_CFG   = FILTER_CH_CFG_MODE(uFILTER_In_Linear) | FILTER_CH_CFG_SIZE(config->in_format);  // config->in_mode
    base->RX_CH_ADDR   = (volatile int)config->output;
    base->RX_CH_LEN0   = trans_size;
    base->RX_CH_CFG    = FILTER_CH_CFG_MODE(uFILTER_Out_Linear) | FILTER_CH_CFG_SIZE(config->out_format); // config->out_mode
    base->AU_CFG       = FILTER_AU_CFG_BYPASS(1);
    base->BINCU_SETUP  = FILTER_CH_CFG_SIZE(config->in_format);
    base->BINCU_TH     = config->threshold;
    base->FILT         = uFILTER_L2_OFF_ON_ON; // config->control;
    base->FILT_CMD     = FILTER_FILT_CMD_START(1);

    FILTER_Wait(base);

    return 0;
}

int FILTER_Transpose(FILTER_Type *base, filter_transpose_config_t *config)
{
    uint32_t trans_col    = config->col_num - 1;
    uint32_t trans_row    = config->row_num - 1;

    base->TX_CH0_ADDR = (volatile int)config->input;
    base->TX_CH0_LEN0 = trans_col;
    base->TX_CH0_LEN1 = trans_row;
    base->TX_CH0_LEN2 = (config->in_stride << config->in_format);
    base->TX_CH0_CFG  = FILTER_CH_CFG_MODE(uFILTER_In_2D) | FILTER_CH_CFG_SIZE(config->in_format);  // config->in_mode
    base->RX_CH_ADDR  = (volatile int)config->output;
    base->RX_CH_LEN0  = trans_row;
    base->RX_CH_LEN1  = trans_col;
    base->RX_CH_LEN2  = (config->out_stride << config->in_format);
    base->RX_CH_CFG   = FILTER_CH_CFG_MODE(uFILTER_Out_2D_COL) | FILTER_CH_CFG_SIZE(config->in_format);  // config->in_mode
    base->AU_CFG      = FILTER_AU_CFG_BYPASS(1);
    base->FILT        = uFILTER_L2_OFF_ON_OFF;
    base->FILT_CMD    = FILTER_FILT_CMD_START(1);

    FILTER_Wait(base);

    return 0;
}

int FILTER_Memcpy(FILTER_Type *base, filter_memcpy_config_t *config)
{
    uint32_t trans_size   = config->size - 1;

    base->TX_CH0_ADDR  = (volatile int)config->input;
    base->TX_CH0_LEN0 = trans_size;
    base->TX_CH0_CFG  = FILTER_CH_CFG_MODE(config->in_mode) | FILTER_CH_CFG_SIZE(config->in_format);
    base->RX_CH_ADDR  = (volatile int)config->output;
    base->RX_CH_LEN0  = trans_size;
    base->RX_CH_CFG   = FILTER_CH_CFG_MODE(config->out_mode) | FILTER_CH_CFG_SIZE(config->in_format);
    base->AU_CFG      = FILTER_AU_CFG_BYPASS(1);
    base->FILT        = uFILTER_L2_OFF_ON_OFF;
    base->FILT_CMD    = FILTER_FILT_CMD_START(1);

    FILTER_Wait(base);

    return 0;
}

int FILTER_Convolution(FILTER_Type *base, filter_convolution_config_t *config)
{
    uint32_t trans_window = config->kernel_size - 1;
    uint32_t trans_length = (config->size - config->kernel_size + 1) - 1;

    base->TX_CH0_ADDR = (volatile int)config->input;
    base->TX_CH0_LEN0 = trans_window;   //window size
    base->TX_CH0_LEN1 = trans_length;   //number of windows
    base->TX_CH0_CFG  = FILTER_CH_CFG_MODE(config->in_mode0) | FILTER_CH_CFG_SIZE(config->in_format);
    base->TX_CH1_ADDR = (volatile int)config->kernel;
    base->TX_CH1_LEN0 = trans_window;   //kernel size
    base->TX_CH1_LEN1 = trans_length;   //number of times buffer is fetched
    base->TX_CH1_CFG  = FILTER_CH_CFG_MODE(config->in_mode1) | FILTER_CH_CFG_SIZE(config->in_format);
    base->RX_CH_ADDR  = (volatile int)config->output;
    base->RX_CH_LEN0  = trans_length;
    base->RX_CH_CFG   = FILTER_CH_CFG_MODE(config->out_mode) | FILTER_CH_CFG_SIZE(config->in_format);  // config->in_mode
    base->AU_CFG      = FILTER_AU_CFG_MODE(config->operation) | FILTER_AU_CFG_SIGNED(config->in_format >> 2);
    base->FILT        = uFILTER_L2_L2_ON_OFF;
    base->FILT_CMD    = FILTER_FILT_CMD_START(1);

    FILTER_Wait(base);

    return 0;
}

int FILTER_Vector(FILTER_Type *base, filter_vector_config_t *config)
{
    uint32_t trans_window = config->size - 1;

    base->TX_CH0_ADDR = (volatile int)config->opa;
    base->TX_CH0_LEN0 = trans_window;   //window size
    base->TX_CH0_CFG  = FILTER_CH_CFG_MODE(config->in_mode0) | FILTER_CH_CFG_SIZE(config->in_format);
    base->TX_CH1_ADDR = (volatile int)config->opb;
    base->TX_CH1_LEN0 = trans_window;   //kernel size
    base->TX_CH1_CFG  = FILTER_CH_CFG_MODE(config->in_mode1) | FILTER_CH_CFG_SIZE(config->in_format);
    base->RX_CH_ADDR  = (volatile int)config->output;
    base->RX_CH_LEN0  = trans_window;
    base->RX_CH_CFG   = FILTER_CH_CFG_MODE(config->out_mode) | FILTER_CH_CFG_SIZE(config->in_format);  // config->in_mode
    base->AU_CFG      = FILTER_AU_CFG_MODE(config->operation) | FILTER_AU_CFG_SIGNED(config->in_format >> 2);
    base->FILT        = uFILTER_L2_L2_ON_OFF;
    base->AU_REG0     = config->reg0;
    base->AU_REG1     = config->reg1;
    base->FILT_CMD    = FILTER_FILT_CMD_START(1);

    FILTER_Wait(base);

    return 0;
}

status_t FILTER_TransferBlocking(FILTER_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length)
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

static status_t FILTER_TransferStart(FILTER_Type *base, filter_transfer_t *transfer)
{
    int32_t status = 0;

    filter_req_t *req = UDMA_FindAvailableRequest();

    /*Start  transfer*/
    if (transfer->rxDataSize) {

        req->info.dataAddr    = (uint32_t)  transfer->rxData;
        req->info.dataSize    = transfer->rxDataSize;
        req->info.isTx        = 0;
        req->info.channelId   = UDMA_EVENT_FILTER_EOT;
    } else if (transfer->txDataSize) {

        req->info.dataAddr    = (uint32_t) transfer->txData;
        req->info.dataSize    = transfer->txDataSize;
        req->info.isTx        = 1;
        req->info.channelId   = UDMA_EVENT_FILTER_EOT;
    }

    req->info.configFlags = 0;
    req->info.ctrl        = UDMA_CTRL_NORMAL;
    req->info.task        = 1;
    req->info.repeat.size = 0;

    return UDMA_SendRequest((UDMA_Type *)base, req);
}

status_t FILTER_TransferNonBlocking(FILTER_Type *base, filter_handle_t *handle, filter_transfer_t *transfer)
{
    assert(handle);

    /* Check that we're not busy.*/
    if (handle->state == uFILTER_Busy) {
        return uStatus_FILTER_Busy;
    }

    handle->state = uFILTER_Busy;

    s_filterIsr = FILTER_TransferHandleIRQ;

    /*Start transfer*/
    return FILTER_TransferStart(base, transfer);
}

void FILTER_TransferCreateHandle(FILTER_Type *base,
                              filter_handle_t *handle,
                              filter_transfer_callback_t callback,
                              void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    s_filterHandle[FILTER_GetInstance(base)] = handle;

    handle->callback = callback;
    handle->userData = userData;
}

void FILTER_TransferHandleIRQ(FILTER_Type *base, filter_handle_t *handle)
{
    assert(handle);

    status_t status = 0;
    if (handle->state == uFILTER_Error)
    {
        status = uStatus_FILTER_Error;
    }
    else
    {
        status = uStatus_Success;
    }

    handle->state = uFILTER_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

static void FILTER_CommonIRQHandler(FILTER_Type *base, void *param)
{
    s_filterIsr(base, (filter_handle_t *)param);
}

void FILTER_IRQHandler(void)
{
    assert(s_filterHandle[0]);
    FILTER_CommonIRQHandler(FILTER0, s_filterHandle[0]);
}

void FILTER_BINCNT_IRQHandler(void)
{
    assert(s_filterHandle[0]);
    FILTER_CommonIRQHandler(FILTER0, s_filterHandle[0]);
}
