#
# Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
#                    University of Bologna
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

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

import imp
import ctypes
import ctypes.util
import json_tools as js


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


class port(object):

    def __init__(self, comp, name, is_slave=False):
        self.name = name
        self.slaves = []
        self.is_a_slave = is_slave
        self.comp = comp
        self.is_bound = False

    def get_ports(self):
        ports = []
        for slave in self.slaves:
            ports += slave.get_ports()
        if self.is_a_slave:
            ports.append(self)
        return ports

    def get_comp(self):
        return self.comp

    def bind_to(self, port, config=None):
        if port is not None:
            self.comp.trace.msg('Binding composite master port (master: %s->%s, slave: %s->%s)' % (self.get_comp().name, self.name, port.get_comp().name, port.name))
            self.slaves.append(port)
            port.is_bound = True

    def is_master(self):
        return True

    def is_slave(self):
        return True

class impl_master_port(object):

    def __init__(self, implem, name, ref):
        self.name = name
        self.ref = ref
        self.implem = implem
        self.slaves = []
        self.is_bound = False
        self.is_bound_to_port = False

    def bind_to(self, port, config=None):
        if port is None:
            raise Exception("SLAVE IS NONE")

        self.slaves.append([port, config])
        port.is_bound = True
        self.is_bound = True

    def is_master(self):
        return True

    def is_slave(self):
        return False

    def bind(self):
        for slave_desc in self.slaves:
            slave = slave_desc[0]
            config = slave_desc[1]

            for port in slave.get_ports():
                self.implem.parent.trace.msg('Binding implementation master port (master: %s->%s, slave: %s->%s)' % (self.implem.parent.name, self.name, port.implem.parent.name, port.name))
                if config is not None:
                    config = str(config).replace('\'', '"').encode('utf-8')

                self.is_bound_to_port = True

                self.implem.implem_bind_to(self.ref, port.ref, config)


class impl_slave_port(object):

    def __init__(self, implem, name, ref):
        self.name = name
        self.ref = ref
        self.implem = implem
        self.is_bound = False

    def get_comp(self):
        return self.implem.parent

    def bind(object):
        pass

    def get_ports(self):
        return [self]

    def is_master(self):
        return False

    def is_slave(self):
        return True


class default_implementation_class(object):

    def __init__(self, name, config, parent, debug, path=None):

        self.parent = parent
        self.instance = None

        self.master_ports = {}
        self.slave_ports = {}

        if debug:
            name = 'debug.' + name

        for x in name.split('.'):
            if path is not None:
                path=[path]

            file, path, descr = imp.find_module(x, path)

        self.module = ctypes.CDLL(path)

        self.module.vp_constructor.argtypes = [ctypes.c_char_p]
        self.module.vp_constructor.restype = ctypes.c_void_p

        self.module.vp_comp_set_config.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

        self.implem_reset = self.module.vp_reset
        self.module.vp_reset.argtypes = [ctypes.c_void_p, ctypes.c_int]

        self.implem_load = self.module.vp_load
        self.module.vp_load.argtypes = [ctypes.c_void_p]

        self.implem_stop = self.module.vp_stop
        self.module.vp_stop.argtypes = [ctypes.c_void_p]

        self.implem_run = self.module.vp_run
        self.module.vp_run.argtypes = [ctypes.c_void_p]
        self.module.vp_run.restype = ctypes.c_char_p

        self.implem_build = self.module.vp_build
        self.module.vp_build.argtypes = [ctypes.c_void_p]

        self.implem_build_new = self.module.vp_build_new
        self.module.vp_build_new.argtypes = [ctypes.c_void_p]

        self.implem_bind_to = self.module.vp_port_bind_to
        self.module.vp_port_bind_to.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p]

        self.module.vp_get_error.restype = ctypes.c_char_p

        self.implem_get_ports = self.module.vp_comp_get_ports
        self.module.vp_comp_get_ports.argtypes = \
            [ctypes.c_void_p, ctypes.c_bool, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p),
                ctypes.c_void_p]
        self.module.vp_comp_get_ports.restype = ctypes.c_int

        if config is not None:
            config_str = config.get_string().encode('utf-8')
        else:
            config_str = None

        self.instance = self.module.vp_constructor(config_str)

        self.module.vp_comp_conf.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
        self.module.vp_run_status.argtypes = [ctypes.c_void_p]
        self.module.vp_run_status.restype = ctypes.c_int

        self.module.vp_comp_conf(self.instance, self.get_path().encode('utf-8'), self.parent.get_parent_implem_class())

    def get_path(self):
        return self.parent.get_path()

    def start(self):
        return self.implem_start(self.instance)

    def reset(self, active):
        return self.implem_reset(self.instance, active)

    def load(self):
        return self.implem_load(self.instance)

    def stop(self):
        return self.implem_stop(self.instance)

    def run(self):
        return self.implem_run(self.instance).decode('utf-8')

    def run_status(self):
        return self.module.vp_run_status(self.instance)

    def get_port_class(self, master):
        if master:
            return impl_master_port
        else:
            return impl_slave_port


    def __get_ports(self, master):

        ports_dict = self.master_ports if master else self.slave_ports

        size = self.implem_get_ports(self.instance, master, 0, None, None)

        if size != 0:
            ports = (ctypes.c_void_p * size)()
            names = (ctypes.c_char_p * size)()

            size = self.implem_get_ports(self.instance, master, size, names, ports)

            for i in range(0, len(names)):

                name = names[i].decode('utf-8')

                port_class = self.get_port_class(master)

                port = port_class(self, name, ports[i])

                ports_dict[name] = port


    def __get_port(self, master, name):
        if master:
            return self.master_ports.get(name)
        else:
            return self.slave_ports.get(name)

    def get_port(self, name):
        port = self.__get_port(True, name)
        if port is not None:
            return port
        return self.__get_port(False, name)

    def get_error(self):
        return self.module.vp_get_error().decode('utf-8')

    def build(self):
        self.parent.trace.msg('Building implementation')

        self.module.vp_comp_set_config(self.instance, self.parent.json_config.dump_to_string().encode('utf-8'))

        # First execute the C build method, this will mainly declare ports
        retval = self.implem_build(self.instance)
        if retval != 0:
            self.parent.trace.error(self.get_error(), path=self.get_path())
            raise Exception("Caught error while building component (path: %s)" % self.get_path())

        # Retrieve ports information from C world and register them in the
        # python class
        self.__get_ports(True)
        self.__get_ports(False)



class trace(object):

    def __init__(self, comp, engine, path):
        self.comp = comp
        self.engine = engine
        self.path = path
        self.is_active = False
        if self.engine is not None:
            self.is_active = self.engine.is_active(path)
            self.engine.reg_path(self.path)

    def msg(self, str, path=None):
        if path is None:
            path = self.path

        if self.is_active:
            self.engine.dump_msg(path, str)

    def error(self, str, path=None):
        if path is None:
            path = self.path
            
        self.engine.dump_error(path, str)





class component_trace(object):

    trace_engine = None

    def init(self):
        self.trace_engine = self.get_service('trace')

    def new_trace(self, name):
        return trace(self, self.trace_engine, self.get_path() + '/' + name)



class component_power(object):

    power_engine = None

    def __init__(self, top):
        self.top = top

    def init(self):
        self.power_engine = self.top.get_service('power')



class component(component_trace):

    implementation_class = default_implementation_class

    def get_json_config(self, config):
        return js.import_config(config.get_dict())

    def __init__(self, name, config, debug, parent=None):

        super(component, self).__init__()

        self.services = {}
        self.sub_comps = []
        self.sub_comps_dict = {}
        self.config = config
        self.parent = parent
        self.impl = None
        self.name = name
        self.ports = {}
        self.debug = debug
        self.power = component_power(self)
        if name is None:
            self.path = ''
        else:
            if parent is None:
                self.path = '/' + name
            else:
                parent_path = parent.get_path()
                if parent_path == '':
                    self.path = '/' + name
                else:
                    self.path = parent.get_path() + '/' + name

        self.json_config = self.get_json_config(config)

        if hasattr(self, 'implementation'):
            self.impl = self.implementation_class(getattr(self, 'implementation'), config, parent=self, debug=debug)

        component_trace.init(self)
        self.power.init()

        self.trace = self.new_trace('py_comp')

        self.build_all()

    def get_implem_class(self):
        if self.impl is not None:
            return self.impl.instance
        else:
            return None

    def get_parent_implem_class(self):
        if self.parent is None:
            return None
        else:
            return self.parent.get_implem_class()

    def get_parent(self):
        return self.parent

    def get_path(self):
        return self.path

    def get_json(self):
        return self.json_config

    def get_module(self, name, path=None):
        for x in name.split('.'):
            if path is not None:
                path=[path]
            file, path, descr = imp.find_module(x, path)

        module = imp.load_module(name, file, path, descr)
        return module

    def get_component(self, name, path=None):
        return self.get_module(name).component

    def get_nb_parent(self):
        if self.get_parent() is None:
            return 0
        else:
            return self.get_parent().get_nb_parent() + 1

    def get_comp(self, name):
        return self.sub_comps_dict.get(name)

    def get_comp_from_list(self, name_list):
        comp = self.get_comp(name_list[0])
        if comp is None or len(name_list) == 1:
            return comp
        else:
            return comp.get_comp_from_list(name_list[1:])

    def new(self, name, component, config=None):

        self.trace.msg('New component (name: %s, class: %s)' % (name, component))

        comp = self.get_component(component)(name, config, parent=self, debug=self.debug)
        self.sub_comps.append(comp)
        self.sub_comps_dict[name] = comp

        return comp

    def build(self):
        pass

    def build_all(self):

        for build in self.sub_comps:
            build.build_all()

        self.trace.msg('Building component')

        self.build()

        if self.impl is not None:
            self.impl.build()

        ports = [ ['clock', False], ['reset', False] ]

        for port_desc in ports:
            port = port_desc[0]
            self.new_port(port, is_slave=port_desc[1])

            for comp in self.sub_comps:
                slave_port = comp.get_port(port)
                master_port = self.get_port(port)
                if slave_port.is_bound:
                    continue
                if master_port.is_master() and slave_port.is_slave():
                    self.get_port(port).bind_to(slave_port)

    def start(self):
        pass

    def reset(self, active):
        pass

    def load(self):
        pass

    def stop(self):
        pass

    def run(self):
        if self.impl is not None:
            return self.impl.run()
        return "error"

    def run_status(self):
        if self.impl is not None:
            return self.impl.run_status()
        else:
            return -1

    def build_new(self):
        self.impl.implem_build_new(self.impl.instance)


    def reset_all(self, active):

        #for build in self.sub_comps:
        #    build.reset_all()

        self.trace.msg('Resetting component')

        self.reset(active)

        if self.impl is not None:
            self.impl.reset(active)

        return 0

    def load_all(self):

        for build in self.sub_comps:
            build.load_all()

        self.trace.msg('Load step')

        self.load()

        if self.impl is not None:
            self.impl.load()

        return 0

    def stop_all(self):

        for build in self.sub_comps:
            build.stop_all()

        self.trace.msg('Stopping component')

        self.stop()

        if self.impl is not None:
            self.impl.stop()

        return 0

    def get_impl(self):
        return self.impl

    def new_port(self, name, is_slave=False):
        self.ports[name] = port(self, name, is_slave=is_slave)

    def get_service(self, name):
        if self.parent is not None:
            return self.parent.get_service(name)
        else:
            return self.services.get(name)

    def new_service(self, name, comp=None):
        if comp is None:
            comp = self
        if self.parent is not None:
            self.parent.new_service(name, comp)
        else:
            self.services[name] = comp

    def get_port(self, name):

        # First look inside the implementation ports
        if self.impl is not None:
            port = self.impl.get_port(name)
            if port is not None:
                return port

        # If not found, look into the python ports
        if self.ports.get(name) is not None:
            return self.ports[name]

        return None

    def get_time_engine(self):
        if self.parent is None:
            return None
        return self.parent.get_time_engine()

    def bind(self):

        for comp in self.sub_comps:
            comp.bind()

        if  self.impl is not None:
            for port in self.impl.master_ports.values():
                port.bind()

    def create_comps(self, comps_tag, class_tag, default_class_name):
        js_config = self.get_json()
        comps = js_config.get(comps_tag)
        if comps is not None:
            for comp_name in comps.get_dict():
                comp_config = js_config.get(comp_name)
                component = comp_config.get_child_str(class_tag)
                if component is None:
                    component = default_class_name

                self.new(comp_name, component=component, config=comp_config)


    def create_ports(self, port_tag):
        ports = self.get_json().get(port_tag)
        if ports is not None:
            for port in ports.get_dict():
                self.new_port(port)


    def create_bindings(self, bindings_tag):

        bindings = self.get_json().get(bindings_tag)
        if bindings is not None:
            for binding in bindings.get_dict():
                master_comp_name, master_port_name = binding[0].split('->')
                slave_comp_name, slave_port_name = binding[1].split('->')
                self.trace.msg('Creating json binding (master: %s, slave: %s)' % (binding[0], binding[1]))

                if master_comp_name == 'self':
                    master_comp = self
                else:
                    master_comp = self.get_comp_from_list(master_comp_name.split('/'))

                if slave_comp_name == 'self':
                    slave_comp = self
                else:
                    slave_comp = self.get_comp_from_list(slave_comp_name.split('/'))

                if master_comp is None:
                    raise Exception('Unknown master component in binding: %s' % master_comp_name)

                if slave_comp is None:
                    raise Exception('Unknown slave component in binding: %s' % slave_comp_name)

                master_port = master_comp.get_port(master_port_name)
                slave_port = slave_comp.get_port(slave_port_name)

                if master_port is None:
                    raise Exception('Unknown master port in binding: %s' % binding[0])

                if slave_port is None:
                    raise Exception('Unknown slave port in binding: %s' % binding[1])

                master_port.bind_to(slave_port)





def map_config(base=0, size=0, add_offset=0, remove_offset=0):    
    binding_conf = {}
    binding_conf['base'] = base
    binding_conf['size'] = size
    binding_conf['add_offset'] = add_offset
    binding_conf['remove_offset'] = remove_offset
    return binding_conf
