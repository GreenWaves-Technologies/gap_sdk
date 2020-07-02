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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

class job_fifo : public vp::component {

public:

  job_fifo(js::config *config);

  int build();

private:

  vp::trace trace;
  vp::wire_master<bool> irq; 
  vp::io_slave in;
  vp::io_slave fifo_in;

  unsigned int size;
  unsigned int width;

  unsigned int fifo_tail;
  unsigned int fifo_head;
  unsigned int nb_elems;

  uint8_t **fifo;

  static vp::io_req_status_e fifo_push(void *__this, vp::io_req *req);
  static vp::io_req_status_e req(void *__this, vp::io_req *req);

};

job_fifo::job_fifo(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e job_fifo::req(void *__this, vp::io_req *req)
{
  job_fifo *_this = (job_fifo *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  if (is_write)
  {
    // This should raise a signal on host side but is not modeled for now
    return vp::IO_REQ_OK;
  }

  if (_this->nb_elems == 0) {
    memset(data, 0, size);
    return vp::IO_REQ_OK;
  }

  uint64_t value = 0;
  if (_this->width <= 64) memcpy((void *)&value, _this->fifo[_this->fifo_head], _this->width/8);

  uint32_t word = 0;
  memcpy((void *)&word, _this->fifo[_this->fifo_head] + offset, 4);

  _this->trace.msg("Popping job word from FIFO (entry: %d, job: 0x%lx, word: 0x%x)\n", _this->fifo_head, value, word);

  memcpy(data, (void *)&word, size);

  if (offset == _this->width/8 - 4) {
    _this->nb_elems--;
    _this->fifo_head++;
    if (_this->fifo_head == _this->size) _this->fifo_head = 0;
    if (_this->nb_elems == 0) {
      _this->trace.msg("Empty fifo, irq sync set low\n");
      if (_this->irq.is_bound()) _this->irq.sync(0);
    }
  }

  return vp::IO_REQ_OK;
}

vp::io_req_status_e job_fifo::fifo_push(void *__this, vp::io_req *req)
{
  job_fifo *_this = (job_fifo *)__this;

  if (_this->nb_elems == _this->size) {
    _this->trace.msg("Trying to push job to FIFO while it is full\n");
    return vp::IO_REQ_INVALID;
  }

  uint64_t value = 0;
  if (req->get_size() <= 8) memcpy((void *)&value, req->get_data(), req->get_size());

  _this->trace.msg("Pushing job to FIFO (entry: %d, job: 0x%lx)\n", _this->fifo_tail, value);

  memcpy(_this->fifo[_this->fifo_tail], req->get_data(), req->get_size());

  _this->nb_elems++;
  _this->fifo_tail++;
  if (_this->fifo_tail == _this->size) _this->fifo_tail = 0;
  if (_this->nb_elems == 1) {
    _this->trace.msg("Non-empty fifo, irq sync set high\n");
    if (_this->irq.is_bound()) _this->irq.sync(1);
  }

  return vp::IO_REQ_OK;
}

int job_fifo::build()
{
  this->in.set_req_meth(&job_fifo::req);
  new_slave_port("input", &this->in);

  this->fifo_in.set_req_meth(&job_fifo::fifo_push);
  new_slave_port("fifo", &this->fifo_in);

  new_master_port("irq", &this->irq);

  this->traces.new_trace("trace", &this->trace, vp::DEBUG);

  this->fifo_head = 0;
  this->fifo_tail = 0;
  this->nb_elems = 0;

  this->size = this->get_config_int("size");
  this->width = this->get_config_int("width");

  this->fifo = new uint8_t *[this->size];

  for (unsigned int i=0; i<this->size; i++) {
    this->fifo[i] = new uint8_t[this->width/8];
  }

  return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new job_fifo(config);
}
