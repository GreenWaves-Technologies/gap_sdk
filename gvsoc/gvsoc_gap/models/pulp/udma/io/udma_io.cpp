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
#include "../udma_v3_impl.hpp"


void Io_Periph::data_grant(void *__this, vp::io_req *req)
{
}



void Io_Periph::data_response(void *__this, vp::io_req *req)
{
}



void Io_Periph::handle_pending_word(void *__this, vp::clock_event *event)
{
  Io_Periph *_this = (Io_Periph *)__this;
  Io_tx_channel *tx_channel = static_cast<Io_tx_channel *>(_this->channel1);
  vp::io_req *req = &_this->io_req;
  vp::io_req *pending_req = _this->pending_req;

  req->init();
  req->set_addr(_this->current_addr);
  req->set_size(4);
  req->set_is_write(true);
  req->set_data(pending_req->get_data());

  int err = _this->io_itf.req(req);

  if (err == vp::IO_REQ_OK) 
  {
  }
  else if (err == vp::IO_REQ_INVALID) 
  {
    _this->top->warning.force_warning("Invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", _this->current_addr, 4, true);
  }
  else
  {
    _this->top->warning.fatal("Unsupported asynchronous request in UDMA IO channel");
  }

  _this->current_addr += 4;
  _this->pending_req = NULL;

  if (tx_channel->handle_ready_req_end(pending_req) && _this->eot_event != -1)
  {
    printf("GEN %d\n", _this->eot_event);
    _this->top->trigger_event(_this->eot_event);
  }

  _this->check_state();
}



void Io_Periph::check_state()
{
  Io_tx_channel *tx_channel = static_cast<Io_tx_channel *>(this->channel1);

  if (this->pending_req == NULL && !tx_channel->ready_reqs->is_empty())
  {
    this->pending_req = tx_channel->ready_reqs->pop();

    this->top->event_enqueue(this->pending_access_event, 1);
  }
}



void Io_Periph::handle_ready_reqs()
{
  this->check_state();
}



Io_Periph::Io_Periph(udma *top, int id, string itf_name) : Udma_periph(top, id)
{
  this->io_itf.set_resp_meth(&Io_Periph::data_response);
  this->io_itf.set_grant_meth(&Io_Periph::data_grant);
  this->top->new_master_port(this, itf_name, &this->io_itf);

  this->pending_access_event = top->event_new(this, Io_Periph::handle_pending_word);
  this->pending_req = NULL;
}


Io_rx_channel::Io_rx_channel(udma *top, Io_Periph *io_periph, int id, string name) : Udma_rx_channel(top, id, name), io_periph(io_periph)
{

}


Io_tx_channel::Io_tx_channel(udma *top, Io_Periph *io_periph, int id, string name) : Udma_tx_channel(top, id, name), io_periph(io_periph)
{
}



void Io_tx_channel::handle_ready_reqs()
{
  this->io_periph->handle_ready_reqs();
}
