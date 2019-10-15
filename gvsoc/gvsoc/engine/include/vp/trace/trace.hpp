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

#ifndef __VP_TRACE_TRACE_HPP__
#define __VP_TRACE_TRACE_HPP__

#include "vp/vp_data.hpp"
#include "vp/trace/event_dumper.hpp"
#include <stdarg.h>

namespace vp {

  #define BUFFER_SIZE (1<<16)

  class trace_engine;

  class trace
  {

    friend class component_trace;
    friend class trace_engine;

  public:

    inline void msg(int level, const char *fmt, ...);
    inline void msg(const char *fmt, ...);
    inline void user_msg(const char *fmt, ...);
    inline void warning(const char *fmt, ...);
    inline void force_warning(const char *fmt, ...);
    inline void fatal(const char *fmt, ...);

    inline void event(uint8_t *value);
    inline void event_pulse(int64_t duration, uint8_t *pulse_value, uint8_t *background_value);
    inline void event_string(std::string value);
    inline void event_real(double value);
    inline void event_real_pulse(int64_t duration, double pulse_value, double background_value);
    inline void event_real_delayed(double value);

    inline string get_name() { return this->name; }

    void dump_header();
    void dump_warning_header();
    void dump_fatal_header();

    void set_active(bool active) { is_active = active; }
    void set_event_active(bool active) { is_event_active = active; }

  #ifndef VP_TRACE_ACTIVE
    inline bool get_active() { return false; }
    inline bool get_event_active() { return false; }
  #else
    inline bool get_active() { return is_active; }
    inline bool get_event_active() { return is_event_active; }
  #endif
    bool is_active = false;

    int width;
    int bytes;
    Event_trace *event_trace = NULL;
    bool is_real = false;
    bool is_string = false;
    int id;
    FILE *trace_file = stdout;

  protected:
    int level;
    component *comp;
    trace_engine *trace_manager;
    bool is_event_active = false;
    string name;
    uint8_t *buffer = NULL;
    uint8_t *buffer2 = NULL;
    trace *next;
    trace *prev;
    int64_t pending_timestamp;
  };    


#define vp_assert_always(cond, trace_ptr, msg...)  \
  if (!(cond)) {                                   \
    if (trace_ptr)                                 \
      ((vp::trace *)(trace_ptr))->fatal(msg);      \
    else                                           \
    {                                              \
      fprintf(stdout, "ASSERT FAILED: %s", msg);   \
      abort();                                     \
    }                                              \
  }

#define vp_warning_always(trace_ptr, msg...)       \
    if (trace_ptr)                                 \
      ((vp::trace *)(trace_ptr))->force_warning(msg);      \
    else                                           \
    {                                              \
      fprintf(stdout, "WARNING: ");                \
      fprintf(stdout, msg);                        \
      abort();                                     \
    }

#ifndef VP_TRACE_ACTIVE
#define vp_assert(cond, trace, msg...)
#else
#define vp_assert(cond, trace_ptr, msg...) vp_assert_always(cond, trace_ptr, msg)
#endif




  void fatal(const char *fmt, ...) ;


};

#endif
