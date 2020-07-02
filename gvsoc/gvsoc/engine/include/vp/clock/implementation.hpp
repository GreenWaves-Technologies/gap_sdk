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

#ifndef __VP_CLOCK_IMPLEMENTATION_HPP__
#define __VP_CLOCK_IMPLEMENTATION_HPP__

#include "vp/component.hpp"
#include "vp/itf/clk.hpp"
#include "vp/itf/wire.hpp"
#include "vp/clock/clock_event.hpp"
#include "vp/clock/component_clock.hpp"
#include "vp/clock/clock_engine.hpp"

using namespace std;

int64_t vp::component_clock::get_period() { return get_clock()->get_period(); }

int64_t vp::component_clock::get_frequency() { return get_clock()->get_frequency(); }

inline int64_t vp::component_clock::get_time() { return clock->get_time(); }

inline int64_t vp::component_clock::get_cycles() { return clock->get_cycles(); }


inline void vp::component_clock::event_enqueue(vp::clock_event *event, int64_t cycles)
{
  clock->enqueue(event, cycles);
}

inline void vp::component_clock::event_enqueue_ext(vp::clock_event *event, int64_t cycles)
{
  clock->enqueue_ext(event, cycles);
}

inline void vp::component_clock::event_cancel(vp::clock_event *event)
{
  clock->cancel(event);
}

inline void vp::component_clock::event_reenqueue(vp::clock_event *event, int64_t cycles)
{
  clock->reenqueue(event, cycles);
}

inline void vp::component_clock::event_reenqueue_ext(vp::clock_event *event, int64_t cycles)
{
  clock->reenqueue_ext(event, cycles);
}

inline vp::clock_event *vp::component_clock::event_new(vp::clock_event_meth_t *meth)
{
  return clock->event_new(this, meth);
}

inline vp::clock_event *vp::component_clock::event_new(void *_this, vp::clock_event_meth_t *meth)
{
  return clock->event_new(this, _this, meth);
}

inline void vp::component_clock::event_del(vp::clock_event *event)
{
  clock->event_del(this, event);
}

inline vp::clock_engine *vp::component_clock::get_clock()
{
  return clock;
}

inline vp::time_engine *vp::component_clock::get_engine()
{
  return get_clock()->get_engine();
}

inline void vp::clock_engine::sync()
{
  if (!is_running() && !nb_enqueued_to_cycle)
  {
    this->update();
  }
}


#endif
