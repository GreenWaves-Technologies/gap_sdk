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

#ifndef __VP_CLOCK_EVENT_HPP__
#define __VP_CLOCK_EVENT_HPP__

#include "vp/vp_data.hpp"

namespace vp {

  class clock_event;
  class component;
  class component_clock;

  #define CLOCK_EVENT_PAYLOAD_SIZE 64
  #define CLOCK_EVENT_NB_ARGS 8
  #define CLOCK_EVENT_QUEUE_SIZE 32
  #define CLOCK_EVENT_QUEUE_MASK (CLOCK_EVENT_QUEUE_SIZE - 1)

  typedef void (clock_event_meth_t)(void *, clock_event *event);

  class clock_event
  {

    friend class clock_engine;

  public:

    clock_event(component_clock *comp, clock_event_meth_t *meth);

    clock_event(component_clock *comp, void *_this, clock_event_meth_t *meth) 
      : comp(comp), _this(_this), meth(meth), enqueued(false) {}

    inline int get_payload_size() { return CLOCK_EVENT_PAYLOAD_SIZE; }
    inline uint8_t *get_payload() { return payload; }

    inline int get_nb_args() { return CLOCK_EVENT_NB_ARGS; }
    inline void **get_args() { return args; }

    inline bool is_enqueued() { return enqueued; }

    int64_t get_cycle() { return cycle; }

  private:
    uint8_t payload[CLOCK_EVENT_PAYLOAD_SIZE];
    void *args[CLOCK_EVENT_NB_ARGS];
    component_clock *comp;
    void *_this;
    clock_event_meth_t *meth;
    clock_event *next;
    bool enqueued;
    int64_t cycle;
  };    

};

#endif
