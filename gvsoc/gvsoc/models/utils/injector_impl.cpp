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
#include <vp/launcher_internal.hpp>
#include <vp/itf/io.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

class injector : public vp::component {

public:

  injector(js::config *config);

  int build();
  void start();

private:

  vp::trace trace;
  vp::io_slave in;
  vp::io_master out;

  vp::io_req ext_req;
  void *binding_context;

  FILE *snd_file;
  FILE *rcv_file;

  static vp::io_req_status_e req(void *__this, vp::io_req *req);
  void binding_routine();

};

injector::injector(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e injector::req(void *__this, vp::io_req *req)
{
  injector *_this = (injector *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("IO access (offset: 0x%lx, size: 0x%lx, is_write: %d)\n", offset, size, req->get_is_write());

  if (_this->snd_file == NULL)
  {
    _this->trace.force_warning("Accessing injector while it is not connected\n");
    return vp::IO_REQ_INVALID;
  }

  gv_ioreq_desc_t desc = { .type=GV_IOREQ_DESC_TYPE_REQUEST, .addr=(uint64_t)offset, .size=(uint64_t)size,
    .is_write=(int64_t)is_write, .timestamp=_this->get_time(), .response_cb=NULL, .response_context=NULL,
    .binding=(void *)_this->binding_context, .req=req
  };

  if (fwrite((void *)&desc, sizeof(desc), 1, _this->snd_file) != 1) return vp::IO_REQ_INVALID;
  if (is_write && fwrite(data, size, 1, _this->snd_file) != 1) return vp::IO_REQ_INVALID;
  fflush(_this->snd_file);

  return vp::IO_REQ_PENDING;
}

void injector::binding_routine()
{
  this->get_clock()->get_engine()->wait_running();

  while(1)
  {
    gv_ioreq_desc_t req;
    if (fread(&req, sizeof(req), 1, rcv_file) != 1) {
      return;
    }

    if (req.type == GV_IOREQ_DESC_TYPE_REQUEST)
    {
      this->trace.msg("Received IO req from external binding (addr: 0x%llx, size: 0x%llx, is_write: %d)\n", req.addr, req.size, req.is_write);

      this->get_clock()->get_engine()->lock();

      uint8_t data[req.size];

      if (req.is_write && fread((void *)data, req.size, 1, rcv_file) != 1) {
        this->get_clock()->get_engine()->unlock();
        return;
      }

      vp::io_req *io_req = &ext_req;
      io_req->init();
      io_req->set_addr(req.addr);
      io_req->set_size(req.size);
      io_req->set_is_write(req.is_write);
      io_req->set_data(data);

      this->get_clock()->sync();

      int err = this->out.req(io_req);

      req.user_req.state = err != vp::IO_REQ_OK ? GV_IOREQ_DONE_ERROR : GV_IOREQ_DONE;
      req.user_req.latency = this->get_time() + io_req->get_latency() + io_req->get_duration();

      this->get_clock()->get_engine()->unlock();

      req.type = GV_IOREQ_DESC_TYPE_RESPONSE;

      if (fwrite(&req, sizeof(req), 1, snd_file) != 1) return;
      if (!req.is_write && fwrite((void *)data, req.size, 1, snd_file) != 1) return;

      fflush(snd_file);
    }
    else
    {
      vp::io_req *ioreq = (vp::io_req *)req.req;
      if (!ioreq->get_is_write() && fread(ioreq->get_data(), ioreq->get_size(), 1, this->rcv_file) != 1) return;
      // Don't specify any timestamp as the initiator will take care of updating
      // the time depending on the latency we report
      this->get_clock()->get_engine()->lock();
      //ioreq->setLatency(getEngine()->getCyclesFromTime(req.latency));
      ioreq->set_latency(0);
      ioreq->get_resp_port()->resp(ioreq);
      this->get_clock()->get_engine()->unlock();
    }
  }
}

int injector::build()
{
  in.set_req_meth(&injector::req);
  new_slave_port("input", &in);

  new_master_port("output", &out);

  traces.new_trace("trace", &trace, vp::DEBUG);

  int snd_fd = this->get_js_config()->get_int("snd_fd");
  int rcv_fd = this->get_js_config()->get_int("rcv_fd");

  this->binding_context = (void *)(long)this->get_js_config()->get_int("context");

  if (snd_fd != -1)
  {
    snd_file = fdopen(snd_fd, "w");
    if (snd_file == NULL)
    {
      snprintf(vp_error, VP_ERROR_SIZE, "Failed to open pipe: %s",  strerror(errno));
      return -1;
    }
  }
  else
  {
    snd_file = NULL;
  }

  if (rcv_fd != -1)
  {
    rcv_file = fdopen(rcv_fd, "r");
    if (rcv_file == NULL)
    {
      snprintf(vp_error, VP_ERROR_SIZE, "Failed to open pipe: %s",  strerror(errno));
      return -1;
    }
  }
  else
  {
    rcv_file = NULL;
  }

  return 0;
}

void injector::start()
{
  if (rcv_file)
  {
    this->get_clock()->retain();
    new std::thread(&injector::binding_routine, this);
  }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new injector(config);
}
