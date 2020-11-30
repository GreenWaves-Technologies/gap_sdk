/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <math.h>

class bus_watchpoint : public vp::component
{

    friend class MapEntry;

public:

    bus_watchpoint(js::config *config);

    int build();


private:

    static vp::io_req_status_e req(void *__this, vp::io_req *req);


    vp::trace     trace;

    vp::io_master out;
    vp::io_slave in;

    unsigned int riscv_fesvr_tohost_addr;
};


bus_watchpoint::bus_watchpoint(js::config *config)
: vp::component(config)
{
}


vp::io_req_status_e bus_watchpoint::req(void *__this, vp::io_req *req)
{
    bus_watchpoint *_this = (bus_watchpoint *)__this;
    uint64_t offset = req->get_addr();
    bool is_write = req->get_is_write();
    uint64_t size = req->get_size();
    uint8_t *data = req->get_data();

    _this->trace.msg("Received IO req (req: %p, offset: 0x%llx, size: 0x%llx, is_write: %d)\n", req, offset, size, is_write);

    if (offset == _this->riscv_fesvr_tohost_addr && size == 4)
    {
        _this->clock->stop_engine((*(uint32_t *)data) >> 1);
    }

    return _this->out.req_forward(req);
}


int bus_watchpoint::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&bus_watchpoint::req);
  new_slave_port("input", &in);

  new_master_port("output", &out);

  this->riscv_fesvr_tohost_addr = this->get_config_int("riscv_fesvr_tohost_addr");

  return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new bus_watchpoint(config);
}
