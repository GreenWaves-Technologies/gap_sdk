/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>

class memory : public vp::component
{

public:

  memory(js::config *config);

  int build();
  void start();
  void reset(bool active);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  static void power_callback(void *__this, vp::clock_event *event);

  vp::trace     trace;
  vp::io_slave in;

  uint64_t size = 0;
  bool check = false;
  int width_bits = 0;

  uint8_t *mem_data;
  uint8_t *check_mem;

  int64_t next_packet_start;

  bool power_trigger; 

  vp::power_trace power_trace;
  vp::power_source idle_power;
  vp::power_source read_8_power;
  vp::power_source read_16_power;
  vp::power_source read_32_power;
  vp::power_source write_8_power;
  vp::power_source write_16_power;
  vp::power_source write_32_power;
  vp::power_source leakage_power;

  vp::clock_event *power_event;
  int64_t last_access_timestamp;
};

memory::memory(js::config *config)
: vp::component(config)
{

}

void memory::power_callback(void *__this, vp::clock_event *event)
{
  memory *_this = (memory *)__this;
  if (_this->last_access_timestamp < _this->get_time())
  {
    _this->idle_power.power_on();
  }
}

vp::io_req_status_e memory::req(void *__this, vp::io_req *req)
{
  memory *_this = (memory *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  _this->trace.msg("Memory access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  // Impact the memory bandwith on the packet
  if (_this->width_bits != 0) {
#define MAX(a,b) (((a)>(b))?(a):(b))
    int duration = MAX(size >> _this->width_bits, 1);
    req->set_duration(duration);
    int64_t cycles = _this->get_cycles();
    int64_t diff = _this->next_packet_start - cycles;
    if (diff > 0) {
      _this->trace.msg("Delayed packet (latency: %ld)\n", diff);
      req->inc_latency(diff);
    }
    _this->next_packet_start = MAX(_this->next_packet_start, cycles) + duration;
  }

  if (offset + size > _this->size) {
    _this->trace.force_warning("Received out-of-bound request (reqAddr: 0x%x, reqSize: 0x%x, memSize: 0x%x)\n", offset, size, _this->size);
    return vp::IO_REQ_INVALID;
  }

  if (_this->power_trace.get_active())
  {
    _this->last_access_timestamp = _this->get_time();

    if (req->get_is_write())
    {
      if (size == 1)
        _this->write_8_power.account_event();
      else if (size == 2)
        _this->write_16_power.account_event();
      else if (size == 4)
        _this->write_32_power.account_event();
    }
    else
    {
      if (size == 1)
        _this->read_8_power.account_event();
      else if (size == 2)
        _this->read_16_power.account_event();
      else if (size == 4)
        _this->read_32_power.account_event();
    }

    if (!_this->power_event->is_enqueued())
      _this->event_enqueue(_this->power_event, 1);
  }

#ifdef VP_TRACE_ACTIVE
  if (_this->power_trigger)
  {
    if (req->get_is_write() && size == 4)
    {
      if (*(uint32_t *)data == 0xabbaabba)
      {
        _this->power.get_engine()->start_capture();
      }
      else if (*(uint32_t *)data == 0xdeadcaca)
      {
        _this->power.get_engine()->stop_capture();
      }
    }
  }
#endif


  if (req->get_is_write()) {
    if (_this->check_mem) {
      for (unsigned int i=0; i<size; i++) {
        _this->check_mem[(offset + i) / 8] |= 1 << ((offset + i) % 8);
      }
    }
    if (data)
      memcpy((void *)&_this->mem_data[offset], (void *)data, size);
  } else {
    if (_this->check_mem) {
      for (unsigned int i=0; i<size; i++) {
        int access = (_this->check_mem[(offset + i) / 8] >> ((offset + i) % 8)) & 1;
        if (!access) {
          //trace.msg("Unitialized access (offset: 0x%x, size: 0x%x, isRead: %d)\n", offset, size, isRead);
          return vp::IO_REQ_INVALID;
        }
      }
    }
    if (data)
      memcpy((void *)data, (void *)&_this->mem_data[offset], size);
  }

  return vp::IO_REQ_OK;
}

void memory::reset(bool active)
{
  if (active)
  {
    this->next_packet_start = 0;
  }
}

int memory::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&memory::req);
  new_slave_port("input", &in);

  js::config *config = get_js_config()->get("power_trigger");
  this->power_trigger = config != NULL && config->get_bool();

  if (power.new_trace("power_trace", &power_trace)) return -1;

  power.new_leakage_event("leakage", &leakage_power, this->get_js_config()->get("**/leakage"), &power_trace);
  power.new_event("idle", &idle_power, this->get_js_config()->get("**/idle"), &power_trace);
  power.new_event("read_8", &read_8_power, this->get_js_config()->get("**/read_8"), &power_trace);
  power.new_event("read_16", &read_16_power, this->get_js_config()->get("**/read_16"), &power_trace);
  power.new_event("read_32", &read_32_power, this->get_js_config()->get("**/read_32"), &power_trace);
  power.new_event("write_8", &write_8_power, this->get_js_config()->get("**/write_8"), &power_trace);
  power.new_event("write_16", &write_16_power, this->get_js_config()->get("**/write_16"), &power_trace);
  power.new_event("write_32", &write_32_power, this->get_js_config()->get("**/write_32"), &power_trace);

  power_event = this->event_new(memory::power_callback);

  return 0;
}

void memory::start()
{
  size = get_config_int("size");
  check = get_config_bool("check");
  width_bits = get_config_int("width_bits");

  trace.msg("Building memory (size: 0x%x, check: %d)\n", size, check);

  mem_data = new uint8_t[size];


  // Special option to check for uninitialized accesses
  if (check)
  {
    check_mem = new uint8_t[(size + 7)/8];
  }
  else
  {
    check_mem = NULL;
  }


  // Initialize the memory with a special value to detect uninitialized
  // variables
  memset(mem_data, 0x57, size);


  // Preload the memory
  js::config *stim_file_conf = this->get_js_config()->get("stim_file");
  if (stim_file_conf != NULL)
  {
    string path = stim_file_conf->get_str();
    trace.msg("Preloading memory with stimuli file (path: %s)\n", path.c_str());

    FILE *file = fopen(path.c_str(), "rb");
    if (file == NULL)
    {
      this->trace.fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
    if (fread(this->mem_data, 1, size, file) == 0)
    {
      this->trace.fatal("Failed to read stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
  }

  this->leakage_power.power_on();
  this->idle_power.power_on();
  this->last_access_timestamp = -1;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new memory(config);
}
