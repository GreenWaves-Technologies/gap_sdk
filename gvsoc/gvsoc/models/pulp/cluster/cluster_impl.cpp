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


#include <vp/vp.hpp>



class cluster : public vp::component
{

public:

  cluster(js::config *config);

  int build();
  void start();

private:

  vp::power_trace power_trace;

};

cluster::cluster(js::config *config)
: vp::component(config)
{

}


int cluster::build()
{
  if (this->power.new_trace("power_trace", &this->power_trace)) return -1;

  return 0;
}


void cluster::start()
{
  this->power_trace.collect();
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new cluster(config);
}
