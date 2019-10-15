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
#include <vp/itf/clk.hpp>
#include <vp/itf/clock.hpp>
#include <vp/clock/clock_engine.hpp>

class clock_domain : public vp::clock_engine
{

public:

  clock_domain(const char *config);

  int build();

  void pre_start();


private:

  static inline void set_frequency(void *__this, int64_t frequency);

  vp::clk_master out;

  vp::clock_slave clock_in;

  vp::trace clock_trace;
};


clock_domain::clock_domain(const char *config)
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

  return 0;
}

void clock_domain::pre_start()
{
  out.reg(this);
}


vp::clock_engine::clock_engine(const char *config)
  : vp::time_engine_client(config), cycles(0), period(0), freq(0), must_flush_delayed_queue(true)
{
  delayed_queue = NULL;
  for (int i=0; i<CLOCK_EVENT_QUEUE_SIZE; i++)
  {
    event_queue[i] = NULL;
  }
  current_cycle = 0;
}


extern "C" void vp_set_time_engine(void *comp, void *engine)
{
  ((vp::clock_engine *)comp)->set_time_engine((vp::time_engine *)engine);
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new clock_domain(config);
}
