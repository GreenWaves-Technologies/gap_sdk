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
 * Authors: Antoine Faravelon, GreenWaves Technologies (antoine.faravelon@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include "vp/itf/wire.hpp"
#include <archi/chips/gap9_v2/xip/xip_regs.h>
#include <archi/chips/gap9_v2/xip/xip_regfields.h>
#include <archi/chips/gap9_v2/xip/xip_gvsoc.h>
#include "pulp/udma/hyper/udma_hyper_v3_refill.hpp"



typedef struct {
    int req;
} fetch_t;

class Xip : public vp::component
{

public:
    Xip(js::config *config);

    int build();
    void reset(bool active);

private:
    static vp::io_req_status_e apb_req(void *__this, vp::io_req *req);
    static vp::io_req_status_e fc_data_req(void *__this, vp::io_req *req);
    static vp::io_req_status_e fc_fetch_req(void *__this, vp::io_req *req);
    static void refill_reply(void *__this, udma_refill_req_t *);

    vp::trace trace;
    vp::io_slave apb_itf;
    vp::io_slave fc_data_in_itf;
    vp::io_slave fc_fetch_in_itf;
    vp::io_master fc_data_out_itf;
    vp::io_master fc_fetch_out_itf;
    vp_regmap_xip regmap;
    udma_refill_req_t refill_req;

    std::vector<vp::wire_master<udma_refill_req_t *> *>     refill_itfs;
};


Xip::Xip(js::config *config)
    : vp::component(config)
{
}


vp::io_req_status_e Xip::apb_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;

    if (_this->regmap.access(req->get_addr(), req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}


vp::io_req_status_e Xip::fc_data_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;

    // L2 req addr: req->get_addr()
    // L2 req data: req->get_data()
    // L2 req size: req->get_size()
    // L2 req is write: req->req->get_is_write()
    _this->refill_req.ext_addr = 0x123456;
    _this->refill_req.l2_addr = 0x1c000000;
    _this->refill_req.size = 0x400;
    _this->refill_req.is_write = 0;

    _this->refill_itfs[0]->sync(&_this->refill_req);

    return vp::IO_REQ_PENDING;
}


vp::io_req_status_e Xip::fc_fetch_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;

    // L2 req addr: req->get_addr()
    // L2 req data: req->get_data()
    // L2 req size: req->get_size()
    // L2 req is write: req->req->get_is_write()
    _this->refill_req.ext_addr = 0x123456;
    _this->refill_req.l2_addr = 0x1c000000;
    _this->refill_req.size = 0x400;
    _this->refill_req.is_write = 0;

    _this->refill_itfs[0]->sync(&_this->refill_req);

    return vp::IO_REQ_PENDING;
}


void Xip::refill_reply(void *__this, udma_refill_req_t *)
{
    printf("Refill reply\n");
}


int Xip::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    apb_itf.set_req_meth(&Xip::apb_req);
    new_slave_port("apb_input", &apb_itf);

    fc_data_in_itf.set_req_meth(&Xip::fc_data_req);
    new_slave_port("fc_data_input", &fc_data_in_itf);
    new_master_port("fc_data_output", &fc_data_out_itf);

    fc_fetch_in_itf.set_req_meth(&Xip::fc_fetch_req);
    new_slave_port("fc_fetch_input", &fc_fetch_in_itf);
    new_master_port("fc_fetch_output", &fc_fetch_out_itf);

    this->regmap.build(this, &this->trace, "xip");

    for (int i=0; i<this->get_js_config()->get_child_int("nb_refill_itfs"); i++)
    {
        vp::wire_master<udma_refill_req_t *> *itf = new vp::wire_master<udma_refill_req_t *>();
        itf->set_sync_meth(&Xip::refill_reply);
        new_master_port("refill_" + std::to_string(i), itf);
        this->refill_itfs.push_back(itf);
    }

    return 0;
}

void Xip::reset(bool active)
{
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Xip(config);
}
