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


#ifndef __VP_PORTS_HPP__
#define __VP_PORTS_HPP__

#include "vp/vp_data.hpp"
#include "vp/config.hpp"

namespace vp
{

class slave_port;
class component;

class port
{

public:
    port(vp::component *owner = NULL) : owner(owner) {}

    virtual std::vector<vp::slave_port *> get_final_ports() { return {}; }
    virtual void bind_to(port *port, vp::config *config);
    virtual void finalize() {}

    virtual void final_bind() {}

    virtual bool is_virtual() { return false; }

    void set_comp(component *comp) { this->owner = comp; }
    component *get_comp() { return owner; }

    inline std::string get_name() { return this->name; }
    inline void set_name(std::string name) { this->name = name; }

    inline void *get_context() { return this->context; }
    inline void set_context(void *comp) { this->context = comp; }

    inline void *get_remote_context() { return this->remote_context; }
    inline void set_remote_context(void *comp)
    {
        this->is_bound = true;
        this->remote_context = comp;
    }

    inline void set_owner(component *comp) { this->owner = comp; }
    inline component *get_owner() { return this->owner; }

    inline void set_itf(void *itf) { this->itf = itf; }
    inline void *get_itf() { return this->remote_port->itf; }

    // This is called during components instantiation
    // to a connect a master port to a slave port which can be an
    // intermediate port from a composite component.
    // This binding will be used later on to connect this master port to final ports
    // implemented by final C++ IPs
    virtual void bind_to_virtual(port *port) {};

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

    void *itf = NULL;

    // Tell if the port is bound to another port
    bool is_bound = false;

    // Name of the port defined in the component, can used for debug purposes.
    std::string name = "";
};

class master_port : public port
{
public:
    master_port(vp::component *owner = NULL);

    void bind_to_virtual(port *port);
    void final_bind();
    void bind_to_slaves();

    std::vector<vp::slave_port *> get_final_ports();

protected:
    // Tell if the port is bound to a final slave port
    bool is_bound_to_port = false;

private:
    // Slave ports to which this one is connected. This may be virtual ports
    // if the binding is crossing composite components.
    std::vector<vp::port *> slave_ports;
};



class virtual_port : public master_port
{
public:
    virtual_port(vp::component *owner = NULL) : master_port(owner) {}
    bool is_virtual() { return true; }
};



class slave_port : public port
{
public:
    void final_bind();
    std::vector<vp::slave_port *> get_final_ports();
};



inline void port::bind_to(port *port, vp::config *config)
{
    this->set_remote_context(port->get_context());
    remote_port = port;
}



}; // namespace vp



#endif
