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
