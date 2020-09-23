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

    static const int LEVEL_ERROR   = 0;
    static const int LEVEL_WARNING = 1;
    static const int LEVEL_INFO    = 2;
    static const int LEVEL_DEBUG   = 3;
    static const int LEVEL_TRACE   = 4;

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

    void register_callback(std::function<void()> callback) { this->callbacks.push_back(callback); }

    inline string get_name() { return this->name; }

    void set_full_path(std::string path) { this->full_path = path; }
    std::string get_full_path() { return this->full_path; }

    void dump_header();
    void dump_warning_header();
    void dump_fatal_header();

    void set_active(bool active);
    void set_event_active(bool active);

    void set_trace_manager(vp::trace_engine *engine) { this->trace_manager = engine; }

  #ifndef VP_TRACE_ACTIVE
    inline bool get_active() { return false; }
    inline bool get_active(int level) { return false; }
    inline bool get_event_active() { return false; }
  #else
    inline bool get_active() { return is_active; }
    bool get_active(int level);
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
    int is_event;

  protected:
    int level;
    component *comp;
    trace_engine *trace_manager;
    bool is_event_active = false;
    string name;
    string path;
    uint8_t *buffer = NULL;
    uint8_t *buffer2 = NULL;
    trace *next;
    trace *prev;
    int64_t pending_timestamp;
    string full_path;
    vector<std::function<void()>> callbacks;
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
