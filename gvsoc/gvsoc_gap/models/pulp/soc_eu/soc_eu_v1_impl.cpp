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
#include <archi/soc_eu/soc_eu_v1.h>

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
  vp::wire_master<int>    fc_event_itf;
  vp::wire_master<int>    cl_event_itf;
  vp::wire_master<int>    pr_event_itf;

  int nb_fc_events;
  int first_fc_event;
  int ref_clock_event;

  vp::trace     trace;
  vp::io_slave in;

  vp::wire_slave<int>      event_in_itf;
  vp::clock_slave          ref_clock;
  vp::wire_master<bool>     ref_clock_event_itf;

  unsigned int fc_event_mask[2];
  unsigned int cl_event_mask[2];
  unsigned int pr_event_mask[2];
};

soc_eu::soc_eu(js::config *config)
: vp::component(config)
{

}

void soc_eu::trigger_event(int event)
{
  int word_id = event >> 5;
  int bit_id = event & 0x1f;
  if (((fc_event_mask[word_id] >> bit_id) & 1) == 0)
  {
    trace.msg("Generating FC event (event: %d)\n", event);
    if (!fc_event_itf.is_bound()) trace.msg("Trying to send event to FC while it is not instantiated\n");
    else fc_event_itf.sync(event);
  }
  if (((cl_event_mask[word_id] >> bit_id) & 1) == 0)
  {
    trace.msg("Generating cluster event (event: %d)\n", event);
    if (!cl_event_itf.is_bound()) trace.msg("Trying to send event to cluster while it is not instantiated\n");
    else cl_event_itf.sync(event);
  }
  if (((pr_event_mask[word_id] >> bit_id) & 1) == 0)
    {
    trace.msg("Generating periph event (event: %d)\n", event);
    if (!pr_event_itf.is_bound()) trace.msg("Trying to send event to periph while it is not instantiated\n");
    else pr_event_itf.sync(event);
  }
}

vp::io_req_status_e soc_eu::req(void *__this, vp::io_req *req)
{
  soc_eu *_this = (soc_eu *)__this;

  uint64_t offset = req->get_addr();
  uint32_t *data = (uint32_t *)req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  if (offset == SOC_EU_EVENT)
  {
    if (!is_write) return vp::IO_REQ_INVALID;
    for (unsigned int i=0; i<_this->nb_fc_events; i++) {
      if ((*(data) >> i) & 1) {
        _this->trigger_event(i + _this->first_fc_event);
      }
    }
    return vp::IO_REQ_OK;
  }
  else if (offset == SOC_FC_MASK_MSB)
  {
    if (!is_write)
      *data = _this->fc_event_mask[1];
    else {
      _this->fc_event_mask[1] = *data;
      _this->trace.msg("Setting FC MSB soc event mask (newMask: 0x%x)\n", _this->fc_event_mask[1]);
    }
    return vp::IO_REQ_OK;
  }
  else if (offset == SOC_FC_MASK_LSB)
  {
    if (!is_write)
      *data = _this->fc_event_mask[0];
    else {
      _this->fc_event_mask[0] = *data;
      _this->trace.msg("Setting FC LSB soc event mask (newMask: 0x%x)\n", _this->fc_event_mask[0]);
    }
    return vp::IO_REQ_OK;
  }
  else if (offset == SOC_CL_MASK_MSB)
  {
    if (!is_write)
      *data = _this->cl_event_mask[1];
    else {
      _this->cl_event_mask[1] = *data;
      _this->trace.msg("Setting cluster MSB soc event mask (newMask: 0x%x)\n", _this->cl_event_mask[1]);
    }
    return vp::IO_REQ_OK;
  }
  else if (offset == SOC_CL_MASK_LSB)
  {
    if (!is_write)
      *data = _this->cl_event_mask[0];
    else {
      _this->cl_event_mask[0] = *data;
      _this->trace.msg("Setting cluster LSB soc event mask (newMask: 0x%x)\n", _this->cl_event_mask[0]);
    }
    return vp::IO_REQ_OK;
  }
  else if (offset == SOC_PR_MASK_MSB)
  {
    if (!is_write)
      *data = _this->pr_event_mask[1];
    else {
      _this->pr_event_mask[1] = *data;
      _this->trace.msg("Setting periph MSB soc event mask (newMask: 0x%x)\n", _this->pr_event_mask[1]);
    }
    return vp::IO_REQ_OK;
  }
  else if (offset == SOC_PR_MASK_LSB)
  {
    if (!is_write)
      *data = _this->pr_event_mask[0];
    else {
      _this->pr_event_mask[0] = *data;
      _this->trace.msg("Setting periph LSB soc event mask (newMask: 0x%x)\n", _this->pr_event_mask[0]);
    }
    return vp::IO_REQ_OK;
  }
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
  new_master_port("fc_event_itf", &fc_event_itf);
  new_master_port("pr_event_itf", &pr_event_itf);
  new_master_port("cl_event_itf", &cl_event_itf);

  return 0;
}

void soc_eu::reset(bool active)
{
  if (active)
  {
    for (int i=0; i<2; i++) {
      fc_event_mask[i] = 0xffffffff;
      cl_event_mask[i] = 0xffffffff;
      pr_event_mask[i] = 0xffffffff;
    }
  }
}

void soc_eu::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new soc_eu(config);
}
