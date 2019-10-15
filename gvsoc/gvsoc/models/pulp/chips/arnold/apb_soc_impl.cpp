/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>

#include "archi/chips/arnold/apb_soc.h"

class apb_soc_ctrl : public vp::component
{

public:

  apb_soc_ctrl(const char *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  void reset(bool active);
  static void bootsel_sync(void *__this, int value);
  static void confreg_ext_sync(void *__this, uint32_t value);

  vp::trace     trace;
  vp::io_slave in;

  vp::wire_master<uint32_t> bootaddr_itf;
  vp::wire_master<int>  event_itf;
  vp::wire_slave<int>   bootsel_itf;

  vp::wire_master<uint32_t> confreg_soc_itf;
  vp::wire_slave<uint32_t> confreg_ext_itf;

  uint32_t core_status;
  uint32_t bootaddr;
  int bootsel;

  vp::reg_32     jtag_reg_ext;
};

apb_soc_ctrl::apb_soc_ctrl(const char *config)
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
  else if (offset == APB_SOC_PADS_CONFIG)
  {
    if (!is_write)
    {
      *(uint32_t *)data = _this->bootsel;
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
  else if (offset == APB_SOC_JTAG_REG)
  {
    if (is_write)
    {
      _this->confreg_soc_itf.sync(*(uint32_t *)data);
    }
    else
    {
      *(uint32_t *)data = _this->jtag_reg_ext.get() << APB_SOC_JTAG_REG_EXT_BIT;
    }
  }
  else
  {

  }


  return vp::IO_REQ_OK;
}

void apb_soc_ctrl::bootsel_sync(void *__this, int value)
{
  apb_soc_ctrl *_this = (apb_soc_ctrl *)__this;
  _this->bootsel = value;
}

void apb_soc_ctrl::confreg_ext_sync(void *__this, uint32_t value)
{
  apb_soc_ctrl *_this = (apb_soc_ctrl *)__this;
  _this->jtag_reg_ext.set(value);
}

int apb_soc_ctrl::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&apb_soc_ctrl::req);
  new_slave_port("input", &in);

  bootsel_itf.set_sync_meth(&apb_soc_ctrl::bootsel_sync);
  new_slave_port("bootsel", &bootsel_itf);

  new_master_port("bootaddr", &this->bootaddr_itf);

  new_master_port("event", &event_itf);

  confreg_ext_itf.set_sync_meth(&apb_soc_ctrl::confreg_ext_sync);
  this->new_slave_port("confreg_ext", &this->confreg_ext_itf);

  this->new_master_port("confreg_soc", &this->confreg_soc_itf);

  this->new_reg("jtag_reg_ext", &this->jtag_reg_ext, 0, false);

  core_status = 0;
  this->bootsel = 0;
  this->jtag_reg_ext.set(0);

  return 0;
}

void apb_soc_ctrl::reset(bool active)
{
}

void apb_soc_ctrl::start()
{
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new apb_soc_ctrl(config);
}
