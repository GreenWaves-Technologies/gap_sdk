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

#ifndef __VP_CLOCK_COMPONENT_CLOCK_HPP__
#define __VP_CLOCK_COMPONENT_CLOCK_HPP__

#include "vp/component.hpp"
#include "vp/itf/clk.hpp"
#include "vp/clock/clock_event.hpp"
#include "vp/itf/implem/wire_class.hpp"

using namespace std;

namespace vp {

  class config;

  class clock_engine;
  class time_engine;

  class component_clock
  {

  public:

    static void clk_reg(component *_this, component *clock);
    static void reset_sync(void *_this, bool active);

    inline void event_enqueue(clock_event *event, int64_t cycles);

    inline void event_reenqueue(clock_event *event, int64_t cycles);

    inline void event_enqueue_ext(clock_event *event, int64_t cycles);

    inline void event_reenqueue_ext(clock_event *event, int64_t cycles);

    inline void event_cancel(clock_event *event);

    clock_event *event_new(clock_event_meth_t *meth);

    clock_event *event_new(void *_this, clock_event_meth_t *meth);

    inline clock_event *event_new(clock_event_meth_t *meth, void *arg)
    {
      clock_event *event = event_new(meth);
      event->get_args()[0] = arg;
      return event;
    }

    void event_del(clock_event *event);

    inline clock_engine *get_clock();

    inline int64_t get_time();

    inline int64_t get_cycles();

    void pre_build(component *comp);

    inline int64_t get_period();

    inline int64_t get_frequency();

    inline time_engine *get_engine();

  protected:
    clock_engine *clock = NULL;

    clk_slave            clock_port;
    vp::wire_slave<bool> reset_port;

  };

};  


#endif
