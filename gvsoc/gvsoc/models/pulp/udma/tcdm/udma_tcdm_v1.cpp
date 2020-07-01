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


#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/io.hpp"


Tcdm_periph_v1::Tcdm_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "tcdm" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new Tcdm_rx_channel(top, this, UDMA_EVENT_ID(id), itf_name + "_from_tcdm");
  channel1 = new Tcdm_tx_channel(top, this, UDMA_EVENT_ID(id) + 1, itf_name + "_to_tcdm");

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
    req->set_data(new uint8_t[4]);
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
    this->read_size = -1;
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





Tcdm_tx_channel::Tcdm_tx_channel(udma *top, Tcdm_periph_v1 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  //pending_word_event = top->event_new(this, Tcdm_tx_channel::handle_pending_word);
}


Tcdm_rx_channel::Tcdm_rx_channel(udma *top, Tcdm_periph_v1 *periph, int id, string name)
: Udma_rx_channel(top, id, name), periph(periph)
{
  //pending_word_event = top->event_new(this, Tcdm_tx_channel::handle_pending_word);
}

void Tcdm_rx_channel::handle_ready()
{
  if (this->periph->r_mem_sel.get())
    return;

  if (this->current_cmd)
  {
    this->periph->read_size = this->current_cmd->size;
  }

  this->periph->check_state();
}


#if 0
void Tcdm_tx_channel::check_state()
{

}
#endif


void Tcdm_tx_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
}


void Tcdm_periph_v1::handle_reqs(void *__this, vp::clock_event *event)
{
  Tcdm_periph_v1 *_this = (Tcdm_periph_v1 *)__this;

  if (_this->read_size > 0)
  {
      vp::io_req *out_req = _this->out_reqs->pop();

      uint32_t addr = _this->r_src_addr.get();
      _this->r_src_addr.set(addr + 4);
      _this->read_size -= 4;

    _this->trace.msg("Sending read request to TCDM (addr: 0x%x)\n", addr);

      out_req->init();
      out_req->set_is_write(false);

      out_req->set_addr(addr);

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
  else if (_this->r_mem_sel.get() && !_this->channel1->ready_reqs->is_empty())
  {
    // Case where the loopback is active (L2 to L2) and there are some input requests
    // ready. Just push them to the RX channel
    vp::io_req *in_req = _this->channel1->ready_reqs->pop();
    (static_cast<Tcdm_rx_channel *>(_this->channel0))->push_data(in_req->get_data(), 4);
    _this->channel1->handle_ready_req_end(in_req);

  }
  else if (!_this->out_reqs->is_empty())
  {
    Udma_channel *channel = NULL;
    uint32_t addr;

    if (!_this->channel1->ready_reqs->is_empty())
    {
      channel = _this->channel1;
      if (_this->r_mem_sel.get())
        addr = _this->channel1->saddr;
      else
        addr = _this->r_dst_addr.get();
      _this->r_dst_addr.set(_this->r_dst_addr.get() + 4);
    }

    if (channel)
    {
      vp::io_req *in_req = channel->ready_reqs->pop();
      vp::io_req *out_req = _this->out_reqs->pop();

      out_req->init();
      *(uint32_t *)out_req->get_data() = *(uint32_t *)in_req->get_data();
      out_req->set_is_write(true);

      out_req->set_addr(addr);

      channel->handle_ready_req_end(in_req);

      if (_this->r_mem_sel.get() == 0)
      {
      vp::io_req_status_e err = _this->top->l2_itf.req(out_req);
      if (err != vp::IO_REQ_OK)
      {
        if (err == vp::IO_REQ_INVALID)
          _this->top->warning.force_warning("Invalid access during UDMA transfer\n");
        else
          _this->top->warning.force_warning("Unsupported pending access\n");
      }
      }

      out_req->set_latency(out_req->get_latency() + _this->top->get_cycles() + 1);
      _this->out_waiting_reqs->push_from_latency(out_req);
    }
  }

  vp::io_req *req = _this->out_waiting_reqs->get_first();
  while (req != NULL && req->get_latency() <= _this->top->get_cycles())
  {
    _this->trace.msg("Read request is ready, pushing to channel (req: %p)\n", req);

    vp::io_req *wait_req = _this->out_waiting_reqs->pop();

    if (!wait_req->get_is_write())
    {
      (static_cast<Tcdm_rx_channel *>(_this->channel0))->push_data(req->get_data(), 4);
    }

    _this->out_reqs->push(req);

    req = _this->out_waiting_reqs->get_first();
  }

  _this->check_state();
}

void Tcdm_periph_v1::check_state()
{
  if (!this->pending_reqs_event->is_enqueued() && !this->out_reqs->is_empty() && (!this->channel1->ready_reqs->is_empty() || this->read_size > 0))
  {
    this->top->event_reenqueue(this->pending_reqs_event, 1);
  }

  if (!this->out_waiting_reqs->is_empty())
  {
    this->top->event_reenqueue_ext(this->pending_reqs_event, this->out_waiting_reqs->get_first()->get_latency() - this->top->get_cycles());
  }
}



void Tcdm_tx_channel::handle_ready_reqs()
{
  this->periph->check_state();
}



void Tcdm_tx_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);

  if (active)
  {
  }
}
