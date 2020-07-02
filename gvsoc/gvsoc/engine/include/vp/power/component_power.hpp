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
