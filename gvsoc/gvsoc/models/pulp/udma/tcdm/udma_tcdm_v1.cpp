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


#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/io.hpp"


Tcdm_periph_v1::Tcdm_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "tcdm" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new Tcdm_channel(top, this, UDMA_EVENT_ID(id), itf_name + "_from_tcdm");
  channel1 = new Tcdm_channel(top, this, UDMA_EVENT_ID(id) + 1, itf_name + "_to_tcdm");

  top->new_master_port(this, itf_name, &io_itf);

  this->top->new_reg(itf_name + "/dst_addr", &this->r_dst_addr, 0);
  this->top->new_reg(itf_name + "/src_addr", &this->r_src_addr, 0);
  this->top->new_reg(itf_name + "/mem_sel", &this->r_mem_sel, 0);

  this->out_reqs = new Udma_queue<vp::io_req>(4);
  for (int i=0; i<4; i++)
  {
    vp::io_req *req = new vp::io_req();
    req->set_is_write(true);
    req->set_size(4);
    req->arg_alloc(); // Used to store read request;
    this->out_reqs->push(req);
  }

  this->out_waiting_reqs = new Udma_queue<vp::io_req>(4);

  this->pending_reqs_event = top->event_new(this, Tcdm_periph_v1::handle_reqs);
}
 

void Tcdm_periph_v1::reset(bool active)
{
  Udma_periph::reset(active);

  if (active)
  {
  }
}



vp::io_req_status_e Tcdm_periph_v1::custom_req(vp::io_req *req, uint64_t offset)
{
  uint8_t *data = req->get_data();
  unsigned int size = req->get_size();
  bool is_write = req->get_is_write();

  if (req->get_size() != 4)
    return vp::IO_REQ_INVALID;

  offset += UDMA_CHANNEL_CUSTOM_OFFSET;

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  switch (reg_id) {
    case UDMA_MEMCPY_DST_ADDR_OFFSET/4:
      this->r_dst_addr.access(reg_offset, size, data, is_write);
      return vp::IO_REQ_OK;

    case UDMA_MEMCPY_SRC_ADDR_OFFSET/4:
      this->r_src_addr.access(reg_offset, size, data, is_write);
      return vp::IO_REQ_OK;

    case UDMA_MEMCPY_MEM_SEL_OFFSET/4:
      this->r_mem_sel.access(reg_offset, size, data, is_write);
      return vp::IO_REQ_OK;
  }

  return vp::IO_REQ_INVALID;
}





Tcdm_channel::Tcdm_channel(udma *top, Tcdm_periph_v1 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  //pending_word_event = top->event_new(this, Tcdm_channel::handle_pending_word);
}


#if 0
void Tcdm_channel::check_state()
{

}
#endif


void Tcdm_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
  printf("%s %d\n", __FILE__, __LINE__);
  #if 0
  if (end)
  {
    _this->handle_ready_req_end(_this->pending_req);
    _this->handle_ready_reqs();
  }

  _this->check_state();
#endif
}


void Tcdm_periph_v1::handle_reqs(void *__this, vp::clock_event *event)
{
  Tcdm_periph_v1 *_this = (Tcdm_periph_v1 *)__this;

  if (!_this->out_reqs->is_empty())
  {
    Udma_channel *channel = NULL;
    uint32_t addr;

    if (!_this->channel0->ready_reqs->is_empty())
    {
      channel = _this->channel0;
      addr = _this->r_src_addr.get();
      _this->r_src_addr.set(_this->r_src_addr.get() + 4);
    }
    else if (!_this->channel1->ready_reqs->is_empty())
    {
      channel = _this->channel1;
      addr = _this->r_dst_addr.get();
      _this->r_dst_addr.set(_this->r_dst_addr.get() + 4);
    }

    if (channel)
    {
      vp::io_req *in_req = channel->ready_reqs->pop();
      vp::io_req *out_req = _this->out_reqs->pop();

      out_req->init();
      out_req->set_data(in_req->get_data());
      out_req->set_addr(addr);

      channel->handle_ready_req_end(in_req);

      vp::io_req_status_e err = _this->top->l2_itf.req(out_req);
      if (err != vp::IO_REQ_OK)
      {
        if (err == vp::IO_REQ_INVALID)
          _this->top->warning.force_warning("Invalid access during UDMA transfer\n");
        else
          _this->top->warning.force_warning("Unsupported pending access\n");
      }

      out_req->set_latency(out_req->get_latency() + _this->top->get_cycles() + 1);
      _this->out_waiting_reqs->push_from_latency(out_req);
    }
  }

  vp::io_req *req = _this->out_waiting_reqs->get_first();
  while (req != NULL && req->get_latency() <= _this->top->get_cycles())
  {
    _this->trace.msg("Read request is ready, pushing to channel (req: %p)\n", req);

    _this->out_waiting_reqs->pop();
    _this->out_reqs->push(req);

    req = _this->out_waiting_reqs->get_first();
  }

  _this->check_state();
}

void Tcdm_periph_v1::check_state()
{
  if (!this->pending_reqs_event->is_enqueued() && !this->out_reqs->is_empty() && (!this->channel1->ready_reqs->is_empty() || !this->channel0->ready_reqs->is_empty()))
  {
    this->top->event_reenqueue(this->pending_reqs_event, 1);
  }

  if (!this->out_waiting_reqs->is_empty())
  {
    this->top->event_reenqueue_ext(this->pending_reqs_event, this->out_waiting_reqs->get_first()->get_latency() - this->top->get_cycles());
  }
}



void Tcdm_channel::handle_ready_reqs()
{
  this->periph->check_state();
}



void Tcdm_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);

  if (active)
  {
  }
}
