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

#ifndef __PULP_UDMA_UDMA_SFU_V1_EMPTY_HPP__
#define __PULP_UDMA_UDMA_SFU_V1_EMPTY_HPP__

#include <vp/vp.hpp>
#include "../udma_impl.hpp"



class Sfu_periph_empty : public Udma_periph
{

public:
    Sfu_periph_empty(udma *top, int id, int itf_id);
    vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);

    vp::trace         trace;

private:

    vp::wire_master<bool> irq;
};


#endif