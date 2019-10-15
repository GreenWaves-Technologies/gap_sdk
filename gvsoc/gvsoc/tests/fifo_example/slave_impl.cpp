/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <time.h>
#include <sstream>


class slave : public vp::component
{

public:

  slave(const char *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req, int id);

  static void grant_request(void *_this, vp::clock_event *event);

  static void send_resp(void *_this, vp::clock_event *event);

private:

  vp::io_slave *in;
  vp::trace     trace;

  int64_t next_cycles = 0;
  int current_port     = 0;

  int nb_ports        = 1;
  int bandwidth       = 4;
  int mode            = 0;
  bool switch_mode    = false;
  uint64_t latency    = 0;
  uint64_t resp_latency = 1;

  vp::clock_event *grant_event;

  vp::io_req *first_stalled = NULL;
};

void slave::send_resp(void *__this, vp::clock_event *event)
{
  slave *_this = (slave *)__this;
  
  _this->trace.msg("Sending response\n");


  vp::io_req *req = (vp::io_req *)event->get_args()[0];
  req->get_resp_port()->resp(req);
}

void slave::grant_request(void *__this, vp::clock_event *event)
{
  slave *_this = (slave *)__this;

  vp::io_req *req = _this->first_stalled, *prev = NULL;
  while (req && req->get_int(0) < _this->current_port)
  {
    prev = req;
    req = req->get_next();
  }

  if (req == NULL)
  {
    req = _this->first_stalled;
    prev = NULL;
  }

  if (prev) prev->set_next(req->get_next());
  else _this->first_stalled = req->get_next();

  _this->trace.msg("Granting stalled request (port: %d)\n", req->get_int(0));

  _this->current_port++;
  if (_this->current_port == _this->nb_ports) _this->current_port = 0;

  _this->next_cycles += (req->get_size() + _this->bandwidth - 1) / _this->bandwidth;

  req->get_resp_port()->grant(req);

  if (_this->resp_latency)
  {
    _this->event_enqueue(_this->event_new(slave::send_resp, req), _this->resp_latency);
  }
  else
  {
    req->get_resp_port()->resp(req);
  }
}

vp::io_req_status_e slave::req(void *__this, vp::io_req *req, int id)
{
  slave *_this = (slave *)__this;

  _this->trace.msg("Slave received request (port: %d)\n", id);

  int64_t cycles = _this->get_cycles();

  int64_t duration = (req->get_size() + _this->bandwidth - 1) / _this->bandwidth;
  req->set_duration(duration);
  req->set_latency(_this->latency);

  if (cycles < _this->next_cycles)
  {
    _this->trace.msg("Rejected (next cycles: %ld)\n", _this->next_cycles);
    if (!_this->grant_event->is_enqueued())
    {
      _this->event_enqueue(_this->grant_event, _this->next_cycles - cycles);
    }
    vp::io_req *current = _this->first_stalled, *prev = NULL;
    while (current && current->get_int(0) < req->get_int(0))
    {
      current = current->get_next();
    }
    if (prev) prev->set_next(req);
    else _this->first_stalled = req;
    req->set_next(current);
    return vp::IO_REQ_DENIED;
  }

  _this->next_cycles = cycles + duration;

  int current_step = _this->mode;

  if (_this->switch_mode)
  {
    _this->mode++;
    if (_this->mode == 4) _this->mode = 0;
  }


  // Switch between 2 different behaviors
  if (current_step == 0)
  {
    // First behavior, just reply synchronously with no latency
    return vp::IO_REQ_OK;
  }
  else if (current_step == 1)
  {
    // Third behavior, reply asynchronously
    _this->event_enqueue(_this->event_new(slave::send_resp, req), _this->resp_latency);
    return vp::IO_REQ_PENDING;
  }
}

int slave::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  nb_ports = get_config_int("nb_ports");
  bandwidth = get_config_int("bandwidth");
  mode = get_config_int("mode");
  switch_mode = get_config_bool("switch_mode");
  latency = get_config_int("latency");
  resp_latency = get_config_int("resp_latency");

  in = new vp::io_slave[nb_ports];

  for (int i=0; i<nb_ports; i++)
  {
    in[i].set_req_meth_muxed(&slave::req, i);
    new_slave_port("in_" + std::to_string(i), &in[i]);
  }

  next_cycles = -1;

  grant_event = event_new(slave::grant_request);

  return 0;
}

void slave::start()
{
  trace.msg("Starting slave (bandwidth: %d, mode: %d, switch_mode: %d, latency: %ld)\n",
    bandwidth, mode, switch_mode, latency);
}

slave::slave(const char *config)
: vp::component(config)
{
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new slave(config);
}
