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
#include <math.h>

class interleaver : public vp::component
{

public:

  interleaver(js::config *config);

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

interleaver::interleaver(js::config *config)
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

  if (stage_bits == 0)
  {
    stage_bits = log2(nb_slaves);
  }

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

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new interleaver(config);
}


