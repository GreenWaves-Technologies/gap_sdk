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
#include <math.h>

class interleaver : public vp::component
{

public:

  interleaver(const char *config);

  int build();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);
  static vp::io_req_status_e req_ts(void *__this, vp::io_req *req);


private:
  vp::trace     trace;

  vp::io_master **out;
  vp::io_slave **masters_in;
  vp::io_slave **masters_ts_in;
  vp::io_slave in;
  vp::io_slave ts_in;

  int nb_slaves;
  int nb_masters;
  int stage_bits;
  uint64_t bank_mask;
  vp::io_req ts_req;
};

interleaver::interleaver(const char *config)
: vp::component(config)
{

}

vp::io_req_status_e interleaver::req(void *__this, vp::io_req *req)
{
  interleaver *_this = (interleaver *)__this;
  uint64_t offset = req->get_addr();
  bool is_write = req->get_is_write();
  uint64_t size = req->get_size();
  uint8_t *data = req->get_data();

  _this->trace.msg("Received IO req (offset: 0x%llx, size: 0x%llx, is_write: %d)\n", offset, size, is_write);
 
  int bank_id = (offset >> 2) & _this->bank_mask;
  uint64_t bank_offset = ((offset >> (_this->stage_bits + 2)) << 2) + (offset & 0x3);

  req->set_addr(bank_offset);
  return _this->out[bank_id]->req_forward(req);
}

vp::io_req_status_e interleaver::req_ts(void *__this, vp::io_req *req)
{
  interleaver *_this = (interleaver *)__this;
  uint64_t offset = req->get_addr();
  bool is_write = req->get_is_write();
  uint64_t size = req->get_size();
  uint8_t *data = req->get_data();

  _this->trace.msg("Received TS IO req (offset: 0x%llx, size: 0x%llx, is_write: %d)\n", offset, size, is_write);
 
  int bank_id = (offset >> 2) & _this->bank_mask;
  uint64_t bank_offset = ((offset >> (_this->stage_bits + 2)) << 2) + (offset & 0x3);

  bank_offset &= ~(1<<(20 - _this->stage_bits));

  if (!is_write)
  {
    req->set_addr(bank_offset);
    vp::io_req_status_e err = _this->out[bank_id]->req_forward(req);
    if (err != vp::IO_REQ_OK) return err;
    _this->trace.msg("Sending test-and-set IO req (offset: 0x%llx, size: 0x%llx)\n", offset & ~(1<<20), size);
    uint64_t ts_data = -1;
    _this->ts_req.set_addr(bank_offset);
    _this->ts_req.set_size(size);
    _this->ts_req.set_is_write(true);
    _this->ts_req.set_data((uint8_t *)&ts_data);
    return _this->out[bank_id]->req(&_this->ts_req);
  }

  req->set_addr(bank_offset);
  return _this->out[bank_id]->req_forward(req);
}

int interleaver::build()
{

  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&interleaver::req);
  new_slave_port("in", &in);

  nb_slaves = get_config_int("nb_slaves");
  nb_masters = get_config_int("nb_masters");
  stage_bits = get_config_int("stage_bits");

  bank_mask = (1<<stage_bits) - 1;

  out = new vp::io_master *[nb_slaves];
  for (int i=0; i<nb_slaves; i++)
  {
    out[i] = new vp::io_master();
    new_master_port("out_" + std::to_string(i), out[i]);
  }

  masters_in = new vp::io_slave *[nb_masters];
  masters_ts_in = new vp::io_slave *[nb_masters];
  for (int i=0; i<nb_masters; i++)
  {
    masters_in[i] = new vp::io_slave();
    masters_in[i]->set_req_meth(&interleaver::req);
    new_slave_port("in_" + std::to_string(i), masters_in[i]);

    masters_ts_in[i] = new vp::io_slave();
    masters_ts_in[i]->set_req_meth(&interleaver::req_ts);
    new_slave_port("ts_in_" + std::to_string(i), masters_ts_in[i]);
  }

  return 0;
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new interleaver(config);
}


