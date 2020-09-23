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
#include <vp/itf/clock.hpp>
#include <stdio.h>
#include <string.h>

class Clock : public vp::component
{

public:

  Clock(js::config *config);

  int build();
  void start();

private:

  static void edge_handler(void *__this, vp::clock_event *event);
  void raise_edge();

  vp::clock_master    clock_itf;
  vp::clock_event *event;
  int value;
};

void Clock::edge_handler(void *__this, vp::clock_event *event)
{
  Clock *_this = (Clock *)__this;
  _this->raise_edge();
}

void Clock::raise_edge()
{
  if (this->clock_itf.is_bound()) {
    this->get_trace()->msg("Changing clock level (level: %d)\n", value);
    this->clock_itf.sync(value);
    this->value ^= 1;
    this->event_enqueue(this->event, 1); 
  }
}

Clock::Clock(js::config *config)
: vp::component(config)
{
}

int Clock::build()
{
  this->event = this->event_new(Clock::edge_handler);

  this->new_master_port("clock_sync", &this->clock_itf);
  this->value = 0;

  return 0;
}

void Clock::start()
{
  this->clock_itf.set_frequency(this->get_clock()->get_frequency() / 2);

  this->event_enqueue(this->event, 1);
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Clock(config);
}
