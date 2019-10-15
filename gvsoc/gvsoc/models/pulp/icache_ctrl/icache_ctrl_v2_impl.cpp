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
#include <stdio.h>
#include <string.h>

class icache_ctrl : public vp::component
{

public:

  icache_ctrl(const char *config);

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

icache_ctrl::icache_ctrl(const char *config)
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

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new icache_ctrl(config);
}
