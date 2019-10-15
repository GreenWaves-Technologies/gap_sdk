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

#include "gap_spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Typedef for master interrupt handler. */
typedef void (*spi_master_isr_t)(SPIM_Type *base, spi_master_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for SPI module.
 *
 * @param base SPI peripheral base address.
 */
uint32_t SPI_GetInstance(SPIM_Type *base);


/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to spi bases for each instance. */
static SPIM_Type *const s_spiBases[] = SPIM_BASE_PTRS;

/*! @brief Pointers to spi handles for each instance. */
static void *g_spiHandle[ARRAY_SIZE(s_spiBases)];

/*! @brief Pointer to master IRQ handler for each instance. */
static spi_master_isr_t s_spiMasterIsr;

/* SPI transfer command sequence array */
static uint32_t s_command_sequence[16];


/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t SPI_GetInstance(SPIM_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_spiBases); instance++)
    {
        if (s_spiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_spiBases));

    return instance;
}

void SPI_MasterFrequencyConfig(SPIM_Type *base, spi_master_config_t *masterConfig)
{
    int index = 0;
    s_command_sequence[index++] = SPIM_CMD_CFG(masterConfig->clkDiv, masterConfig->cpol, masterConfig->cpha);

    /* Blocking transfer */
    SPI_MasterTransferBlocking(base,
                               (uint32_t*)s_command_sequence,
                               (index * sizeof(uint32_t)),
                               NULL, 0, 32);
}

void SPI_MasterInit(SPIM_Type *base, spi_master_config_t *masterConfig, uint32_t srcClock_Hz)
{
    assert(masterConfig);

    /* Clock Gating */
    UDMA_Init((UDMA_Type *)base);

    SPI_MasterFrequencyConfig(base, masterConfig);
}

void SPI_MasterGetDefaultConfig(spi_master_config_t *masterConfig)
{
    assert(masterConfig);
    masterConfig->baudRate = 500000;
    masterConfig->bitsPerFrame = 8;
    masterConfig->qpi= uSPI_Single;
    masterConfig->cpol = uSPI_ClockPolarityActiveHigh;
    masterConfig->cpha = uSPI_ClockPhaseFirstEdge;
    masterConfig->direction = uSPI_MsbFirst;
    masterConfig->clkDiv = 1;
}

void SPI_MasterDeInit(SPIM_Type *base)
{
    UDMA_Deinit((UDMA_Type *)base);
}

int SPI_MasterTransferCommandSequence(SPIM_Type *base, spi_command_sequence_t* seq)
{
    int index = 0;

    s_command_sequence[index++] = SPIM_CMD_SOT(seq->csn);

    if (seq->cmd_bits) {
        s_command_sequence[index++] = SPIM_CMD_SEND_CMD(seq->cmd, 8, seq->cmd_mode);
    }

    if (seq->addr_bits) {
        s_command_sequence[index++] = SPIM_CMD_SEND_ADDR(seq->addr_bits, seq->addr_mode);
        s_command_sequence[index++] = seq->addr;
    }

    if (seq->tx_bits > 32) {

        if (seq->full_duplex) {
            s_command_sequence[index++] = SPIM_CMD_DUPLEX(seq->tx_bits, 0);
        } else {
            s_command_sequence[index++] = SPIM_CMD_TX_DATA(seq->tx_bits, seq->data_mode, 0);
        }

        /* Transfer TX part of the command */
        SPI_MasterTransferBlocking(base,
                                   (const char*)s_command_sequence,
                                   (index * sizeof(uint32_t)),
                                   0, 0, 32);


        if (seq->full_duplex) {
            SPI_MasterTransferBlocking(base,
                                       (const uint8_t*)seq->tx_buffer,
                                       (seq->tx_bits >> 3),
                                       (uint8_t*) seq->rx_buffer,
                                       (seq->rx_bits >> 3),
                                       32);
            seq->rx_bits = 0;
        } else {
            /* Transfer TX buffer */
            SPI_MasterTransferBlocking(base,
                                       (const uint8_t*)seq->tx_buffer,
                                       (seq->tx_bits >> 3),
                                       0, 0, 32);
        }

        index = 0;
        seq->rx_buffer = (void *)0;
    } else {
        if (seq->tx_bits) {
            s_command_sequence[index++] = SPIM_CMD_TX_DATA(seq->tx_bits, seq->data_mode, 0);
            s_command_sequence[index++] = seq->tx_data;
        }

        if (seq->rx_bits) {
            if (seq->alter_data_bits)
                s_command_sequence[index++] = SPIM_CMD_SEND_CMD(seq->alter_data, seq->alter_data_bits, seq->alter_data_mode);
            s_command_sequence[index++] = SPIM_CMD_DUMMY (seq->dummy);
            s_command_sequence[index++] = SPIM_CMD_RX_DATA(seq->rx_bits, seq->data_mode, 0);
        } else {
            seq->rx_buffer = (void *)0;
        }
    }
    s_command_sequence[index++]  = SPIM_CMD_EOT(0);

    SPI_MasterTransferBlocking(base,
                               (const uint32_t*)s_command_sequence,
                               (index * sizeof(uint32_t)),
                               (uint8_t*) seq->rx_buffer,
                               (seq->rx_bits >> 3),
                               32);

    return (index * sizeof(uint32_t));
}

void SPI_Master_CS(SPIM_Type *base, int whichCsn, int status)
{
    int index = 0;

    if (status)
        s_command_sequence[index++] = SPIM_CMD_EOT(1);
    else
        s_command_sequence[index++] = SPIM_CMD_SOT(whichCsn);

    /* Blocking transfer */
    SPI_MasterTransferBlocking(base,
                               (uint32_t* )s_command_sequence,
                               (index * sizeof(uint32_t)),
                               NULL, 0, 32);

}

void SPI_Master_DulpexTransfer(SPIM_Type *base, int tx_value, void *rx_buffer, int bits)
{
    int index = 0;

    s_command_sequence[index++] = SPIM_CMD_DUPLEX(bits, 0);
    s_command_sequence[index++] = tx_value;

    /* Blocking transfer */
    SPI_MasterTransferBlocking(base,
                               (uint32_t* )s_command_sequence,
                               (index * sizeof(uint32_t)),
                               (uint8_t *)rx_buffer,
                               (bits>> 3),
                               32);

}

int SPI_Master_AutoPolling(SPIM_Type *base, spi_polling_config_t *conf)
{
    int index = 0;

    /* Set a reference event */
    UDMA_GC->EVTIN |= UDMA_GC_EVTIN_CHOICE0( REF32K_CLK_RISE_EVENT );

    /* Enable SPI master end event mask */
    SOC_EU_SetFCMask(UDMA_EVENT_SPIM0_EOT + SPI_GetInstance(base));

    /* Allow reference Clock propagating to UDMA */
    SOC_EU_SetPRMask( REF32K_CLK_RISE_EVENT );

    /* Empty transfer */
    s_command_sequence[index++] = SPIM_CMD_SOT(conf->csn);
    s_command_sequence[index++] = SPIM_CMD_SEND_CMD(0x0, 8, conf->cmd_mode);
    s_command_sequence[index++] = SPIM_CMD_EOT(0);
    s_command_sequence[index++] = SPIM_CMD_RPT(conf->time_out),
    s_command_sequence[index++] = SPIM_CMD_SOT(conf->csn);
    s_command_sequence[index++] = SPIM_CMD_SEND_CMD(conf->cmd, 8, conf->cmd_mode);
    s_command_sequence[index++] = SPIM_CMD_RX_CHECK(conf->match_type, conf->match_bits, conf->match_value, conf->data_mode, 0),
    s_command_sequence[index++] = SPIM_CMD_EOT(0);
    /* Wait in the first event - REF32K_CLK_EVENT */
    s_command_sequence[index++] = SPIM_CMD_WAIT( SPIM_CMD_WAIT_EVENT_ID(0) );
    s_command_sequence[index++] = SPIM_CMD_RPT_END();

    /* Generate SPI maste rend event */
    s_command_sequence[index++] = SPIM_CMD_EOT(1);

    /* Blocking transfer */
    SPI_MasterTransferBlocking(base,
                               (uint32_t* )s_command_sequence,
                               (index *sizeof(uint32_t)),
                               0, 0, 32);

    /* Wait the polling end */
    UDMA_BlockWait(UDMA_EVENT_SPIM0_EOT >> 1);

    /* Disallow reference Clock propagating to UDMA */
    SOC_EU_ClearPRMask( REF32K_CLK_RISE_EVENT );

    /* Deset a reference event */
    UDMA_GC->EVTIN = 0;

    /* Disble SPI master end event mask */
    SOC_EU_ClearFCMask(UDMA_EVENT_SPIM0_EOT + SPI_GetInstance(base));

    return 0;
}

void SPI_MasterTransferBlocking(SPIM_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width)
{
    udma_req_info_t info;

    if (rx_length) {
        info.dataAddr    = (uint32_t) rx_buffer;
        info.dataSize    = (uint32_t) rx_length;
        info.configFlags = UDMA_CFG_DATA_SIZE((bit_width >> 4)) | UDMA_CFG_EN(1);
        info.isTx        = 0;
        UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_NO_WAIT);
    }

    if (tx_length) {
        info.dataAddr    = (uint32_t) tx_buffer;
        info.dataSize    = (uint32_t) tx_length;
        /* TX cahnnel is fixed 32bits */
        info.configFlags = UDMA_CFG_DATA_SIZE((32 >> 4)) | UDMA_CFG_EN(1);
        info.isTx        = 1;

        if(rx_length)
            UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT_RX);
        else
            UDMA_BlockTransfer((UDMA_Type *)base, &info, UDMA_WAIT);
    }
}

static void SPI_MasterTransferStart(SPIM_Type *base, spi_transfer_t *transfer)
{
    /*Start master transfer*/
    if (transfer->rxDataSize) {
      spi_req_t *RX = UDMA_FindAvailableRequest();

      RX->info.dataAddr  = (uint32_t)  transfer->rxData;
      RX->info.dataSize  = transfer->rxDataSize;
      RX->info.isTx      = 0;
      RX->info.channelId = UDMA_EVENT_SPIM0_RX + (SPI_GetInstance(base) << 1);
      RX->info.ctrl        = UDMA_CTRL_DUAL_RX;
      RX->info.configFlags = UDMA_CFG_DATA_SIZE((transfer->configFlags >> 4));
      RX->info.task        = 1;
      RX->info.repeat.size = 0;

      UDMA_SendRequest((UDMA_Type *)base, RX);
    }

    if (transfer->txDataSize) {
      spi_req_t *TX = UDMA_FindAvailableRequest();

      TX->info.dataAddr = (uint32_t) transfer->txData;
      TX->info.dataSize  = transfer->txDataSize;
      TX->info.isTx      = 1;
      TX->info.channelId   = UDMA_EVENT_SPIM0_TX + (SPI_GetInstance(base) << 1);
      TX->info.configFlags = UDMA_CFG_DATA_SIZE((transfer->configFlags >> 4));

      if(transfer->rxDataSize){
          TX->info.ctrl = UDMA_CTRL_DUAL_TX;
          TX->info.task = 0;
      } else {
          TX->info.ctrl = UDMA_CTRL_NORMAL;
          TX->info.task = 1;
      }
      TX->info.repeat.size      = 0;

      UDMA_SendRequest((UDMA_Type *)base, TX);
    }
}

status_t SPI_MasterTransferNonBlocking(SPIM_Type *base, spi_master_handle_t *handle, spi_transfer_t *transfer)
{
    assert(handle);

    /* Check that we're not busy.*/
    if (handle->state == uSPI_Busy)
    {
        return uStatus_SPI_Busy;
    }

    handle->state = uSPI_Busy;

    s_spiMasterIsr = SPI_MasterTransferHandleIRQ;

    /*Start master transfer*/
    SPI_MasterTransferStart(base, transfer);

    return uStatus_Success;
}

/*Transactional APIs -- Master*/

void SPI_MasterTransferCreateHandle(SPIM_Type *base,
                                     spi_master_handle_t *handle,
                                     spi_master_transfer_callback_t callback,
                                     void *userData)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    g_spiHandle[SPI_GetInstance(base)] = handle;

    handle->callback = callback;
    handle->userData = userData;
}

void SPI_MasterTransferHandleIRQ(SPIM_Type *base, spi_master_handle_t *handle)
{
    assert(handle);

    status_t status = 0;
    if (handle->state == uSPI_Error)
    {
        status = uStatus_SPI_Error;
    }
    else
    {
        status = uStatus_Success;
    }

    handle->state = uSPI_Idle;

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

static void SPI_CommonIRQHandler(SPIM_Type *base, void *param)
{
    s_spiMasterIsr(base, (spi_master_handle_t *)param);
}

#if defined(SPIM0)
void SPI0_DriverIRQHandler(void)
{
    assert(g_spiHandle[0]);
    SPI_CommonIRQHandler(SPIM0, g_spiHandle[0]);
}
#endif


#if defined(SPIM1)
void SPI1_DriverIRQHandler(void)
{
    assert(g_spiHandle[1]);
    SPI_CommonIRQHandler(SPIM1, g_spiHandle[1]);
}
#endif
