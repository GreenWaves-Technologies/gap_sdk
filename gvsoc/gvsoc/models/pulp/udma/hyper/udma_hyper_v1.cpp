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
#include "archi/udma/hyper/udma_hyper_v1.h"
#include "archi/utils.h"
#include "vp/itf/hyper.hpp"


Hyper_periph_v1::Hyper_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "hyper" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new Hyper_rx_channel(top, this, UDMA_EVENT_ID(id), itf_name + "_rx");
  channel1 = new Hyper_tx_channel(top, this, UDMA_EVENT_ID(id) + 1, itf_name + "_tx");

  this->hyper_itf.set_sync_cycle_meth(&Hyper_periph_v1::rx_sync);
  top->new_master_port(this, itf_name, &this->hyper_itf);

  this->regs = new unsigned int[HYPER_NB_REGS];

  this->pending_word_event = top->event_new(this, Hyper_periph_v1::handle_pending_word);

  this->pending_bytes = 0;
  this->next_bit_cycle = -1;
  this->state = HYPER_STATE_IDLE;

  this->rx_channel = static_cast<Hyper_rx_channel *>(this->channel0);
  this->tx_channel = static_cast<Hyper_tx_channel *>(this->channel1);

  //hyper_itf.set_cs_sync_meth(&Hyper_periph_v1::cs_sync);
}
 

void Hyper_periph_v1::reset(bool active)
{
  Udma_periph::reset(active);

  if (active)
  {
    memset(this->regs, 0, sizeof(unsigned int)*HYPER_NB_REGS);
    this->clkdiv = 0;
    this->pending_tx = false;
    this->pending_rx = false;
    this->current_cmd = NULL;
  }
}


void Hyper_periph_v1::rx_sync(void *__this, int data)
{
  Hyper_periph_v1 *_this = (Hyper_periph_v1 *)__this;
  (static_cast<Hyper_rx_channel *>(_this->channel0))->handle_rx_data(data);
}


void Hyper_periph_v1::handle_pending_word(void *__this, vp::clock_event *event)
{
  Hyper_periph_v1 *_this = (Hyper_periph_v1 *)__this;
  uint8_t byte;
  int cs;
  int cs_value;
  bool send_byte = false;
  bool send_cs = false;
  bool end = false;
  unsigned int mbr = ARCHI_REG_FIELD_GET(_this->regs[HYPER_MEM_CFG4_CHANNEL_OFFSET/4], HYPER_MEM_CFG4_MBR1_OFFSET, 8) << 24;
  uint32_t addr = _this->regs[HYPER_EXT_ADDR_CHANNEL_OFFSET/4];

  if (_this->state == HYPER_STATE_IDLE)
  {
    if (_this->pending_bytes > 0)
    {
      if (addr >= mbr)
        addr -= mbr;

      _this->state = HYPER_STATE_DELAY;
      _this->delay = 72;
      _this->ca_count = 6;
      _this->ca.low_addr = ARCHI_REG_FIELD_GET(addr, 0, 3) & ~1;
      _this->ca.high_addr = ARCHI_REG_FIELD_GET(addr, 3, 29);
      _this->ca.burst_type = 0;
      _this->ca.address_space = ARCHI_REG_FIELD_GET(_this->regs[HYPER_MEM_CFG3_CHANNEL_OFFSET/4], HYPER_MEM_CFG3_CRT0_OFFSET, 1);
      _this->ca.read = _this->pending_rx ? 1 : 0;

      if (_this->ca.read)
        _this->transfer_size = _this->rx_channel->current_cmd->size;
      else
        _this->transfer_size = _this->tx_channel->current_cmd->size;
    }
  }
  else if (_this->state == HYPER_STATE_DELAY)
  {
    _this->delay--;
    if (_this->delay == 0)
      _this->state = HYPER_STATE_CS;
  }
  else if (_this->state == HYPER_STATE_CS)
  {
    _this->state = HYPER_STATE_CA;
    send_cs = true;
    cs = addr >= mbr;
    cs_value = 1;
  }
  else if (_this->state == HYPER_STATE_CA)
  {
    send_byte = true;
    _this->ca_count--;
    byte = _this->ca.raw[_this->ca_count];
    if (_this->ca_count == 0)
    {
      _this->state = HYPER_STATE_DATA;
    }
  }
  else if (_this->state == HYPER_STATE_DATA && _this->pending_bytes > 0)
  {
    send_byte = true;
    byte = _this->pending_word & 0xff;
    _this->pending_word >>= 8;
    _this->pending_bytes--;
    _this->transfer_size--;

    if (_this->transfer_size == 0)
    {
      _this->pending_bytes = 0;
      _this->state = HYPER_STATE_CS_OFF;
    }
    if (_this->pending_bytes == 0)
    {
      end = true;
    }
  }
  else if (_this->state == HYPER_STATE_CS_OFF)
  {
    _this->state = HYPER_STATE_IDLE;
    send_cs = true;
    cs = addr >= mbr;
    cs_value = 0;
  }

  if (send_byte || send_cs)
  {
    if (!_this->hyper_itf.is_bound())
    {
      _this->top->get_trace()->warning("Trying to send to HYPER interface while it is not connected\n");
    }
    else
    {
      _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->clkdiv;
      if (send_byte)
      {
        _this->top->get_trace()->msg("Sending byte (value: 0x%x)\n", byte);
        _this->hyper_itf.sync_cycle(byte);
      }
      else
      {
        _this->top->get_trace()->msg("Updating CS (cs: %d, value: %d)\n", cs, cs_value);
        _this->hyper_itf.cs_sync(cs, cs_value);
      }
    }
  }

  if (end)
  {
    if (!_this->ca.read)
    {
      _this->pending_tx = false;
      _this->tx_channel->handle_ready_req_end(_this->pending_req);
      _this->tx_channel->handle_ready_reqs();
    }
    else
      _this->pending_rx = false;

  }

  _this->check_state();
}

void Hyper_periph_v1::check_state()
{
  if (this->pending_bytes == 0)
  {
    if (!this->tx_channel->ready_reqs->is_empty() && (this->pending_tx || !this->pending_rx))
    {
      this->pending_tx = true;
      vp::io_req *req = this->tx_channel->ready_reqs->pop();
      this->pending_req = req;
      this->pending_word = *(uint32_t *)req->get_data();
      this->pending_bytes = req->get_size();
    }
    else if (this->rx_channel->current_cmd && (this->pending_rx || !this->pending_tx))
    {
      this->pending_rx = true;
      this->pending_bytes = rx_channel->current_cmd->size;
    }
  }

  if (this->pending_bytes != 0)
  {
    if (!this->pending_word_event->is_enqueued())
    {
      int latency = 1;
      int64_t cycles = this->top->get_clock()->get_cycles();
      if (this->next_bit_cycle > cycles)
        latency = this->next_bit_cycle - cycles;

      this->top->event_enqueue_ext(this->pending_word_event, latency);
    }
  }
}


void Hyper_periph_v1::handle_ready_reqs()
{
  this->check_state();
}



vp::io_req_status_e Hyper_periph_v1::custom_req(vp::io_req *req, uint64_t offset)
{

  if (req->get_size() != 4)
    return vp::IO_REQ_INVALID;

  int reg_id = offset / 4;
  if (reg_id >= HYPER_NB_REGS) return vp::IO_REQ_INVALID;

  if (!req->get_is_write())
  {
    *(uint32_t *)(req->get_data()) = this->regs[reg_id];
  }
  else
  { 
    this->regs[reg_id] = *(uint32_t *)(req->get_data());
    trace.msg("Setting hyper register (id: %d, value: 0x%x)\n", reg_id, this->regs[reg_id]);
  }


  return vp::IO_REQ_OK;
}


#if 0
void Hyper_periph_v1::sync_cycle(void *__this, int data)
{
  Hyper_periph_v1 *_this = (Hyper_periph_v1 *)__this;
  (static_cast<Hyper_rx_channel *>(_this->channel0))->handle_rx_bit(data);
}
#endif




Hyper_tx_channel::Hyper_tx_channel(udma *top, Hyper_periph_v1 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
}







void Hyper_tx_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);
}

void Hyper_tx_channel::handle_ready_reqs()
{
  this->periph->handle_ready_reqs();
}


Hyper_rx_channel::Hyper_rx_channel(udma *top, Hyper_periph_v1 *periph, int id, string name) : Udma_rx_channel(top, id, name), periph(periph)
{
}

void Hyper_rx_channel::reset(bool active)
{
  Udma_rx_channel::reset(active);
}

void Hyper_rx_channel::handle_rx_data(int data)
{
  this->push_data((uint8_t *)&data, 1);
}

void Hyper_rx_channel::handle_ready()
{
  this->periph->handle_ready_reqs();
}