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


#include "udma_mram_v1.hpp"
#include "archi/utils.h"



void Mram_periph_v1::data_grant(void *__this, vp::io_req *req)
{
}



void Mram_periph_v1::data_response(void *__this, vp::io_req *req)
{
}


void Mram_periph_v1::handle_tx_pending_word(void *__this, vp::clock_event *event)
{
  Mram_periph_v1 *_this = (Mram_periph_v1 *)__this;
  vp::io_req *req = &_this->io_req;
  vp::io_req *pending_req = _this->pending_req;

  req->init();
  req->set_addr(_this->tx_channel->current_addr);
  req->set_size(4);
  req->set_is_write(true);
  req->set_data(pending_req->get_data());

  int err = _this->io_itf.req(req);

  if (err == vp::IO_REQ_OK) 
  {
  }
  else if (err == vp::IO_REQ_INVALID) 
  {
    _this->top->warning.force_warning("Invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", _this->tx_channel->current_addr, 4, true);
  }
  else
  {
    _this->top->warning.fatal("Unsupported asynchronous request in UDMA IO channel");
  }

  _this->tx_channel->current_addr += 4;
  
  _this->pending_req = NULL;

  if (_this->tx_channel->handle_ready_req_end(pending_req))
  {
    int event;
    switch(_this->r_mode.cmd_get())
    {
      case MRAM_CMD_TRIM_CFG:    event = 3; break;
      case MRAM_CMD_NORMAL_TX:   event = 3; break;
      case MRAM_CMD_ERASE_CHIP:  event = 2; break;
      case MRAM_CMD_ERASE_SECT:  event = 2; break;
      case MRAM_CMD_ERASE_WORD:  event = 2; break;
      case MRAM_CMD_PWDN:        event = 3; break;
      case MRAM_CMD_READ_RX:     event = 1; break;
      case MRAM_CMD_REF_LINE_P:  event = 2; break;
      case MRAM_CMD_REF_LINE_AP: event = 2; break;
    }
    _this->top->trigger_event(_this->first_event + event);
  }

  _this->check_state();
}



void Mram_periph_v1::handle_rx_pending_word(void *__this, vp::clock_event *event)
{
  Mram_periph_v1 *_this = (Mram_periph_v1 *)__this;
  vp::io_req *req = &_this->io_req;

  req->init();
  req->set_addr(_this->rx_channel->current_addr);
  req->set_size(4);
  req->set_is_write(false);
  req->set_data((uint8_t *)&_this->rx_data);

  int err = _this->io_itf.req(req);

  if (err == vp::IO_REQ_OK) 
  {
  }
  else if (err == vp::IO_REQ_INVALID) 
  {
    _this->top->warning.force_warning("Invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", _this->rx_channel->current_addr, 4, false);
  }
  else
  {
    _this->top->warning.fatal("Unsupported asynchronous request in UDMA IO channel");
  }

  _this->rx_channel->current_addr += 4;
  
  _this->pending_rx = false;

  _this->rx_channel->push_data((uint8_t *)&_this->rx_data, 4);


  #if 0
  if (tx_channel->handle_ready_req_end(pending_req))
  {
    int event;
    switch(_this->r_mode.cmd_get())
    {
      case MRAM_CMD_TRIM_CFG:    event = 3; break;
      case MRAM_CMD_NORMAL_TX:   event = 3; break;
      case MRAM_CMD_ERASE_CHIP:  event = 2; break;
      case MRAM_CMD_ERASE_SECT:  event = 2; break;
      case MRAM_CMD_ERASE_WORD:  event = 2; break;
      case MRAM_CMD_PWDN:        event = 3; break;
      case MRAM_CMD_READ_RX:     event = 1; break;
      case MRAM_CMD_REF_LINE_P:  event = 2; break;
      case MRAM_CMD_REF_LINE_AP: event = 2; break;
    }
    _this->top->trigger_event(_this->first_event + event);
  }
  #endif

  _this->check_state();
}



void Mram_periph_v1::handle_ready_reqs()
{
  this->check_state();
}

void Mram_periph_v1::check_state()
{
  if (this->pending_req == NULL)
  {
    if (!tx_channel->ready_reqs->is_empty())
    {
      this->pending_req = this->tx_channel->ready_reqs->pop();
      this->top->event_enqueue(this->pending_tx_access_event, 1);
    }
    else if (this->rx_channel->current_cmd && !pending_rx)
    {
      this->pending_rx = true;
      this->top->event_enqueue(this->pending_rx_access_event, 1);
    }
  }
}

Mram_periph_v1::Mram_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "mram" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new Mram_v1_rx_channel(top, this, UDMA_EVENT_ID(id), itf_name + "_rx");
  channel1 = new Mram_v1_tx_channel(top, this, UDMA_EVENT_ID(id) + 1, itf_name + "_tx");

  this->first_event = UDMA_EVENT_ID(id);

  this->io_itf.set_resp_meth(&Mram_periph_v1::data_response);
  this->io_itf.set_grant_meth(&Mram_periph_v1::data_grant);
  this->top->new_master_port(this, itf_name, &this->io_itf);

  this->pending_tx_access_event = top->event_new(this, Mram_periph_v1::handle_tx_pending_word);
  this->pending_rx_access_event = top->event_new(this, Mram_periph_v1::handle_rx_pending_word);
  this->pending_req = NULL;

  this->rx_channel = static_cast<Mram_v1_rx_channel *>(this->channel0);
  this->tx_channel = static_cast<Mram_v1_tx_channel *>(this->channel1);

  this->top->new_reg(itf_name + "/tx_daddr", &this->r_tx_daddr, 0);
  this->top->new_reg(itf_name + "/rx_daddr", &this->r_rx_daddr, 0);
  this->top->new_reg(itf_name + "/status", &this->r_status, 0);
  this->top->new_reg(itf_name + "/mode", &this->r_mode, 0);
  this->top->new_reg(itf_name + "/erase_addr", &this->r_erase_addr, 0);
  this->top->new_reg(itf_name + "/erase_size", &this->r_erase_size, 0);
  this->top->new_reg(itf_name + "/clock_div", &this->r_clock_div, 0);
  this->top->new_reg(itf_name + "/trigger", &this->r_trigger, 0);
  this->top->new_reg(itf_name + "/isr", &this->r_isr, 0);
  this->top->new_reg(itf_name + "/ier", &this->r_ier, 0);
  this->top->new_reg(itf_name + "/icr", &this->r_icr, 0);
}
 

void Mram_periph_v1::reset(bool active)
{
  Udma_periph::reset(active);

  this->mram = (Mram_itf *)this->io_itf.get_itf();
  this->pending_rx = false;
}





vp::io_req_status_e Mram_periph_v1::custom_req(vp::io_req *req, uint64_t offset)
{
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  if (size != 4)
    return vp::IO_REQ_INVALID;

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  switch (offset + UDMA_CHANNEL_CUSTOM_OFFSET)
  {
    case UDMA_MRAM_TX_DADDR_OFFSET:
      this->r_tx_daddr.access(reg_offset, size, data, is_write);
      if (req->get_is_write())
      {
        this->tx_channel->set_addr((this->r_tx_daddr.get() << 3) - 0x1D000000);
      }
      break;

    case UDMA_MRAM_RX_DADDR_OFFSET:
      this->r_rx_daddr.access(reg_offset, size, data, is_write);
      if (req->get_is_write())
      {
        this->rx_channel->set_addr((this->r_rx_daddr.get() << 3) - 0x1D000000);
      }
      break;

    case UDMA_MRAM_STATUS_OFFSET:
      this->r_status.access(reg_offset, size, data, is_write);
      break;

    case UDMA_MRAM_MRAM_MODE_OFFSET:
      this->r_mode.access(reg_offset, size, data, is_write);
      if (is_write)
      {
        this->trace.msg("Setting MRAM_MODE (value: 0x%x, ecc_byps: %d, dpd: %d, aref: %d, tmen: %d, nvr: %d, rst_b: %d, ret_b: %d, port_b: %d, cmd: %d)\n",
          this->r_mode.get(), this->r_mode.ecc_byps_get(), this->r_mode.dpd_get(), this->r_mode.aref_get(), this->r_mode.tmen_get(), this->r_mode.nvr_get(), this->r_mode.rst_b_get(), this->r_mode.ret_b_get(), this->r_mode.por_b_get(), this->r_mode.cmd_get());

        this->mram->set_cmd(this->r_mode.cmd_get());
      }
      break;

    case UDMA_MRAM_ERASE_ADDR_OFFSET:
      this->r_erase_addr.access(reg_offset, size, data, is_write);
      break;

    case UDMA_MRAM_ERASE_SIZE_OFFSET:
      this->r_erase_size.access(reg_offset, size, data, is_write);
      break;

    case UDMA_MRAM_CLOCK_DIV_OFFSET:
      this->r_clock_div.access(reg_offset, size, data, is_write);
      break;

    case UDMA_MRAM_TRIGGER_OFFSET:
      this->r_trigger.access(reg_offset, size, data, is_write);
      if (is_write)
      {
        this->trace.msg("Setting TRIGGER (value: 0x%x)\n", this->r_trigger.get());
        this->top->trigger_event(this->first_event + 2);
      }
      break;

    case UDMA_MRAM_ISR_OFFSET:
      this->r_isr.access(reg_offset, size, data, is_write);
      break;

    case UDMA_MRAM_IER_OFFSET:
      this->r_ier.access(reg_offset, size, data, is_write);
      break;

    case UDMA_MRAM_ICR_OFFSET:
      this->r_icr.access(reg_offset, size, data, is_write);
      break;

    default:
      return vp::IO_REQ_INVALID;
  }


  return vp::IO_REQ_OK;
}

Mram_v1_rx_channel::Mram_v1_rx_channel(udma *top, Mram_periph_v1 *Mram_periph_v1, int id, string name) : Udma_rx_channel(top, id, name), periph(Mram_periph_v1)
{

}


Mram_v1_tx_channel::Mram_v1_tx_channel(udma *top, Mram_periph_v1 *Mram_periph_v1, int id, string name) : Udma_tx_channel(top, id, name), periph(Mram_periph_v1)
{
}



void Mram_v1_tx_channel::handle_ready_reqs()
{
  this->periph->handle_ready_reqs();
}

void Mram_v1_rx_channel::handle_ready()
{
  this->periph->handle_ready_reqs();
}