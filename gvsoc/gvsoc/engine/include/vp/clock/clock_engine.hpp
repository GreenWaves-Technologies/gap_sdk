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

#ifndef __VP_CLOCK_ENGINE_HPP__
#define __VP_CLOCK_ENGINE_HPP__

#include "vp/vp_data.hpp"
#include "vp/component.hpp"
#include "vp/time/time_engine.hpp"

namespace vp {

  class clock_event;
  class component;

  class clock_engine : public time_engine_client
  {

  public:

    clock_engine(const char *config);

    void cancel(clock_event *event);

    void reenqueue_to_engine();

    bool dequeue_from_engine();

    void apply_frequency(int frequency);
    
    inline clock_event *reenqueue(clock_event *event, int64_t cycles);

    inline clock_event *reenqueue_ext(clock_event *event, int64_t cycles);

    inline clock_event *enqueue(clock_event *event, int64_t cycles)
    {
      vp_assert(!event->enqueued, 0, "Enqueueing already enqueued event\n");

      event->enqueued = true;

      // The event is enqueued directly into the circular buffer if it is
      // close enough.
      if (likely(is_running() && cycles < CLOCK_EVENT_QUEUE_SIZE))
      {
        enqueue_to_cycle(event, cycles);
      }
      else
      {
        enqueue_other(event, cycles);
      }
      return event;
    }

    inline clock_event *enqueue_ext(clock_event *event, int64_t cycles)
    {
      this->sync();
      return this->enqueue(event, cycles);
    }

    clock_event *event_new(component_clock *comp, clock_event_meth_t *meth)
    {
      clock_event *event = new clock_event(comp, meth);
      return event;
    }

    clock_event *event_new(component_clock *comp, void *_this, clock_event_meth_t *meth)
    {
      clock_event *event = new clock_event(comp, _this, meth);
      return event;
    }

    inline void retain() { engine->retain(); }
    inline void release() { engine->release(); }

    vp::clock_event *get_next_event();

    void event_del(component_clock *comp, clock_event *event)
    {
      delete event;
    }

    int64_t exec();

    inline void sync();

    void update();

    void set_time_engine(vp::time_engine *engine) { this->engine = engine; }

    vp::time_engine *get_engine() { return engine; }

    inline int64_t get_cycles() { return cycles; }

    inline void stop_engine(int status) { engine->stop_engine(status); }

    int64_t get_period() { return period; }

    int64_t get_frequency() { return freq; }

    bool has_events() { return this->nb_enqueued_to_cycle || this->delayed_queue; }

  protected:

    void flush_delayed_queue();

    inline void enqueue_to_cycle(clock_event *event, int64_t cycles)
    {
      // The position of one round of the circular buffer is always aligned
      // on the buffer size.
      int cycle = (current_cycle + cycles) & CLOCK_EVENT_QUEUE_MASK;
      event->next = event_queue[cycle];
      event_queue[cycle] = event;
      nb_enqueued_to_cycle++;
      event->cycle = cycles + get_cycles();
    }

    clock_event *enqueue_other(clock_event *event, int64_t cycles);

    clock_event *event_queue[CLOCK_EVENT_QUEUE_SIZE];
    clock_event *delayed_queue = NULL;
    int current_cycle = 0;
    int64_t period = 0;
    int64_t freq;

    // Gives the current cycle count of this engine.
    // It is always usable, whatever the state of the engine.
    // It is updated either when events are executed or when the 
    // engine is updated by an external interaction.
    int64_t cycles = 0;

    // Tells how many events are enqueued to the circular buffer.
    // If it is zero, there could still be some events in the delayed queue.
    int nb_enqueued_to_cycle = 0;

    // This time is relevant only when no event is enqueued into the circular
    // buffer of event so that the number of cycles can be resynchronized when
    // something happen (an event is pushed or the frequency is changed).
    // This is set when control is given back to the time engine and used
    // to recompute the numer of cycles when the engine is updated by an
    // external event.
    int64_t stop_time = 0;

    bool must_flush_delayed_queue;

    vp::trace cycles_trace;
  };    

};


inline vp::clock_event *vp::clock_engine::reenqueue(vp::clock_event *event, int64_t enqueue_cycles)
{
  int64_t cycles = this->cycles + enqueue_cycles;
  if (event->is_enqueued())
  {
    if (cycles >= event->get_cycle()) return event;
    cancel(event);

  }
  enqueue(event, enqueue_cycles);
  event->cycle = cycles;

  return event;
}

inline vp::clock_event *vp::clock_engine::reenqueue_ext(vp::clock_event *event, int64_t enqueue_cycles)
{
  this->sync();
  return this->reenqueue(event, enqueue_cycles);
}

#endif
