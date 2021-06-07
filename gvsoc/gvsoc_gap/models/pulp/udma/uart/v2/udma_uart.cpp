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

#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/uart.hpp"
#include "udma_uart.hpp"

using namespace std::placeholders;


Uart_periph::Uart_periph(udma *top, int id, int itf_id)
    : Udma_periph(top, id),
    rx_fsm(top, this),
    tx_fsm(top, this),
    tx_bit(1),
    rx_rts(1)
{
    std::string itf_name = "uart" + std::to_string(itf_id);

    /* create trace utility */
    top->traces.new_trace(itf_name, &trace, vp::DEBUG);

    /* create udma channels */
    this->rx_channel = new Uart_rx_channel(top, this, itf_name + "_rx");
    this->tx_channel = new Uart_tx_channel(top, this, itf_name + "_tx");

    /* register UART external port */
    top->new_master_port(this, itf_name, &(this->uart_itf));

    /* setup pad synchronization method */
    this->uart_itf.set_sync_full_meth(&Uart_periph::rx_sync);

    /* build the register map */
    this->regmap.build(top, &this->trace, itf_name);

    /* setup register callbacks */
    this->regmap.rx_dest.register_callback(std::bind(&Uart_periph::rx_dest_req, this, _1, _2, _3, _4));
    this->regmap.tx_dest.register_callback(std::bind(&Uart_periph::tx_dest_req, this, _1, _2, _3, _4));
    this->regmap.status.register_callback(std::bind(&Uart_periph::status_req, this, _1, _2, _3, _4));
    this->regmap.error.register_callback(std::bind(&Uart_periph::error_req, this, _1, _2, _3, _4));
    this->regmap.misc.register_callback(std::bind(&Uart_periph::misc_req, this, _1, _2, _3, _4));

    /* get soc event numbers */
    {
        this->error_soc_event = -1;
        js::config *config = this->top->get_js_config()->get("uart/err_events");
        if (config)
        {
            this->error_soc_event = config->get_elem(itf_id)->get_int();
        }
    }
    {
        this->rx_soc_event = -1;
        js::config *config = this->top->get_js_config()->get("uart/rx_events");
        if (config)
        {
            this->rx_soc_event = config->get_elem(itf_id)->get_int();
        }
    }
    {
        this->tx_soc_event = -1;
        js::config *config = this->top->get_js_config()->get("uart/tx_events");
        if (config)
        {
            this->tx_soc_event = config->get_elem(itf_id)->get_int();
        }
    }
}


void Uart_periph::rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.rx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting RX channel (id: %d)\n", this->regmap.rx_dest.rx_dest_get());
        this->top->channel_register(this->regmap.rx_dest.rx_dest_get(), this->rx_channel);
    }
}


void Uart_periph::tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.tx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting TX channel (id: %d)\n", this->regmap.tx_dest.tx_dest_get());
        this->top->channel_register(this->regmap.tx_dest.tx_dest_get(), this->tx_channel);

        /* kickstart transfer */
        // TODO add guards
        this->trace.msg(vp::trace::LEVEL_TRACE, "getting 1 byte of data\n");
        this->tx_channel->get_data(1);
    }
}


void Uart_periph::status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.status.update(reg_offset, size, value, is_write);
    //TODO do what ?
}

void Uart_periph::error_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    /* any read (or write, but register is not write-able) clears the register */
    this->regmap.error.set(0);
}

void Uart_periph::misc_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.misc.update(reg_offset, size, value, is_write);

    if (this->regmap.misc.tx_fifo_clear_event_o_get() != 0)
    {
        this->tx_fsm.clear_fifo();
        this->regmap.misc.tx_fifo_clear_event_o_set(0);
    }

    if (this->regmap.misc.tx_fsm_reset_event_o_get() != 0)
    {
        this->tx_fsm.clear_fsm();
        this->regmap.misc.tx_fsm_reset_event_o_set(0);
    }

    if (this->regmap.misc.rx_fifo_clear_event_o_get() != 0)
    {
        this->rx_fsm.clear_fifo();
        this->regmap.misc.rx_fifo_clear_event_o_set(0);
    }

    if (this->regmap.misc.rx_fsm_reset_event_o_get() != 0)
    {
        this->rx_fsm.clear_fsm();
        this->regmap.misc.rx_fsm_reset_event_o_set(0);
    }
}


void Uart_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->tx_fsm.reset(active);
    this->rx_fsm.reset(active);
}



vp::io_req_status_e Uart_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}

void Uart_periph::send_bit(int bit)
{
    this->tx_bit = bit;
    this->sync_pins();
}

void Uart_periph::send_rts(int rts)
{
    this->rx_rts = rts;
    this->sync_pins();
}

void Uart_periph::sync_pins(void)
{
    if (this->uart_itf.is_bound())
    {
        this->uart_itf.sync_full(this->tx_bit, 2, this->rx_rts);
    }
}


void Uart_periph::rx_sync(void *__this, int data, int sck, int cts)
{
    Uart_periph *_this = (Uart_periph *)__this;
    _this->trace.msg(vp::trace::LEVEL_TRACE, "rx_sync (%d, %d, %d)\n", data, sck, cts);
    _this->rx_fsm.handle_rx_bits(data, sck);
    _this->tx_fsm.update_cts(cts);
}


Uart_tx_channel::Uart_tx_channel(udma *top, Uart_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}

void Uart_tx_channel::push_data(uint8_t* data, int size)
{
    this->periph->tx_fsm.push_bytes(data, size);
}

Uart_rx_channel::Uart_rx_channel(udma *top, Uart_periph *periph, string name)
    : Udma_rx_channel(top, name), periph(periph)
{
}

void Uart_rx_channel::wait_active_done(void)
{
    this->periph->rx_fsm.notify_active();
}
