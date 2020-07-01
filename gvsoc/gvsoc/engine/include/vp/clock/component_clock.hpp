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
