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
    this->active_transfer = false;
}


void Udma_addrgen_linear::check_pending_transfer()
{
    if (!this->active_transfer && this->nb_pending_transfers)
    {
        this->nb_pending_transfers--;
        this->active_transfer = true;

        this->current_addr = this->pending_addr;
        this->current_size = this->pending_size;
    }
}


void Udma_addrgen_linear::cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ctrl.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        if (this->regmap.cfg_ctrl.stop_get())
        {
            trace.msg(vp::trace::LEVEL_TRACE, "UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
        }
        else if (this->regmap.cfg_ctrl.en_get())
        {
            this->pending_addr = this->regmap.cfg_sa_buf0.get();
            this->pending_size = this->regmap.cfg_size.get();

            this->nb_pending_transfers++;

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


void Udma_addrgen_linear::get_next_transfer(uint32_t *addr, int *size)
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

        if (this->current_size == 0)
        {
            this->active_transfer = false;
            this->top->trigger_event(this->event_id);
            this->check_pending_transfer();
        }
    }
    else
    {
        *size = 0;
    }

    trace.msg(vp::trace::LEVEL_TRACE, "Providing next chunk (addr: 0x%x, size: 0x%x)\n", *addr, *size);
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
    this->active_transfer = false;
}


void Udma_addrgen_2d::check_pending_transfer()
{
    if (!this->active_transfer && this->nb_pending_transfers)
    {
        this->nb_pending_transfers--;
        this->active_transfer = true;

        this->current_addr = this->pending_addr;
        this->current_size = this->pending_size;
        this->current_stride = this->pending_stride;
        this->current_length = this->pending_length;
        this->remaining_length = this->pending_length;
    }
}


void Udma_addrgen_2d::cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ctrl.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        if (this->regmap.cfg_ctrl.stop_get())
        {
            trace.msg(vp::trace::LEVEL_TRACE, "UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
        }
        else if (this->regmap.cfg_ctrl.en_get())
        {
            this->pending_addr = this->regmap.cfg_sa_buf0.get();
            this->pending_size = this->regmap.cfg_size.get();
            this->pending_stride = this->regmap.cfg_stride.get();
            this->pending_length = this->regmap.cfg_row_len.get();

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


void Udma_addrgen_2d::get_next_transfer(uint32_t *addr, int *size)
{
    if (this->current_size > 0)
    {
        int iter_size = 4;
        
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

        if (this->current_size == 0)
        {
            this->active_transfer = false;
            this->top->trigger_event(this->event_id);
            this->check_pending_transfer();
        }
    }
    else
    {
        *size = 0;
    }

    trace.msg(vp::trace::LEVEL_TRACE, "Providing next chunk (addr: 0x%x, size: 0x%x)\n", *addr, *size);
}
