/**
 * Defines/types and small low level driver for udma spim main functionnalities
 */

#ifndef __UDMA_SPIM_H__
#define __UDMA_SPIM_H__

#include "targets/pmsis_targets.h"

#define SPIM(id) ((spim_t *) UDMA_SPIM(id))

/*! @brief SPI master request create sequence structure.*/
typedef struct _spi_command_sequence {
    uint8_t  csn;           /*!< The chip select to device */
    uint16_t cmd;           /*!< The command sent to device */
    uint8_t  cmd_bits;      /*!< The bits of cmd sent to device */
    uint8_t  cmd_mode;      /*!< Specifies the Instruction Mode single or quad */
    uint8_t  addr_bits;     /*!< The bits of address sent to device */
    uint32_t addr;          /*!< If addr_bits is not 0, the address sent to device*/
    uint8_t  addr_mode;     /*!< Specifies the Address Mode 1 or 4 */
    uint32_t tx_size;       /*!< The size (in bytes) of buffer sent to device*/
    uint8_t *tx_buffer;     /*!< If tx_bits is more than a word, the data buffer pointer written to device*/
    uint32_t rx_size;       /*!< The length (in bytes) of data readen from device*/
    uint8_t  alter_data;       /*!< Specifies the alternative data Mode (used for dummy cycles and data phases)*/
    uint8_t  alter_data_bits;  /*!< Specifies the alternative data bits */
    uint8_t  alter_data_mode;  /*!< Specifies the alternative data Mode single or quad */
    uint8_t  dummy;         /*!< Specifies the Number of Dummy Cycles */
    uint8_t  data_mode;     /*!< Specifies the Data Mode single or quad */
    uint8_t *rx_buffer;    /*!< If rx_size is not 0, pointer of receive buffer*/
} spi_command_sequence_t;

// Fill command buffer, uses UCA/UCS method
static inline void spim_fill_command_buffer(uint32_t *buffer, uint32_t *size,
        spi_command_sequence_t *seq)
{
    int index = 0;

    buffer[index++] = SPI_CMD_SOT(seq->csn);

    if (seq->cmd_bits)
    {
        buffer[index++] = SPI_CMD_SEND_CMD(seq->cmd, 7, 0, seq->cmd_mode);
    }

    if (seq->addr_bits)
    {
        for (int i = (seq->addr_bits - 8); i >= 0 ; i-=8)
        {
            buffer[index++] = SPI_CMD_SEND_CMD(((seq->addr >> i) &
                        0xFF), 7, 0, seq->addr_mode);
        }
    }

    if (seq->tx_size)
    {
        buffer[index++] = SPI_CMD_TX_DATA(seq->tx_size-1, 7, 0,
                0, seq->data_mode);
        buffer[index++] = SPI_CMD_SETUP_UCA(seq->tx_buffer&0x1FFFFF, SPI_4_WORD_FORMAT,
                1);
        buffer[index++] = SPI_CMD_SETUP_UCS(seq->tx_size&0xFFFF, SPI_4_WORD_FORMAT,
                1);
    }

    if (seq->rx_size)
    {
        if (seq->alter_data_bits)
        {
            buffer[index++] = SPI_CMD_SEND_CMD(seq->alter_data,
                    seq->alter_data_bits-1, 0, seq->alter_data_mode);
        }
        if(seq->dummy)
        {
            buffer[index++] = SPI_CMD_DUMMY (seq->dummy-1);
        }
        buffer[index++] = SPI_CMD_RX_DATA(seq->rx_size-1, 7, 0,
                0, seq->data_mode);
        buffer[index++] = SPI_CMD_SETUP_UCA(seq->rx_buffer&0x1FFFFF, SPI_4_WORD_FORMAT,
                0);
        buffer[index++] = SPI_CMD_SETUP_UCS(seq->rx_size, SPI_4_WORD_FORMAT,
                0);
    }
    else
    {
        seq->rx_buffer = (void *)0;
    }

    buffer[index++] = SPI_CMD_EOT(1);
    *size=index*sizeof(uint32_t);
}

/*
 * SPIM udma channel enqueue
 */
static inline void spim_enqueue_channel(spim_t *spim, uint32_t addr,
        uint32_t size, uint32_t config, udma_channel_e channel)
{
    udma_cmd_enqueue_channel(&spim->udma_spim, addr, size, config, channel);
}

static inline void spim_send_command_array_buffer(spim_t *spim,
        uint32_t *command_buffer, uint32_t size, uint32_t config)
{
    spim_enqueue_channel(spim, (uint32_t)command_buffer, size,
            config, COMMAND_CHANNEL);
}


static inline void spi_master_transfer_command_sequence(int spim_id,
        spi_command_sequence_t *command_sequence)
{
    // TODO: put this function in a real driver and alloc memory dynamically
    uint32_t command_buffer[32];
    uint32_t size;
    spim_fill_command_buffer(command_buffer, &size, command_sequence);
    spim_send_command_array_buffer(SPIM(spim_id), command_buffer, size,
            UDMA_CFG_EN(1) | UDMA_CFG_DATA_SIZE(sizeof(uint32_t)));

    do
    {
        hal_itc_wait_for_event_no_irq(FC_SOC_EVENT_IRQN);
    }while(hal_itc_event_fifo_pop() != (uint32_t)(SOC_EVENT_UDMA_SPIM_EOT(spim_id)));
}

static inline void udma_spim_init(uint32_t spim_id, uint32_t clock_divizer,
        uint32_t polarity, uint32_t phase)
{
    uint32_t buffer[2];
    uint32_t config;
    // disable clock gating for said device
    udma_ctrl_cg_disable(UDMA_SPIM_ID(spim_id));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(spim_id));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_TX(spim_id));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_CMD(spim_id));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_EOT(spim_id));

    // placeholder for now, only clock div really configured
    buffer[0] = SPI_CMD_CFG (clock_divizer, phase, polarity);
    buffer[1] = SPI_CMD_EOT (1);
    // set frenquency, polarity and clock_divizer
    config = UDMA_CFG_DATA_SIZE((32 >> 4)) | UDMA_CFG_EN(1);
    spim_send_command_array_buffer(SPIM(spim_id), buffer,
            8, config);

    do
    {
        hal_itc_wait_for_event_no_irq(FC_SOC_EVENT_IRQN);
    }while(hal_itc_event_fifo_pop() != SOC_EVENT_UDMA_SPIM_EOT(spim_id));
}



static inline void spim_deinit_device(uint32_t spim_id)
{
    // disable clock gating for said device
    udma_ctrl_cg_enable(UDMA_SPIM_ID(spim_id));
}

#endif
