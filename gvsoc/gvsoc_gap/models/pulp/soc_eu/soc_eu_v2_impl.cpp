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
#include <vp/itf/wire.hpp>
#include <vp/itf/clock.hpp>
#include <string.h>
#include <archi/soc_eu/soc_eu_v2.h>

class soc_eu;

class soc_eu_target
{
public:

  soc_eu_target(soc_eu *top, std::string name, std::string itf_name);

  vp::wire_master<int>    event_itf;
  unsigned int event_mask[SOC_NB_EVENT_REGS];
  std::string name;
};

class soc_eu : public vp::component
{

public:

  soc_eu(js::config *config);

  int build();
  void start();
  void reset(bool active);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  static void event_in_sync(void *__this, int event);
  static void ref_clock_sync(void *_this, bool value);
  void trigger_event(int event);

  vp::io_req_status_e trigger_event_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e mask_req(int target_id, int reg_id, int reg_offset, int size, bool is_write, uint8_t *data);

  int nb_fc_events;
  int first_fc_event;
  int ref_clock_event;

  vp::trace     trace;
  vp::io_slave in;

  vp::wire_slave<int>      event_in_itf;
  vp::clock_slave          ref_clock;
  vp::wire_master<bool>     ref_clock_event_itf;

  vector<soc_eu_target *> targets;
};

soc_eu::soc_eu(js::config *config)
: vp::component(config)
{

}

void soc_eu::trigger_event(int event)
{
  int word_id = event >> 5;
  int bit_id = event & 0x1f;

  for (auto target: this->targets)
  {
    if (((target->event_mask[word_id] >> bit_id) & 1) == 0)
    {
      trace.msg("Generating %s event (event: %d)\n", target->name.c_str(), event);
      if (!target->event_itf.is_bound())
        trace.msg("Trying to send event to %s while it is not instantiated\n", target->name.c_str());
      else
        target->event_itf.sync(event);
    }
  }
}

vp::io_req_status_e soc_eu::trigger_event_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (!is_write) return vp::IO_REQ_INVALID;
  for (unsigned int i=0; i<this->nb_fc_events; i++) {
    if ((*(data) >> i) & 1) {
      this->trigger_event(i + this->first_fc_event);
    }
  }
  return vp::IO_REQ_OK;
}

vp::io_req_status_e soc_eu::mask_req(int target_id, int reg_id, int reg_offset, int size, bool is_write, uint8_t *data)
{
  soc_eu_target *target = this->targets[target_id];

  if (!is_write)
  {
    memcpy(data, &(((uint8_t *)&target->event_mask[reg_id])[reg_offset]), size);
  }
  else {
    memcpy(&(((uint8_t *)&target->event_mask[reg_id])[reg_offset]), data, size);
    this->trace.msg("Setting soc event mask (target: %s, mask_id: %d, value: 0x%x)\n", target->name.c_str(), reg_id, target->event_mask[reg_id]);
  }
  return vp::IO_REQ_OK;
}

vp::io_req_status_e soc_eu::req(void *__this, vp::io_req *req)
{
  soc_eu *_this = (soc_eu *)__this;

  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Soc event unit access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  if (reg_offset + size > 4) {
    _this->get_trace()->warning("Accessing 2 registers in one access\n");
    goto error;
  }

  switch (reg_id) {
    case SOC_EU_EVENT/4:     err = _this->trigger_event_req(reg_offset, size, is_write, data); break;
    default: {
      int target_id = (reg_id - SOC_FC_FIRST_MASK/4) / SOC_NB_EVENT_REGS;
      int target_reg_id = (reg_id - SOC_FC_FIRST_MASK/4) % SOC_NB_EVENT_REGS;

      if (target_id < SOC_NB_EVENT_TARGETS)
        err = _this->mask_req(target_id, target_reg_id, reg_offset, size, is_write, data);

      break;
    }
  }

  if (err != vp::IO_REQ_OK)
    goto error;

  return vp::IO_REQ_OK;

error:
  _this->get_trace()->force_warning("Soc event unit invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}

void soc_eu::event_in_sync(void *__this, int event)
{
  soc_eu *_this = (soc_eu *)__this;
  _this->trace.msg("Received incoming event (event: %d)\n", event);
  _this->trigger_event(event);
}

void soc_eu::ref_clock_sync(void *__this, bool value)
{
  soc_eu *_this = (soc_eu *)__this;
  _this->trace.msg("Received ref clock event, generating event (event: %d)\n", _this->ref_clock_event);
  _this->ref_clock_event_itf.sync(true);
  _this->trigger_event(_this->ref_clock_event);
}

int soc_eu::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&soc_eu::req);
  new_slave_port("input", &in);

  this->ref_clock_event = this->get_config_int("ref_clock_event");
  this->ref_clock.set_sync_meth(&soc_eu::ref_clock_sync);
  this->new_slave_port("ref_clock", &this->ref_clock);

  event_in_itf.set_sync_meth(&soc_eu::event_in_sync);
  new_slave_port("event_in", &event_in_itf);

  nb_fc_events = get_config_int("properties/nb_fc_events");
  first_fc_event = get_config_int("properties/first_fc_event");

  new_master_port("ref_clock_event", &ref_clock_event_itf);

  this->targets.push_back(new soc_eu_target(this, "FC", "fc_event_itf"));
  this->targets.push_back(new soc_eu_target(this, "PR", "pr_event_itf"));
  this->targets.push_back(new soc_eu_target(this, "CL", "cl_event_itf"));

  return 0;
}

void soc_eu::reset(bool active)
{
  if (active)
  {
    for (auto target: this->targets)
    {
      for (int i=0; i<SOC_NB_EVENT_REGS; i++)
      {
        target->event_mask[i] = 0xffffffff;
      }
    }
  }
}

void soc_eu::start()
{
}

soc_eu_target::soc_eu_target(soc_eu *top, std::string name, std::string itf_name)
: name(name)
{
  top->new_master_port(itf_name, &this->event_itf);
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new soc_eu(config);
}
