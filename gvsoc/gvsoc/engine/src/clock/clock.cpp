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
