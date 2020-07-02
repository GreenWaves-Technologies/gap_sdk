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

class icache_ctrl : public vp::component
{

public:

  icache_ctrl(js::config *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::trace     trace;
  vp::io_slave in;

  vp::wire_master<bool>     enable_itf;
  vp::wire_master<bool>     flush_itf;
  vp::wire_master<bool>     flush_line_itf;
  vp::wire_master<uint32_t> flush_line_addr_itf;
};

icache_ctrl::icache_ctrl(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e icache_ctrl::req(void *__this, vp::io_req *req)
{
  icache_ctrl *_this = (icache_ctrl *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("icache_ctrl access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (offset == 0)
  {
    if (_this->enable_itf.is_bound())
      _this->enable_itf.sync(*data != 0);
  }

  return vp::IO_REQ_OK;
}

int icache_ctrl::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&icache_ctrl::req);
  new_slave_port("input", &in);

  this->new_master_port("enable", &this->enable_itf);
  this->new_master_port("flush", &this->flush_itf);
  this->new_master_port("flush_line", &this->flush_line_itf);
  this->new_master_port("flush_line_addr", &this->flush_line_addr_itf);

  return 0;
}

void icache_ctrl::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new icache_ctrl(config);
}
