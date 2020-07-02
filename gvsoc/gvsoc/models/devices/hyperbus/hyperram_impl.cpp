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
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vp/itf/hyper.hpp"
#include "vp/itf/wire.hpp"
#include "archi/utils.h"

#define REGS_AREA_SIZE 1024



typedef enum
{
  HYPERBUS_STATE_CA,
  HYPERBUS_STATE_DATA
} Hyperbus_state_e;



class Hyperram : public vp::component
{
public:
  Hyperram(js::config *config);

  void handle_access(int reg_access, int address, int read, uint8_t data);

  int build();

  static void sync_cycle(void *_this, int data);
  static void cs_sync(void *__this, bool value);

protected:
  vp::trace     trace;
  vp::hyper_slave   in_itf;
  vp::wire_slave<bool> cs_itf;

private:
  int size;
  uint8_t *data;
  uint8_t *reg_data;

  union
  {
    struct {
      unsigned int low_addr:3;
      unsigned int reserved:13;
      unsigned int high_addr:29;
      unsigned int burst_type:1;
      unsigned int address_space:1;
      unsigned int read:1;
    } __attribute__((packed));;
    uint8_t raw[6];
  } ca;

  int ca_count;
  int current_address;
  int reg_access;

  Hyperbus_state_e state;
};





void Hyperram::handle_access(int reg_access, int address, int read, uint8_t data)
{
  if (address >= this->size)
  {
    this->warning.force_warning("Received out-of-bound request (addr: 0x%x, ram_size: 0x%x)\n", address, this->size);
  }
  else
  {
    if (read)
    {
      uint8_t data = this->data[address];
      this->trace.msg(vp::trace::LEVEL_TRACE, "Sending data byte (value: 0x%x)\n", data);
      this->in_itf.sync_cycle(data);

    }
    else
    {
      this->trace.msg(vp::trace::LEVEL_TRACE, "Received data byte (value: 0x%x)\n", data);
      this->data[address] = data;
    }
  }
}



Hyperram::Hyperram(js::config *config)
: vp::component(config)
{
}



void Hyperram::sync_cycle(void *__this, int data)
{
  Hyperram *_this = (Hyperram *)__this;

  if (_this->state == HYPERBUS_STATE_CA)
  {
    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received command byte (value: 0x%x)\n", data);

    _this->ca_count--;
    _this->ca.raw[_this->ca_count] = data;
    if (_this->ca_count == 0)
    {
      _this->state = HYPERBUS_STATE_DATA;
      _this->current_address = _this->ca.low_addr | (_this->ca.high_addr << 3);

      _this->reg_access = _this->ca.address_space == 1;

      _this->trace.msg(vp::trace::LEVEL_TRACE, "Received command header (reg_access: %d, addr: 0x%x, read: %d)\n", _this->ca.address_space, _this->current_address, _this->ca.read);
    }
  }
  else if (_this->state == HYPERBUS_STATE_DATA)
  {
    _this->handle_access(_this->reg_access, _this->current_address, _this->ca.read, data);
    _this->current_address++;
  }
}

void Hyperram::cs_sync(void *__this, bool value)
{
  Hyperram *_this = (Hyperram *)__this;
  _this->trace.msg(vp::trace::LEVEL_TRACE, "Received CS sync (value: %d)\n", value);

  _this->state = HYPERBUS_STATE_CA;
  _this->ca_count = 6;
}

int Hyperram::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in_itf.set_sync_cycle_meth(&Hyperram::sync_cycle);
  new_slave_port("input", &in_itf);

  cs_itf.set_sync_meth(&Hyperram::cs_sync);
  new_slave_port("cs", &cs_itf);

  js::config *conf = this->get_js_config();

  this->size = conf->get("size")->get_int();

  this->data = new uint8_t[this->size];
  memset(this->data, 0xff, this->size);

  this->reg_data = new uint8_t[REGS_AREA_SIZE];
  memset(this->reg_data, 0x57, REGS_AREA_SIZE);
  ((uint16_t *)this->reg_data)[0] = 0x8F1F;
  
  return 0;
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Hyperram(config);
}
