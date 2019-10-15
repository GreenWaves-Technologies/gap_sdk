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

class xne : public vp::component
{

public:

  xne(const char *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::trace     trace;
  vp::io_slave in;
  vp::io_master out;
};

xne::xne(const char *config)
: vp::component(config)
{

}

vp::io_req_status_e xne::req(void *__this, vp::io_req *req)
{
  xne *_this = (xne *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("xne access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_OK;
}

int xne::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&xne::req);
  new_slave_port("in", &in);
  new_master_port("out", &out);

  return 0;
}

void xne::start()
{
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new xne(config);
}
