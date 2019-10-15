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

    trace_engine *get_trace_manager() { return trace_manager; }

  protected:

    std::map<std::string, trace *> traces;
    std::map<std::string, trace *> trace_events;
    trace_engine *trace_manager = NULL;

  private:
    component &top;

  };

};  


#endif
