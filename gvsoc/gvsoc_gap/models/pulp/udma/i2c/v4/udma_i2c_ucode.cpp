/*
 * Copyright (C) 2021  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "udma_i2c_ucode.hpp"

#include <stdio.h>
#include <archi/utils.h>

//#define UCODE_DEBUG(...)    (fprintf(stderr, "[I2C-UCODE] " __VA_ARGS__))
#define UCODE_DEBUG(...)

namespace {

    ucode_data_t null_decoder (uint32_t ucode)
    {
        UCODE_DEBUG("decoder: NOT IMPLEMENTED - ucode: 0x%08x\n", ucode);
        ucode_data_t empty;
        empty.id = CMD_NOT_IMPLEMENTED;
        return empty;
    }

    void null_callback(ucode_data_t ucode)
    {
        (void) ucode;
        UCODE_DEBUG("callback: NOT IMPLEMENTED\n");
    }

    ucode_data_t timing_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_TIMING;

        data.value.timings.div_prescaler = ARCHI_REG_FIELD_GET(ucode, 8, 4);
        data.value.timings.delay_low = ARCHI_REG_FIELD_GET(ucode, 0, 4);
        data.value.timings.delay_high = ARCHI_REG_FIELD_GET(ucode, 4, 4);

        UCODE_DEBUG("TIMING: %d %d %d\n",
                data.value.timings.div_prescaler,
                data.value.timings.delay_low,
                data.value.timings.delay_high);

        return data;
    }

    ucode_data_t event_send_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_EVENT_SEND;

        UCODE_DEBUG("CMD_EVENT_SEND: no data\n");

        return data;
    }

    ucode_data_t tx_chan_cfg_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_UDMA_TX_CHAN_CFG;

        data.value.tx_chan_cfg.register_address = ARCHI_REG_FIELD_GET(ucode, 0, 3);
        UCODE_DEBUG("CMD_UDMA_TX_CHAN_CFG: %d\n",
                data.value.tx_chan_cfg.register_address);

        return data;
    }

    ucode_data_t rx_chan_cfg_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_UDMA_RX_CHAN_CFG;

        data.value.rx_chan_cfg.register_address = ARCHI_REG_FIELD_GET(ucode, 0, 3);
        UCODE_DEBUG("CMD_UDMA_RX_CHAN_CFG: %d\n",
                data.value.rx_chan_cfg.register_address);

        return data;
    }

    ucode_data_t nop_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_MISC_NOP;

        UCODE_DEBUG("CMD_MISC_NOP: no data\n");

        return data;
    }

    ucode_data_t wait_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_MISC_WAIT;

        UCODE_DEBUG("CMD_MISC_WAIT: no data\n");

        return data;
    }

    ucode_data_t repeat_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_MISC_REPEAT;

        data.value.repeat.counter = ARCHI_REG_FIELD_GET(ucode, 0, 16);
        UCODE_DEBUG("CMD_MISC_REPEAT: %d\n",
                data.value.repeat.counter);

        return data;
    }

    ucode_data_t wait_i2c_period_end_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_MISC_WAIT_I2C_PERIOD_END;

        UCODE_DEBUG("CMD_MISC_WAIT_I2C_PERIOD_END: no data\n");

        return data;
    }

    ucode_data_t foll_addr_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_FOLL_ADDR;

        data.value.foll_addr.address = ARCHI_REG_FIELD_GET(ucode, 0, 10);
        data.value.foll_addr.mask = ARCHI_REG_FIELD_GET(ucode, 10, 5);

        data.value.foll_addr.enable_7bits = ARCHI_REG_FIELD_GET(ucode, 15, 1);
        data.value.foll_addr.enable_10bits = ARCHI_REG_FIELD_GET(ucode, 16, 1);

        data.value.foll_addr.enable_sof_rcv = ARCHI_REG_FIELD_GET(ucode, 17, 1);
        data.value.foll_addr.enable_eof_rcv = ARCHI_REG_FIELD_GET(ucode, 18, 1);

        data.value.foll_addr.enable_sof_snd = ARCHI_REG_FIELD_GET(ucode, 19, 1);
        data.value.foll_addr.enable_eof_snd = ARCHI_REG_FIELD_GET(ucode, 20, 1);

        data.value.foll_addr.enable_push = ARCHI_REG_FIELD_GET(ucode, 21, 1);
        data.value.foll_addr.slot_index = ARCHI_REG_FIELD_GET(ucode, 22, 2);

        UCODE_DEBUG("CMD_FOLL_ADDR: address=0x%x, mask=0x%x, 7bits=%s, 10bits=%s, sof_rcv=%s, eof_rcv=%s, "
                "sof_snd=%s, eof_snd=%s, slot_index=%d\n",
                data.value.foll_addr.address,
                data.value.foll_addr.mask,
                data.value.foll_addr.enable_7bits ? "true" : "false",
                data.value.foll_addr.enable_10bits ? "true" : "false",
                data.value.foll_addr.enable_sof_rcv ? "true" : "false",
                data.value.foll_addr.enable_eof_rcv ? "true" : "false",
                data.value.foll_addr.enable_sof_snd ? "true" : "false",
                data.value.foll_addr.enable_eof_snd ? "true" : "false",
                data.value.foll_addr.slot_index
                );

        return data;
    }

    ucode_data_t lead_start_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_START;

        UCODE_DEBUG("CMD_LEAD_START: no data\n");
        return data;
    }

    ucode_data_t lead_send_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_SEND;

        UCODE_DEBUG("CMD_LEAD_SEND: no data\n");
        return data;
    }

    ucode_data_t lead_send_imm_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_SEND_IMM;

        data.value.lead_send_imm.byte = ARCHI_REG_FIELD_GET(ucode, 0, 8);
        UCODE_DEBUG("CMD_LEAD_SEND_IMM: byte=%d\n",
                data.value.lead_send_imm.byte);
        return data;
    }

    ucode_data_t lead_send_imm_addr_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_SEND_IMM_ADDR;

        data.value.lead_send_imm_addr.is_10bits= ARCHI_REG_FIELD_GET(ucode, 15, 1);
        data.value.lead_send_imm_addr.address = ARCHI_REG_FIELD_GET(ucode, 0, 15);
        UCODE_DEBUG("CMD_LEAD_SEND_IMM_ADDR: address=0x%x, is_10_bits=%s\n",
                data.value.lead_send_imm_addr.address,
                data.value.lead_send_imm_addr.is_10bits ? "true" : "false");
        return data;
    }

    ucode_data_t lead_recv_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_RECV;

        UCODE_DEBUG("CMD_LEAD_RECV: no data\n");
        return data;
    }

    ucode_data_t lead_recv_last_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_RECV_LAST;

        UCODE_DEBUG("CMD_LEAD_RECV_LAST: no data\n");
        return data;
    }

    ucode_data_t lead_stop_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_LEAD_STOP;

        UCODE_DEBUG("CMD_LEAD_STOP: no data\n");
        return data;
    }

    ucode_data_t event_recv_decoder(uint32_t ucode)
    {
        ucode_data_t data;
        data.id = CMD_EVENT_RECV;

        data.value.event_recv.event_index = ARCHI_REG_FIELD_GET(ucode, 0, 2);
        UCODE_DEBUG("CMD_EVENT_RECV: index=%d\n",
                data.value.event_recv.event_index);

        return data;
    }
}

I2c_ucode::I2c_ucode(void)
{
    UCODE_DEBUG("constructor\n");

    this->callback = null_callback;
    /* initialize authorized ucode id */
    this->id_parsers[CMD_TIMING]                   = timing_decoder;
    this->id_parsers[CMD_EVENT_SEND]               = event_send_decoder;
    this->id_parsers[CMD_UDMA_TX_CHAN_CFG]         = tx_chan_cfg_decoder;
    this->id_parsers[CMD_UDMA_RX_CHAN_CFG]         = rx_chan_cfg_decoder;
    this->id_parsers[CMD_MISC_NOP]                 = nop_decoder;
    this->id_parsers[CMD_MISC_WAIT]                = wait_decoder;
    this->id_parsers[CMD_MISC_REPEAT]              = repeat_decoder;
    this->id_parsers[CMD_MISC_WAIT_I2C_PERIOD_END] = wait_i2c_period_end_decoder;
    this->id_parsers[CMD_FOLL_ADDR]                = foll_addr_decoder;
    this->id_parsers[CMD_LEAD_START]               = lead_start_decoder;
    this->id_parsers[CMD_LEAD_SEND]                = lead_send_decoder;
    this->id_parsers[CMD_LEAD_SEND_IMM]            = lead_send_imm_decoder;
    this->id_parsers[CMD_LEAD_SEND_IMM_ADDR]       = lead_send_imm_addr_decoder;
    this->id_parsers[CMD_LEAD_RECV]                = lead_recv_decoder;
    this->id_parsers[CMD_LEAD_RECV_LAST]           = lead_recv_last_decoder;
    this->id_parsers[CMD_LEAD_STOP]                = lead_stop_decoder;
    this->id_parsers[CMD_EVENT_RECV]               = event_recv_decoder;
}

bool I2c_ucode::decode(uint32_t ucode)
{
    uint8_t ucode_id = (uint8_t) (ucode >> 24);
    auto it = this->id_parsers.find(ucode_id);

    UCODE_DEBUG("ucode_id: %d\n", ucode_id);

    if (this->id_parsers.end() != it)
    {
        UCODE_DEBUG("Decoding\n");
        ucode_data_t data = it->second(ucode);

        this->callback(data);
        return true;
    }

    UCODE_DEBUG("parser not found\n");
    return false;
}

void I2c_ucode::register_callback(ucode_callback_t callback)
{
    this->callback = callback;
}
