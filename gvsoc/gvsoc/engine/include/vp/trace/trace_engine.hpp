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

#ifndef __VP_TRACE_ENGINE_HPP__
#define __VP_TRACE_ENGINE_HPP__

#include "vp/vp_data.hpp"
#include "vp/component.hpp"
#include "vp/trace/trace.hpp"
#include <pthread.h>
#include <thread>

namespace vp {

  #define TRACE_EVENT_BUFFER_SIZE (1<<16)
  #define TRACE_EVENT_NB_BUFFER   4

  class trace_engine : public component
  {
  public:
    trace_engine(js::config *config);

    void stop();

    virtual void reg_trace(vp::trace *trace, int event, string path, string name) = 0;

    virtual int get_max_path_len() = 0;
    virtual int get_trace_level() = 0;

    void dump_event(vp::trace *trace, int64_t timestamp, uint8_t *event, int width);

    void dump_event_string(vp::trace *trace, int64_t timestamp, uint8_t *event, int width);

    void dump_event_pulse(vp::trace *trace, int64_t timestamp, int64_t end_timestamp, uint8_t *pulse_event, uint8_t *event, int width);

    void dump_event_delayed(vp::trace *trace, int64_t timestamp, uint8_t *event, int width);

    void set_global_enable(bool enable) { this->global_enable = enable; }

    Event_dumper event_dumper;

    vp::trace *get_trace_from_path(std::string path);

    vp::trace *get_trace_from_id(int id);

    virtual void add_trace_path(int events, std::string path) {}
    virtual void add_exclude_trace_path(int events, std::string path) {}
    virtual void check_traces() {}

  protected:
    std::map<std::string, trace *> traces_map;
    std::vector<trace *> traces_array;

  private:
    void enqueue_pending(vp::trace *trace, int64_t timestamp, uint8_t *event);
    char *get_event_buffer(int bytes);
    void vcd_routine();
    void flush();
    void check_pending_events(int64_t timestamp);
    void dump_event_to_buffer(vp::trace *trace, int64_t timestamp, uint8_t *event, int bytes, bool include_size=false);
    void flush_Event_traces(int64_t timestamp);

    vector<char *> event_buffers;
    vector<char *> ready_event_buffers;
    char *current_buffer;
    int current_buffer_size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int end = 0;
    std::thread *thread;
    trace *first_pending_event;

    Event_trace *first_trace_to_dump;
    bool global_enable = true;
  };

};

#endif
