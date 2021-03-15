/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
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
#include "udma_aes_v1.hpp"

#include "archi/utils.h"

using namespace std::placeholders;


Aes_periph::Aes_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "aes" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &this->trace, vp::DEBUG);

    /* Build the register map. It is auto-generated from the IP specs */
    this->regmap.build(top, &this->trace, itf_name);

    /* setup tx/rx channel register callbacks */
    this->regmap.dest.register_callback(std::bind(&Aes_periph::dest_req, this, _1, _2, _3, _4));

    /* setup udma channels */
    this->rx_channel = new Aes_rx_channel(top, this, itf_name + "_rx");
    this->tx_channel = new Aes_tx_channel(top, this, itf_name + "_tx");
}


void Aes_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->rx_channel->reset(active);
    this->tx_channel->reset(active);
}


void Aes_periph::dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.dest.update(reg_offset, size, value, is_write);

    this->trace.msg(vp::trace::LEVEL_TRACE, "Set DEST channels\n");
    // TODO
    //if (is_write)
    //{
    //    this->top->channel_register(this->regmap.rx_dest.get(), this->rx_channel);
    //}
}

vp::io_req_status_e Aes_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "aes_register_req\n");
    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}

Aes_tx_channel::Aes_tx_channel(udma *top, Aes_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}

void Aes_tx_channel::push_data(uint8_t *data, int size)
{
}


Aes_rx_channel::Aes_rx_channel(udma *top, Aes_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}
