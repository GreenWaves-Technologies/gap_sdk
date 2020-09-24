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
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <math.h>

class interleaver : public vp::component
{

public:

  interleaver(js::config *config);

  int build();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);


  static void grant(void *_this, vp::io_req *req);

  static void response(void *_this, vp::io_req *req);

private:
  vp::trace     trace;

  vp::io_master **out;
  vp::io_slave **masters_in;
  vp::io_slave in;

  int nb_slaves;
  int nb_masters;
  int interleaving_bits;
  int stage_bits;
  uint64_t offset_mask;
  uint64_t remove_offset;
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

  uint8_t *init_data = data;
  uint64_t init_size = size;
  uint64_t init_offset = offset;

  _this->trace.msg("Received IO req (offset: 0x%llx, size: 0x%llx, is_write: %d)\n", offset, size, is_write);
 
  int port_size = 1<<_this->interleaving_bits;
  int align_size = offset & (port_size - 1);
  if (align_size) align_size = port_size - align_size;

  offset -= _this->remove_offset;

  while(size) {
    
    int loop_size = port_size;
    if (align_size) {
      loop_size = align_size;
      align_size = 0;
    }
    if (loop_size > size) loop_size = size;

    int output_id = (offset >> _this->interleaving_bits) & ((1 << _this->stage_bits) - 1);
    uint64_t new_offset = ((offset & _this->offset_mask) >> _this->stage_bits) + (offset & ((1<<_this->interleaving_bits)-1));

    _this->trace.msg("Forwarding interleaved packet (port: %d, offset: 0x%x, size: 0x%x)\n", output_id, new_offset, loop_size);

    if (!_this->out[output_id]) return vp::IO_REQ_INVALID;

    req->set_addr(new_offset);
    req->set_size(loop_size);
    req->set_data(data);

    if (_this->out[output_id]->req_forward(req)) return vp::IO_REQ_INVALID;
    
    size -= loop_size;
    offset += loop_size;
    if (data)
      data += loop_size;
  }

  req->set_addr(init_offset);
  req->set_size(init_size);
  req->set_data(init_data);


  return vp::IO_REQ_OK;
}

void interleaver::grant(void *_this, vp::io_req *req)
{

}

void interleaver::response(void *_this, vp::io_req *req)
{
}

int interleaver::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&interleaver::req);
  new_slave_port("input", &in);

  nb_slaves = get_config_int("nb_slaves");
  nb_masters = get_config_int("nb_masters");
  stage_bits = get_config_int("stage_bits");
  interleaving_bits = get_config_int("interleaving_bits");
  remove_offset = get_config_int("remove_offset");

  if (stage_bits == 0)
  {
    stage_bits = log2(nb_slaves);
  }

  offset_mask = -1;
  offset_mask &= ~((1 << (interleaving_bits + stage_bits)) - 1);

  out = new vp::io_master *[nb_slaves];
  for (int i=0; i<nb_slaves; i++)
  {
    out[i] = new vp::io_master();
    out[i]->set_resp_meth(&interleaver::response);
    out[i]->set_grant_meth(&interleaver::grant);
    new_master_port("out_" + std::to_string(i), out[i]);
  }

  masters_in = new vp::io_slave *[nb_masters];
  for (int i=0; i<nb_masters; i++)
  {
    masters_in[i] = new vp::io_slave();
    masters_in[i]->set_req_meth(&interleaver::req);
    new_slave_port("in_" + std::to_string(i), masters_in[i]);
  }
  return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new interleaver(config);
}


