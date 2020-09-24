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

#include "vp/vp.hpp"

void vp::clk_master::bind_to(vp::port *_port, vp::config *config)
{
  clk_slave *port = (clk_slave *)_port;

  port->next = ports;
  ports = port;
}



void vp::clk_master::reg(component *clock)
{
  clk_slave *port = ports;
  while (port)
  {
    port->req((component *)port->get_context(), clock);
    port = port->next;
  }
}

vp::clk_slave::clk_slave() : req(NULL) {
  req = (clk_reg_meth_t *)&clk_slave::reg_default;
}

void vp::clk_slave::set_reg_meth(clk_reg_meth_t *meth)
{
  req = meth;
}

void vp::clk_slave::reg_default(clk_slave *)
{
}
