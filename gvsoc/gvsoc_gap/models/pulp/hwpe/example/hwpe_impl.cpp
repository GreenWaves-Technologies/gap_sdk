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

#define TOTAL_REQ 10

class hwpe : public vp::component
{

public:

  hwpe(js::config *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

  static void event_handler(void *_this, vp::clock_event *event);

  static void grant(void *_this, vp::io_req *req);

  static void response(void *_this, vp::io_req *req);

  void check_requests();

private:

  vp::trace     trace;
  vp::io_slave in;
  vp::io_master out;

  vp::clock_event *event;

  int pending_req = 0;
  int enqueued_req = 0;
  int remaining_req = 0;
  bool stalled = false;

};

hwpe::hwpe(js::config *config)
: vp::component(config)
{

}

void hwpe::check_requests()
{
  // We can continue to send requests if we are not stalled, we didn't send all
  // requests and there are less than 2 pending requests.
  if (!stalled && pending_req < 2 && enqueued_req < TOTAL_REQ)
  {
    event_enqueue(event, 1);    
  }

}

void hwpe::event_handler(void *__this, vp::clock_event *event)
{
  hwpe *_this = (hwpe *)__this;
  _this->trace.msg("Entered event handler\n");

  _this->enqueued_req++;
  vp::io_req *req = _this->out.req_new(0x1c010000 + (_this->enqueued_req - 1) * 4, new uint8_t[4], 4, 1);
  int err = _this->out.req(req);
  if (err)
  {
    if (err == vp::IO_REQ_DENIED)
    {
      // If the access is not granted, block the hwpe until it is granted
      _this->stalled = true;
      // We mark it as pending now as we will receive a call to the grant callback
      // telling that the requests is now pending
      _this->pending_req++;
    }
    else if (err == vp::IO_REQ_PENDING)
    {
      // Request was granted but is pending, just account it
      _this->pending_req++;
    }
  }
  else
  {
    // Request is finished
    _this->remaining_req--;
  }

  _this->check_requests();
}

vp::io_req_status_e hwpe::req(void *__this, vp::io_req *req)
{
  hwpe *_this = (hwpe *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (offset == 0)
  {
    if (_this->remaining_req == 0)
    {
      _this->remaining_req = TOTAL_REQ;
      _this->event_enqueue(_this->event, 1);
    }
  }
  else if (offset == 4)
  {
    if (!is_write && size == 4) *(uint32_t *)data = _this->remaining_req;
    _this->trace.msg("Returning %x\n", *(uint32_t *)data);
  }


  return vp::IO_REQ_OK;
}


void hwpe::grant(void *__this, vp::io_req *req)
{
  hwpe *_this = (hwpe *)__this;

  _this->trace.msg("Got grant (req: %p)\n", req);

  _this->stalled = false;

  _this->check_requests();
}

void hwpe::response(void *__this, vp::io_req *req)
{
  hwpe *_this = (hwpe *)__this;

  _this->trace.msg("Got response (req: %p)\n", req);

  _this->pending_req--;
  _this->remaining_req--;

  _this->check_requests();
}

int hwpe::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&hwpe::req);
  new_slave_port("in", &in);

  out.set_resp_meth(&hwpe::response);
  out.set_grant_meth(&hwpe::grant);
  new_master_port("out", &out);

  event = event_new(hwpe::event_handler);

  return 0;
}

void hwpe::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new hwpe(config);
}
