#
# Copyright (C) 2018 ETH Zurich and University of Bologna
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)

import json
import json_tools as js
import os
from collections import OrderedDict
try:
    import pyyed
    import xml.dom.minidom as minidom
except:
    pass



class Interface(object):

    def __init__(self, comp, name):
        self.__dict__['name'] = name
        self.__dict__['comp'] = comp

    def get_slave(self):
        return self.comp.get_slave(self.name)


class Component(object):

    def __init__(self, properties=None):
        self.__dict__['is_empty'] = False
        self.__dict__['is_tb_comp'] = False

        self.__dict__['_Component__comps'] = OrderedDict()
        self.__dict__['_Component__master_itfs'] = OrderedDict()
        self.__dict__['_Component__slave_itfs'] = OrderedDict()

        if properties is not None:
            self.__dict__['_Component__properties'] = properties

    def get_master_itfs(self):
        return self.__dict__['_Component__master_itfs']

    def get_slave_itfs(self):
        return self.__dict__['_Component__slave_itfs']

    def set_name(self, name):
        self.__dict__['_Component__name'] = name

    def set_full_name(self, name):
        self.__dict__['_Component__fullname'] = name

    def get_name(self):
        return self.__dict__.get('_Component__name')

    def get_full_name(self):
        return self.__dict__.get('_Component__fullname')

    def get_json_config(self, configs=None, expand=False):
        return self.get_js_config(configs=configs, expand=expand).get_string()

    def get_js_config(self, configs=None, expand=False):
        result = js.import_config(self.gen(), interpret=expand)
        return result

    def dump_to_graph(self, g, gg):
        if len(self.__dict__.get('_Component__comps').values()) == 0:
            gg.add_node(self.get_full_name(), label=self.get_name())
        else:
            name = self.get_full_name()
            if name is None:
                name = 'top'
            ggg = gg.add_group(name, label=self.get_name())
            for comp in self.__dict__.get('_Component__comps').values():
                comp.dump_to_graph(g, ggg)

    def dump_edges_to_graph(self, g):
        if len(self.__dict__.get('_Component__comps').values()) == 0:
            for itf_name, slave_itf_list in self.get_master_itfs().items():
                for slave_itf in slave_itf_list:
                    slave = slave_itf.get_slave()
                    if slave is not None:
                        g.add_edge(self.get_full_name(), slave.get_full_name())

        else:
            for comp in self.__dict__.get('_Component__comps').values():
                comp.dump_edges_to_graph(g)

    def get_graph(self):
        g = pyyed.Graph()

        self.dump_to_graph(g, g)

        self.dump_edges_to_graph(g)

        graph = g.get_graph()
        return minidom.parseString(graph).toprettyxml(indent="  ")

    def get_component(self, name):
        return self.__dict__['_Component__comps'][name]

    def add_component(self, name, comp):
        self.__dict__['_Component__comps'][name] = comp
        self.__dict__[name] = comp

        comp.set_name(name)

        if self.get_full_name() is None:
            comp.set_full_name(name)
        else:
            comp.set_full_name(self.get_full_name() + '::' + name)

    def get(self, name):
        return self.__dict__[name]

    def set(self, name, value):
        self.__setattr__(name, value)

    def new_itf(self, name):
        itf = Interface(self, name)
        self.__dict__['_Component__slave_itfs'][name] = itf
        return itf

    def __setattr__(self, name, value):
        if type(value) == Interface:
            if self.__dict__['_Component__master_itfs'].get(name) is None:
                self.__dict__['_Component__master_itfs'][name] = []

            self.__dict__['_Component__master_itfs'][name].append(value)
        else:
            self.add_component(name, value)

    def __getattr__(self, name):
        if self.__dict__.get(name) is None:
            itf = Interface(self, name)
            self.__dict__['_Component__slave_itfs'][name] = itf
            return itf
        else:
            return self.__dict__[name]

    def get_property(self, name):
        return self.__dict__.get('_Component__properties').get(name)

    def set_property(self, name, value):
        if self.__dict__.get('_Component__properties') is None:
            self.__dict__['_Component__properties'] = OrderedDict()
        self.__dict__.get('_Component__properties')[name] = value

    def get_comp(self, name):
        comp = self.__dict__.get('_Component__vp_comps').get(name)
        if comp is None:
            comp = self.__dict__.get('_Component__tb_comps').get(name)
        return comp

    def gen(self):
        result = OrderedDict()

        properties = self.__dict__.get('_Component__properties')
        if properties is not None:
            for key, value in properties.items():
                result[key] = value

        comps = list(self.__dict__['_Component__comps'])
        if len(comps) != 0:
            vp_comps = []
            tb_comps = []
            for comp_name in comps:
                comp = self.__dict__['_Component__comps'].get(comp_name)
                if comp.__dict__.get('is_empty'):
                    continue

                if comp.__dict__.get('is_tb_comp'):
                    tb_comps.append(comp_name)
                else:
                    vp_comps.append(comp_name)

            if len(vp_comps) != 0:
                result['vp_comps'] = vp_comps
            if len(tb_comps) != 0:
                result['tb_comps'] = tb_comps

        vp_bindings = []
        tb_bindings = []

        ports = []
        for port in list(self.get_master_itfs().keys()) + list(self.get_slave_itfs().keys()):
            if not port in ports:
                ports.append(port)
        if len(ports) != 0:
            result['vp_ports'] = ports            

        for itf_name, slave_itf_list in self.get_master_itfs().items():
            for slave_itf in slave_itf_list:
                slave_name = slave_itf.comp.get_name()

                if self.__dict__.get('_Component__comps').get(slave_name) != slave_itf.comp:
                    continue
                binding = [
                    "self->%s" % (itf_name),
                    "%s->%s" % (slave_name, slave_itf.name)
                ]

                if slave_itf.comp.__dict__.get('is_tb_comp') or self.__dict__.get('is_tb_comp'):
                    tb_bindings.append(binding)
                else:
                    vp_bindings.append(binding)

        for comp_name, comp in self.__dict__['_Component__comps'].items():
            for itf_name, slave_itf_name in comp.get_master_itfs().items():
                for slave_itf in slave_itf_name:
                    if slave_itf.comp == self:
                        slave_name = 'self'
                    else:
                        slave_name = slave_itf.comp.get_name()

                        if self.__dict__.get('_Component__comps').get(slave_name) != slave_itf.comp:
                            continue

                    binding = [
                        "%s->%s" % (comp_name, itf_name),
                        "%s->%s" % (slave_name, slave_itf.name)
                    ]

                    if slave_itf.comp.__dict__.get('is_tb_comp') or comp.__dict__.get('is_tb_comp'):
                        tb_bindings.append(binding)
                    else:
                        vp_bindings.append(binding)


        if len(vp_bindings) != 0:
            result['vp_bindings'] = vp_bindings

        if len(tb_bindings) != 0:
            result['tb_bindings'] = tb_bindings

        for name, comp in self.__dict__['_Component__comps'].items():
            result[name] = comp.gen()

        return result

    def get_slave(self, name):
        if len(self.__dict__.get('_Component__comps').values()) == 0:
            return self
        else:
            if self.__dict__['_Component__master_itfs'].get(name) is not None:
                for itf in self.__dict__['_Component__master_itfs'][name]:
                    itf_name = itf.get_slave()
                    if itf_name is not None:
                        return itf_name
            return None


class Tb_Component(Component):
    def __init__(self, properties=None):
        super(Tb_Component, self).__init__(properties=properties)

        self.__dict__['is_tb_comp'] = True

class Empty_Component(Component):
    def __init__(self, properties=None):
        super(Empty_Component, self).__init__(properties=properties)

        self.__dict__['is_empty'] = True



def get_mapping(mapping, remove_base=False, add_offset=None):
    result = OrderedDict()

    result['base'] = mapping.get('base')
    result['size'] = mapping.get('size')

    if remove_base:
        result['remove_offset'] = result.get('base')

    if add_offset is not None:
        result['add_offset'] = add_offset

    return result

def get_area(base, size, index):
    return '0x%x' % (int(base, 0)+index*size)

def get_area_int(base, size, index):
    return '%d' % (int(base, 0)+index*size)



def get_mapping_area(mapping, size, index, remove_base=False, add_offset=None):
    result = OrderedDict()

    result['base'] = '0x%x' % (int(mapping.get('base'), 0)+index*size)
    result['size'] = mapping.get('size')

    if remove_base:
        result['remove_offset'] = result.get('base')

    if add_offset is not None:
        result['add_offset'] = '0x%x' % (int(add_offset, 0)+index*size)

    return result
