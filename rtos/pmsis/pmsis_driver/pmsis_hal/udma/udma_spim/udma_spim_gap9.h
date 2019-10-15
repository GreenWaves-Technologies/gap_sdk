#ifndef __UDMA_SPIM_GAP9__
#define __UDMA_SPIM_GAP9__

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
