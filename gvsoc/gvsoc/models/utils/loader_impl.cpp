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

class loader : public vp::component
{

public:

  loader(js::config *config);

  int build();
  void start();

  void io_req(uint64_t addr, uint64_t size, bool is_write, uint8_t *data);
  void memset(uint64_t addr, uint64_t size, uint8_t value);
  bool send_req(vp::io_req *req);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

  static void grant(void *_this, vp::io_req *req);
  static void response(void *_this, vp::io_req *req);

private:

  void do_io_req(uint64_t addr, uint64_t size, bool is_write, uint8_t *data);
  std::list<vp::io_req *> pending_reqs;
  vp::trace     trace;
  vp::io_master out;
};

loader::loader(js::config *config)
: vp::component(config)
{
}

void loader::grant(void *_this, vp::io_req *req)
{
}

bool loader::send_req(vp::io_req *req)
{  
  vp::io_req_status_e err = out.req(req);
  if (err == vp::IO_REQ_OK) return false;
  else if (err == vp::IO_REQ_INVALID)
  {
    warning.warning("Invalid access while loading binary (addr: 0x%x, size: 0x%x, is_write: %d)\n", req->get_addr(), req->get_size(), true);
    return false;
  }
  return true;
}

void loader::response(void *__this, vp::io_req *req)
{
  loader *_this = (loader *)__this;
  _this->pending_reqs.pop_front();
  delete req->get_data();
  _this->out.req_del(req);

  while(1)
  {
    if (_this->pending_reqs.empty()) break;

    if (_this->send_req(_this->pending_reqs.front())) break;

    _this->pending_reqs.pop_front();
  }
}

int loader::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  out.set_resp_meth(&loader::response);
  out.set_grant_meth(&loader::grant);
  new_master_port("out", &out);
  
  return 0;
}

void loader::start()
{
}

void loader::do_io_req(uint64_t addr, uint64_t size, bool is_write, uint8_t *data)
{

  void *req_data = new uint8_t[size];
  memcpy(req_data, data, size);

  vp::io_req *req = out.req_new(addr, (uint8_t *)req_data, size, is_write);

  if (!this->pending_reqs.empty())
  {
    this->pending_reqs.push_back(req);
  }
  else
  {
    if (send_req(req))
    {
      this->pending_reqs.push_back(req);
    }
  }
}

void loader::io_req(uint64_t addr, uint64_t size, bool is_write, uint8_t *data)
{
  trace.msg("Loading section (base: 0x%x, size: 0x%x, is_write: %d)\n", addr, size, is_write);

  this->do_io_req(addr, size, is_write, data);
}

void loader::memset(uint64_t addr, uint64_t size, uint8_t value)
{
  trace.msg("Padding section (base: 0x%x, size: 0x%x, value: %d)\n", addr, size, value);

  uint8_t data[size];
  ::memset((void *)data, value, size);

  this->do_io_req(addr, size, true, data);
}

extern "C" void loader_io_req(void *__this, uint64_t addr, uint64_t size, bool is_write, uint8_t *data)
{
  loader *_this = (loader *)__this;
  _this->io_req(addr, size, is_write, data);
}

extern "C" void loader_memset(void *__this, uint64_t addr, uint64_t size, uint8_t value)
{
  loader *_this = (loader *)__this;
  _this->memset(addr, size, value);
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new loader(config);
}
