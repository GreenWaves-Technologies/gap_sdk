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

#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/i2c.hpp"
#include "udma_i2c.hpp"

I2c_periph::I2c_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "i2c" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &trace, vp::DEBUG);

    /* create udma channels */
    this->rx_channel = new I2c_rx_channel(top, this, itf_name + "_rx");
    this->tx_channel = new I2c_tx_channel(top, this, itf_name + "_tx");

    top->new_master_port(this, itf_name, &i2c_itf);

    /* build the register map */
    this->regmap.build(top, &this->trace, itf_name);

}

void I2c_periph::reset(bool active)
{
    Udma_periph::reset(active);

    if (active)
    {
    }
}

vp::io_req_status_e I2c_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}

I2c_tx_channel::I2c_tx_channel(udma *top, I2c_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void I2c_tx_channel::push_data(uint8_t *data, int size)
{

}


I2c_rx_channel::I2c_rx_channel(udma *top, I2c_periph *periph, string name)
    : Udma_rx_channel(top, name), periph(periph)
{
}

void I2c_rx_channel::wait_active_done(void)
{

}