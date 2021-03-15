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
 * Authors: Vladimir Popovic, GreenWaves Technologies (vladimir.popovic@greenwaves-technologies.com)
 */


#include "udma_sfu_v1_empty.hpp"




Sfu_periph_empty::Sfu_periph_empty(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "sfu" + std::to_string(itf_id);
    top->traces.new_trace(itf_name, &this->trace, vp::DEBUG);
    top->new_master_port("sfu_irq", &this->irq);
}
 

vp::io_req_status_e Sfu_periph_empty::custom_req(vp::io_req *req, uint64_t offset)
{
    vp_warning_always(&this->trace, "SFU model not implemented\n");
    return vp::IO_REQ_INVALID;
}
