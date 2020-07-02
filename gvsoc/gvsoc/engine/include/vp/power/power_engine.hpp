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

#ifndef __VP_POWER_ENGINE_HPP__
#define __VP_POWER_ENGINE_HPP__

#include "vp/vp_data.hpp"
#include "vp/component.hpp"
#include "vp/power/power.hpp"
#include <pthread.h>
#include <thread>

namespace vp {

  class power_engine : public component
  {
  public:
    power_engine(js::config *config);

    virtual void start_capture() {}

    virtual void stop_capture() {}

    virtual void reg_trace(vp::power_trace *trace) {}
  };

};

#endif
