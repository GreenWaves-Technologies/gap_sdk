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

#ifndef __VP_ITF_CLK_HPP__
#define __VP_ITF_CLK_HPP__

#include "vp/vp_data.hpp"
#include "vp/ports.hpp"

namespace vp {

  class component;

  typedef void (clk_reg_meth_t)(component *_this, component *clock);

  class clk_slave;

  class clk_master : public master_port
  {
  public:

    clk_master() : ports(NULL) {}

    void reg(component *clock);

    void bind_to(vp::port *port, vp::config *config);

  private:
    void (*reg_meth)(component *, component *clock);

    clk_slave *ports;
  };



  class clk_slave : public slave_port
  {

    friend class clk_master;

  public:

    clk_slave();

    void set_reg_meth(clk_reg_meth_t *meth);


  private:

    void (*req)(component *comp, component *clock);
    static void reg_default(clk_slave *);

    clk_slave *next;

  };


};

#endif
