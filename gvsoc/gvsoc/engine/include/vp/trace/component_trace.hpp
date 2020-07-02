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

#ifndef __VP_TRACE_COMPONENT_TRACE_HPP__
#define __VP_TRACE_COMPONENT_TRACE_HPP__

#include "vp/component.hpp"
#include "vp/trace/trace.hpp"

using namespace std;

namespace vp {

  class trace;
  class trace_engine;

  typedef enum {
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE
  } trace_level_e;

  class component_trace
  {

  public:

    component_trace(component &top);

    void post_post_build();

    void new_trace(std::string name, trace *trace, trace_level_e level);

    void new_trace_event(std::string name, trace *trace, int width);

    void new_trace_event_string(std::string name, trace *trace);

    void new_trace_event_real(std::string name, trace *trace);

    inline trace_engine *get_trace_manager();

    void set_trace_manager(trace_engine *trace_manager) { this->trace_manager = trace_manager; }

  protected:

    std::map<std::string, trace *> traces;
    std::map<std::string, trace *> trace_events;
    trace_engine *trace_manager = NULL;

  private:
    void reg_trace(trace *trace, int event);

    component &top;

  };

};  


#endif
