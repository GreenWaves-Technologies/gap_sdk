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

#ifndef __VP_IMPLEMENTATION_HPP__
#define __VP_IMPLEMENTATION_HPP__

#include <map>
#include <list>
#include <string>
#include <vector>

#include "vp/vp.hpp"
#include "vp/clock/implementation.hpp"

using namespace std;

namespace vp {


  inline int component::get_config_int(std::string name, int index)
  {
    return get_js_config()->get(name)->get_elem(index)->get_int();
  }


  inline int component::get_config_int(std::string name)
  {
    return get_js_config()->get_child_int(name);
  }

  inline bool component::get_config_bool(std::string name)
  {
    return get_js_config()->get_child_bool(name);
  }

  inline std::string component::get_config_str(std::string name)
  {
    return get_js_config()->get_child_str(name);
  }

  inline int64_t component::get_time() { return this->get_time_engine()->get_time(); }


};

#endif
