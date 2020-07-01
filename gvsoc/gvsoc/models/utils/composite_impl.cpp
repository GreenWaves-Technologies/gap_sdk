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



class composite : public vp::component
{

public:
    composite(js::config *config);

    vp::port *get_slave_port(std::string name) { return this->ports[name]; }
    vp::port *get_master_port(std::string name) { return this->ports[name]; }

    void add_slave_port(std::string name, vp::slave_port *port) { this->add_port(name, port); }
    void add_master_port(std::string name, vp::master_port *port) { this->add_port(name, port); }

    int build();


private:
    void add_port(std::string name, vp::port *port);
    std::map<std::string, vp::port *> ports;
};



composite::composite(js::config *config)
    : vp::component(config)
{
}




int composite::build()
{
    this->create_comps();
    this->create_ports();
    this->create_bindings();

    return 0;
}



void composite::add_port(std::string name, vp::port *port)
{
    vp_assert_always(port != NULL, this->get_trace(), "Adding NULL port\n");
    //vp_assert_always(this->ports[name] == NULL, this->get_trace(), "Adding already existing port\n");
    this->ports[name] = port;
}



extern "C" vp::component *vp_constructor(js::config *config)
{
    return new composite(config);
}
