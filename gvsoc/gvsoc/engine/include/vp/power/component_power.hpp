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

#ifndef __VP_TRACE_COMPONENT_POWER_HPP__
#define __VP_TRACE_COMPONENT_POWER_HPP__

#include "vp/component.hpp"
#include "vp/power/power.hpp"
#include "json.hpp"

using namespace std;

namespace vp {

  class power_trace;
  class power_source;

  class component_power
  {

  public:

    component_power(component &top);

    power_engine *get_engine() { return power_manager; }

    void post_post_build();

    void pre_start();

    int new_event(std::string name, power_source *source, js::config *config, power_trace *trace, bool is_leakage=false);

    int new_leakage_event(std::string name, power_source *source, js::config *config, power_trace *trace) { return this->new_event(name, source, config, trace, true); }

    int new_trace(std::string name, power_trace *trace);

    void reg_top_trace(vp::power_trace *trace);

  protected:

  private:
    component &top;

    std::vector<power_trace *> traces;

    power_engine *power_manager = NULL;
  };

};  


#endif
