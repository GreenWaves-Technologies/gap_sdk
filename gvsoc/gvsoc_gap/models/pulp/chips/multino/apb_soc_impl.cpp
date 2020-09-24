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
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>

#include "archi/chips/multino/apb_soc.h"

class apb_soc_ctrl : public vp::component
{

public:

  apb_soc_ctrl(js::config *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::trace     trace;
  vp::io_slave in;

  vp::wire_master<uint32_t> bootaddr_itf;
  vp::wire_master<int>  event_itf;
  
  uint32_t core_status;
  uint32_t bootaddr;

};

apb_soc_ctrl::apb_soc_ctrl(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e apb_soc_ctrl::req(void *__this, vp::io_req *req)
{
  apb_soc_ctrl *_this = (apb_soc_ctrl *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Apb_soc_ctrl access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  if (offset == APB_SOC_CORESTATUS_OFFSET)
  {
    if (!is_write)
    {
      *(uint32_t *)data = _this->core_status;
    }
    else
    {
      // We are writing to the status register, the 31 LSBs are the return value of the platform and the last bit
      // makes the platform exit when set to 1
      _this->core_status = *(uint32_t *)data;
      
      if ((_this->core_status >> APB_SOC_STATUS_EOC_BIT) & 1) 
      {
        _this->clock->stop_engine(_this->core_status & 0x7fffffff);
      }
    }
  }
  else if (offset == APB_SOC_BOOTADDR_OFFSET)
  {
    if (is_write)
    {
      _this->trace.msg("Setting boot address (addr: 0x%x)\n", *(uint32_t *)data);
      if (_this->bootaddr_itf.is_bound())
        _this->bootaddr_itf.sync(*(uint32_t *)data);
      
      _this->bootaddr = *(uint32_t *)data;
    }
    else *(uint32_t *)data = _this->bootaddr;
  }
  else
  {

  }


  return vp::IO_REQ_OK;
}

int apb_soc_ctrl::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&apb_soc_ctrl::req);
  new_slave_port("input", &in);

  new_master_port("bootaddr", &this->bootaddr_itf);

  new_master_port("event", &event_itf);

  core_status = 0;

  return 0;
}

void apb_soc_ctrl::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new apb_soc_ctrl(config);
}
