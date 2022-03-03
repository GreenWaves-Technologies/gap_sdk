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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "archi/utils.h"
#include "udma_v4_impl.hpp"
#include "udma_ctrl/udma_ctrl_regs.h"
#include "udma_ctrl/udma_ctrl_regfields.h"
#include "udma_ctrl/udma_ctrl_gvsoc.h"

using namespace std::placeholders;


void Udma_addrgen::register_channel(Udma_channel *channel)
{
    this->channel = channel;
    if (channel)
    {
        channel->set_active(this->active_transfer);
    }
}


void Udma_addrgen::set_active_transfer(bool active)
{
    this->active_transfer = active;

    if (this->channel)
    {
        this->channel->set_active(active);
    }
}


Udma_addrgen_linear::Udma_addrgen_linear(udma *top, int id, int event_id) : Udma_addrgen(top, id, event_id)
{
    std::string name = "addrgen_linear_" + std::to_string(id);
    top->traces.new_trace(name + "/trace", &this->trace, vp::DEBUG);
    this->regmap.build(top, &this->trace, name);
    this->regmap.cfg_ctrl.register_callback(std::bind(&Udma_addrgen_linear::cfg_ctrl_req, this, _1, _2, _3, _4));
}


void Udma_addrgen_linear::reset(bool active)
{
    this->nb_pending_transfers = 0;
    this->channel = NULL;
    this->set_active_transfer(false);
    this->remaining_size = 0;
}

void Udma_addrgen_linear::check_pending_transfer()
{
    if (!this->active_transfer && this->nb_pending_transfers)
    {
        trace.msg(vp::trace::LEVEL_TRACE, "Starting new buffer (addr: 0x%x, size: 0x%x, pending: %d)\n", this->pending_addr, this->pending_size, this->nb_pending_transfers);

        this->nb_pending_transfers--;
        this->set_active_transfer(true);

        this->current_addr = this->pending_addr;
        this->current_size = this->pending_size;

        this->regmap.cfg_bytes_left.set(this->current_size);
    }
}


void Udma_addrgen_linear::cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ctrl.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        if (this->regmap.cfg_ctrl.stop_get())
        {
            this->nb_pending_transfers = 0;
            this->set_active_transfer(false);
        }
        else if (this->regmap.cfg_ctrl.en_get())
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Enqeueing transfer (pending: %d)\n", this->nb_pending_transfers);

            if (this->nb_pending_transfers == 1)
            {
                this->trace.force_warning("Trying to enqueue while alreay 2 transfers are enqueued\n");
            }
            else
            {
                if (this->nb_pending_transfers > 0)
                {
                    this->remaining_size -= this->pending_size;
                }

                this->pending_addr = this->regmap.cfg_sa_buf0.get();
                this->pending_size = this->regmap.cfg_size.get();
                this->remaining_size += this->pending_size;

                this->nb_pending_transfers++;
            }

            this->check_pending_transfer();

            if (this->channel)
            {
                this->channel->check_state();
            }
        }
    }
}


vp::io_req_status_e Udma_addrgen_linear::access(uint64_t offset, int size, uint8_t *value, bool is_write)
{

    if (this->regmap.access(offset, size, value, is_write))
        return vp::IO_REQ_INVALID;

    return vp::IO_REQ_OK;
}


bool Udma_addrgen_linear::get_next_transfer(uint32_t *addr, int *size)
{
    if (this->current_size > 0)
    {
        int iter_size = 4;
        
        if (iter_size > this->current_size)
            iter_size = this->current_size;

        if (iter_size > *size)
            iter_size = *size;

        *addr = this->current_addr;
        *size = iter_size;

        this->current_size -= iter_size;
        this->current_addr += iter_size;

        this->regmap.cfg_bytes_left.set(this->current_size);

        if (this->current_size == 0)
        {
            this->set_active_transfer(false);
            this->top->trigger_event(this->event_id);
            this->check_pending_transfer();
        }
        trace.msg(vp::trace::LEVEL_TRACE, "Providing next chunk (addr: 0x%x, size: 0x%x)\n", *addr, *size);
        return false;
    }
    else
    {
        *addr = 0;
        *size = 0;
        trace.force_warning("Getting next chunk with no transfer\n");
        return true;
    }
}






Udma_addrgen_2d::Udma_addrgen_2d(udma *top, int id, int event_id) : Udma_addrgen(top, id, event_id)
{
    std::string name = "addrgen_2d_" + std::to_string(id);
    top->traces.new_trace(name + "/trace", &this->trace, vp::DEBUG);
    this->regmap.build(top, &this->trace, name);
    this->regmap.cfg_ctrl.register_callback(std::bind(&Udma_addrgen_2d::cfg_ctrl_req, this, _1, _2, _3, _4));
}


void Udma_addrgen_2d::reset(bool active)
{
    this->nb_pending_transfers = 0;
    this->set_active_transfer(false);
    this->remaining_size = 0;
}


void Udma_addrgen_2d::check_pending_transfer()
{
    if (!this->active_transfer && this->nb_pending_transfers)
    {
        this->nb_pending_transfers--;
        this->set_active_transfer(true);

        this->current_addr = this->pending_addr;
        this->current_size = this->pending_size;
        this->current_stride = this->pending_stride;
        this->current_length = this->pending_length;
        this->remaining_length = this->pending_length;

        trace.msg(vp::trace::LEVEL_TRACE, "Activating transfer (addr: 0x%x, size: 0x%x, stride: 0x%x, length: 0x%x)\n",
            this->pending_addr, this->pending_size, this->pending_stride, this->pending_length);

    }
}


void Udma_addrgen_2d::cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ctrl.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        if (this->regmap.cfg_ctrl.stop_get())
        {
            this->nb_pending_transfers = 0;
            this->set_active_transfer(false);
        }
        else if (this->regmap.cfg_ctrl.en_get())
        {
            if (this->nb_pending_transfers > 0)
            {
                this->remaining_size -= this->pending_size;
            }

            this->pending_addr = this->regmap.cfg_sa_buf0.get();
            this->pending_size = this->regmap.cfg_size.get();
            this->pending_stride = this->regmap.cfg_stride.get();
            this->pending_length = this->regmap.cfg_row_len.get();
            this->remaining_size += this->pending_size;

            trace.msg(vp::trace::LEVEL_TRACE, "Configuring (addr: 0x%x, size: 0x%x, stride: 0x%x, length: 0x%x)\n",
                this->pending_addr, this->pending_size, this->pending_stride, this->pending_length);

            this->nb_pending_transfers++;

            this->check_pending_transfer();
        }
    }
}


vp::io_req_status_e Udma_addrgen_2d::access(uint64_t offset, int size, uint8_t *value, bool is_write)
{
    if (this->regmap.access(offset, size, value, is_write))
        return vp::IO_REQ_INVALID;

    return vp::IO_REQ_OK;
}


bool Udma_addrgen_2d::get_next_transfer(uint32_t *addr, int *size)
{
    trace.msg(vp::trace::LEVEL_TRACE, "Getting next chunk (current_addr: 0x%x, current_size: 0x%x, remaining_length: 0x%x)\n", this->current_addr, this->current_size, this->remaining_length);

    if (this->current_size > 0)
    {
        int align = this->current_addr & 0x3;
        int iter_size = 4 - align;
        
        if (iter_size > this->remaining_length)
            iter_size = this->remaining_length;

        if (iter_size > this->current_size)
            iter_size = this->current_size;

        if (iter_size > *size)
            iter_size = *size;

        *addr = this->current_addr;
        *size = iter_size;

        this->current_size -= iter_size;
        this->remaining_length -= iter_size;
        this->current_addr += iter_size;

        if (this->remaining_length == 0)
        {
            this->current_addr = this->current_addr - this->current_length + this->current_stride;
            this->remaining_length = this->current_length;
        }

        trace.msg(vp::trace::LEVEL_TRACE, "Updated current chunk (current_addr: 0x%x, current_size: 0x%x, remaining_length: 0x%x)\n", this->current_addr, this->current_size, this->remaining_length);

        if (this->current_size == 0)
        {
            this->set_active_transfer(false);
            this->top->trigger_event(this->event_id);
            this->check_pending_transfer();
        }

        trace.msg(vp::trace::LEVEL_TRACE, "Providing next chunk (addr: 0x%x, size: 0x%x)\n", *addr, *size);

        return false;
    }
    else
    {
        *size = 0;
        trace.force_warning("Getting next chunk with no transfer\n");
        return true;
    }
}
