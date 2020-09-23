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
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>
#include "archi/itc/itc_v1.h"

class itc : public vp::component
{

public:

  itc(js::config *config);

  int build();
  void reset(bool active);


private:

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

  vp::io_req_status_e itc_mask_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_mask_set_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_mask_clr_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_status_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_status_set_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_status_clr_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_ack_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_ack_set_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_ack_clr_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);
  vp::io_req_status_e itc_fifo_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write);

  void itc_status_setValue(uint32_t value);

  void check_state();

  static void irq_ack_sync(void *__this, int irq);
  static void in_event_sync(void *__this, bool active, int id);
  static void soc_event_sync(void *__this, int event);

  vp::trace     trace;
  vp::io_slave in;

  uint32_t ack;
  uint32_t status;
  uint32_t mask;

  int nb_free_events;
  int fifo_event_head;
  int fifo_event_tail;
  int *fifo_event;

  int nb_fifo_events;
  int fifo_irq;

  int sync_irq;

  vp::wire_master<int>    irq_req_itf;
  vp::wire_slave<int>     irq_ack_itf;
  vp::wire_slave<int>     soc_event_itf;


  vp::wire_slave<bool> in_event_itf[32];

};

itc::itc(js::config *config)
: vp::component(config)
{

}

void itc::itc_status_setValue(uint32_t value)
{
  trace.msg("Updated irq status (value: 0x%x)\n", value);
  status = value;

  check_state();
}

void itc::check_state()
{
  uint32_t status_masked = status & mask;
  int irq = status_masked ? 31 - __builtin_clz(status_masked) : -1;

  if (irq != sync_irq) {
    trace.msg("Updating irq req (irq: %d)\n", irq);
    sync_irq = irq;
    irq_req_itf.sync(irq);
  }

}

vp::io_req_status_e itc::itc_mask_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) *data = mask;
  else {
    mask = *data;
    trace.msg("Updated irq mask (value: 0x%x)\n", mask);
  }

  check_state();

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_mask_set_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) return vp::IO_REQ_INVALID;

  mask |= *data;
  trace.msg("Updated irq mask (value: 0x%x)\n", mask);

  check_state();
  
  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_mask_clr_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) return vp::IO_REQ_INVALID;

  mask &= ~(*data);
  trace.msg("Updated irq mask (value: 0x%x)\n", mask);

  check_state();
  
  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_status_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) *data = status;
  else {
    itc_status_setValue(*data);
  }

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_status_set_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) return vp::IO_REQ_INVALID;

  itc_status_setValue(status | *data);

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_status_clr_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) return vp::IO_REQ_INVALID;

  itc_status_setValue(status & ~(*data));

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_ack_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) *data = ack;
  else {
    ack = *data;
    trace.msg("Updated irq ack (value: 0x%x)\n", ack);
  }

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_ack_set_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) return vp::IO_REQ_INVALID;

  ack |= *data;
  trace.msg("Updated irq ack (value: 0x%x)\n", ack);

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_ack_clr_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (!is_write) return vp::IO_REQ_INVALID;

  ack &= ~(*data);
  trace.msg("Updated irq ack (value: 0x%x)\n", ack);

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::itc_fifo_ioReq(uint32_t offset, uint32_t *data, uint32_t size, bool is_write)
{
  if (is_write) return vp::IO_REQ_INVALID;

  if (nb_free_events == nb_fifo_events) {
    trace.warning("Reading FIFO with no event\n");
    *data = 0;
  } else {
    trace.msg("Popping event from FIFO (id: %d)\n", fifo_event[fifo_event_tail]);
    *data = fifo_event[fifo_event_tail];
    fifo_event_tail++;
    if (fifo_event_tail == nb_fifo_events) fifo_event_tail = 0;
    nb_free_events++;
    if (nb_free_events != nb_fifo_events) {
      itc_status_setValue(status | (1<<fifo_irq));
    }
  }

  return vp::IO_REQ_OK;
}

vp::io_req_status_e itc::req(void *__this, vp::io_req *req)
{
  itc *_this = (itc *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Itc access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  switch (offset) {
    case ITC_MASK_OFFSET:       return _this->itc_mask_ioReq       (offset, (uint32_t *)data, size, is_write);
    case ITC_MASK_SET_OFFSET:   return _this->itc_mask_set_ioReq   (offset, (uint32_t *)data, size, is_write);
    case ITC_MASK_CLR_OFFSET:   return _this->itc_mask_clr_ioReq   (offset, (uint32_t *)data, size, is_write);
    case ITC_STATUS_OFFSET:     return _this->itc_status_ioReq     (offset, (uint32_t *)data, size, is_write);
    case ITC_STATUS_SET_OFFSET: return _this->itc_status_set_ioReq (offset, (uint32_t *)data, size, is_write);
    case ITC_STATUS_CLR_OFFSET: return _this->itc_status_clr_ioReq (offset, (uint32_t *)data, size, is_write);
    case ITC_ACK_OFFSET:        return _this->itc_ack_ioReq        (offset, (uint32_t *)data, size, is_write);
    case ITC_ACK_SET_OFFSET:    return _this->itc_ack_set_ioReq    (offset, (uint32_t *)data, size, is_write);
    case ITC_ACK_CLR_OFFSET:    return _this->itc_ack_clr_ioReq    (offset, (uint32_t *)data, size, is_write);
    case ITC_FIFO_OFFSET:       return _this->itc_fifo_ioReq       (offset, (uint32_t *)data, size, is_write);
  }

  return vp::IO_REQ_OK;
}

void itc::soc_event_sync(void *__this, int event)
{
  itc *_this = (itc *)__this;
  _this->trace.msg("Received soc event (event: %d, fifo elems: %d)\n", event, _this->nb_fifo_events - _this->nb_free_events);

  if (_this->nb_free_events == 0) {
    _this->trace.msg("FIFO is full, dropping event\n");
    return;
  }

  _this->nb_free_events--;
  _this->fifo_event[_this->fifo_event_head] = event;
  _this->fifo_event_head++;
  if (_this->fifo_event_head == _this->nb_fifo_events) _this->fifo_event_head = 0;

  if (_this->fifo_irq != -1 && _this->nb_free_events == _this->nb_fifo_events - 1) {
    _this->trace.msg("Generating FIFO irq (id: %d)\n", _this->fifo_irq);
    _this->itc_status_setValue(_this->status | (1<<_this->fifo_irq));
  }
}

void itc::irq_ack_sync(void *__this, int irq)
{
  itc *_this = (itc *)__this;

  _this->trace.msg("Received IRQ acknowledgement (irq: %d)\n", irq);

  _this->itc_status_setValue(_this->status & ~(1<<irq));
  _this->ack |= 1<<irq;
  _this->sync_irq = -1;

  _this->trace.msg("Updated irq ack (value: 0x%x)\n", _this->ack);
}

void itc::in_event_sync(void *__this, bool active, int id)
{
  itc *_this = (itc *)__this;
  _this->trace.msg("Received input event (event: %d, active: %d)\n", id, active);
  _this->itc_status_setValue(_this->status | (1<<id));
  _this->check_state();
}

int itc::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&itc::req);
  new_slave_port("input", &in);

  new_master_port("irq_req", &irq_req_itf);

  soc_event_itf.set_sync_meth(&itc::soc_event_sync);
  new_slave_port("soc_event", &soc_event_itf);

  irq_ack_itf.set_sync_meth(&itc::irq_ack_sync);
  new_slave_port("irq_ack", &irq_ack_itf);

  nb_fifo_events = get_config_int("**/nb_fifo_events");
  fifo_irq = get_config_int("**/fifo_irq");

  fifo_event = new int[nb_fifo_events];

  for (int i=0; i<32; i++)
  {
    in_event_itf[i].set_sync_meth_muxed(&itc::in_event_sync, i);
    new_slave_port("in_event_" + std::to_string(i), &in_event_itf[i]);
  }

  return 0;
}


void itc::reset(bool active)
{
  if (active)
  {
    status = 0;
    mask   = 0;
    ack    = 0;

    sync_irq = -1;

    nb_free_events = nb_fifo_events;
    fifo_event_head = 0;
    fifo_event_tail = 0;
  }
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new itc(config);
}
