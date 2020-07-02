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
#include <vp/itf/clk.hpp>
#include <vp/itf/clock.hpp>
#include <vp/clock/clock_engine.hpp>

class clock_domain : public vp::clock_engine
{

public:

  clock_domain(js::config *config);

  int build();

  void pre_start();


private:

  static inline void set_frequency(void *__this, int64_t frequency);

  vp::clk_master out;

  vp::clock_slave clock_in;

  vp::trace clock_trace;
};


clock_domain::clock_domain(js::config *config)
: vp::clock_engine(config)
{
  this->apply_frequency(get_config_int("frequency"));
}

void clock_domain::set_frequency(void *__this, int64_t frequency)
{
  clock_domain *_this = (clock_domain *)__this;
  _this->apply_frequency(frequency);
  _this->clock_trace.event_real(_this->period);
}

int clock_domain::build()
{
  new_master_port("out", &out);

  clock_in.set_set_frequency_meth(&clock_domain::set_frequency);
  new_slave_port("clock_in", &clock_in);

  this->traces.new_trace_event_real("period", &this->clock_trace);

  this->traces.new_trace_event_real("cycles", &this->cycles_trace);

  this->set_time_engine((vp::time_engine*)this->get_service("time"));

  return 0;
}

void clock_domain::pre_start()
{
  out.reg(this);
}


vp::clock_engine::clock_engine(js::config *config)
  : vp::time_engine_client(config), cycles(0), period(0), freq(0), must_flush_delayed_queue(true)
{
  delayed_queue = NULL;
  for (int i=0; i<CLOCK_EVENT_QUEUE_SIZE; i++)
  {
    event_queue[i] = NULL;
  }
  current_cycle = 0;
}


extern "C" vp::component *vp_constructor(js::config *config)
{
  return new clock_domain(config);
}
