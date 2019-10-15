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

#ifndef __VP_PORTS_HPP__
#define __VP_PORTS_HPP__

#include "vp/vp_data.hpp"
#include "vp/config.hpp"


namespace vp {

  class component;

  class port
  {

  public:

    virtual void bind_to(port *port, vp::config *config);
    virtual void finalize() {}

    void set_comp(component *comp) { this->owner = comp; }
    component *get_comp() { return owner; }

    inline void *get_context() { return this->context; }
    inline void set_context(void *comp) { this->context = comp; }

    inline void *get_remote_context() { return this->remote_context; }
    inline void set_remote_context(void *comp) { this->remote_context = comp; }

    inline void set_owner(component *comp) { this->owner = comp; }
    inline component *get_owner() { return this->owner; }

    inline void set_itf(void *itf) { this->itf = itf; }
    inline void *get_itf() { return this->remote_port->itf; }

  protected:

    // Component owner of this port.
    // The port is considered in the same domains as the owner component.
    component *owner = NULL;

    // Local context.
    // This pointer must be the first argument of any binding call to this port.
    void *context = NULL;

    // Remote context.
    // This pointer must be the first argument of any binding call to a remote port.
    void *remote_context = NULL;

    // Remote port connected to this port
    port *remote_port = NULL;

    void *itf;

  };

  class master_port : public port
  {

  };

  class slave_port : public port
  {
  };

  inline void port::bind_to(port *port, vp::config *config)
  {
    this->set_remote_context(port->get_context());
    remote_port = port;
  }

};  

#endif
