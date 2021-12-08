# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


import json_tools as js
import collections
import os
import json



class Component(object):
    """
    This class corresponds to a generic HW component from which any component should inherit.

    Attributes
    ----------
    parent : Component
        The component which this components belongs to. Can be None if the component is the top one.
    name: str
        Name of the component. This names is used to indentify this component in the parent component.
    options: list
        List of options of forms key=value which should overwrite component properties.
    
    """

    def __init__(self, parent, name, options=[], is_top=False):
        self.name = name
        self.parent = parent
        self.json_config_files = []
        self.components = {}
        self.properties = {}
        self.bindings = []
        self.master_ports = []
        self.slave_ports = []
        self.build_done = False
        self.options = []
        self.comp_options = {}
        self.is_top = is_top
        self.vcd_group_create = True
        self.vcd_group_closed = True

        if len(options) > 0:
            for option in options:
                name, value = option.split('=', 1)
                name_list = name.split('/')
                name_list.append(value)

                self.options.append(name_list)

        if parent is not None:
            parent.add_component(name, self)

    def gen_stimuli(self, work_dir):
        """Generate stimuli.

        This method can be called to make the system described by this component hierarchy
        generates all the needed stimuli to launch execution.
        ANy sub-component can overload this method to get called and generates stimuli.

        Parameters
        ----------
        work_dir : str
            Working directory where the stimuli should be generated
        """
        for component in self.components.values():
            component.gen_stimuli(work_dir)


    def get_path(self, child_path=None, gv_path=False, *kargs, **kwargs):
        """Get component path.

        This returns the full path of the component, taking into account all the parents.

        Parameters
        ----------
        child_path : str, optional
            The path of the childs which can should appended to this path.
        *kargs, **kwargs
            Additional arguments which can be passed to the parents.

        Returns
        -------
        str
            The path.
        """

        if not gv_path or not self.is_top:
            path = self.name
            if child_path is not None:
                path = self.name + '/' + child_path

            if self.parent is not None:
                path = self.parent.get_path(child_path=path, gv_path=gv_path, *kargs, **kwargs)
        
        else:
            path = child_path

        return path


    def declare_flash(self, path=None):
        """Declare a flash.

        This declaration is propagated towards the parents so that the runner taking care of
        the flash image generation knows where the flash are.

        Parameters
        ----------
        path : str, optional
            This should be None when the flash is triggering the declaration and is then used
            to construct the path of the flash when going through the parents.
        """
        if self.parent is not None:
            if path is None:
                self.parent.declare_flash(self.name)
            else:
                self.parent.declare_flash(self.name + '/' + path)


    def declare_runner_target(self, path=None):
        """Declare a runner target.

        A runner target is a stand-alone system like a board for which the runner should generate stimuli like
        a flash image. This declaration is propagated towards the parents so that the runner taking care of
        the system knows where are the targets, and where it should generates stimulis.

        Parameters
        ----------
        path : str, optional
            This should be None when the runner target is triggering the declaration and is then used
            to construct the path of the runner when going through the parents.
        """
        if self.parent is not None:
            if path is None:
                self.parent.declare_runner_target(self.name)
            else:
                self.parent.declare_runner_target(self.name + '/' + path)


    def add_component(self, name, component):
        """Add a new component.

        The new component will be a sub-component of this component and will be identified by the specified name

        Parameters
        ----------
        name : str
            Name of the component.
        component: Component
            Component

        Returns
        -------
        Component
            The component
        """
        self.components[name] = component
        component.name = name

        # Determine the set of options which should be propagated to the sub-component
        # based on the path
        comp_options = []
        for option in self.options:
            option_name = None
            name_pos = 0
            for item in option:
                if item != "*" and item != "**":
                    option_name = item
                    break
                name_pos += 1

            if option_name == name:
                comp_options.append(option[name_pos + 1:])
            elif option[0] == "*":
                comp_options.append(option[1:])
            elif option[0] == "**":
                comp_options.append(option)

        if len(comp_options) != 0:
            component.__set_options(comp_options)

        return component


    def get_component(self, name):
        """Get a new component.

        Parameters
        ----------
        name : str
            Name of the component.
            
        Returns
        -------
        Component
            The component
        """
        return self.components[name]


    def add_property(self, name, property, format=None):
        """Add a property.

        A property is made available to the C++ model.

        Parameters
        ----------
        name : str
            Name of the property.

        property : str, int, float, list or dict
            Value of the property.
            
        """
        properties = self.properties

        for item in name.split('/')[:-1]:
            if properties.get(item) is None:
                properties[item] = {}

            properties = properties.get(item)

        properties[name.split('/')[-1]] = property

        return self.get_property(name, format=format)


    def get_property(self, name, format=None):
        """Get a property.

        The value of the property returned here can be overwritten by an option.

        Parameters
        ----------
        name : str
            Name of the property.

        Returns
        -------
        str, int, float, list or dict
            Value of the property.    
        """
        option_property = None

        comp_options = self.comp_options

        for item in name.split('/'):
            if comp_options.get(item) is None:
                option_property = None
                break
            else:
                option_property = comp_options.get(item)
            comp_options = comp_options.get(item)

        property = None
        for item in name.split('/'):
            if property is None:
                property = self.properties.get(item)
            else:
                property = property[item]

        if option_property is not None:
            if property is not None and type(property) == list:
                if type(property) == list:
                    property = property.copy()
                    if type(option_property) == list:
                        property += option_property
                    else:
                        property.append(option_property)
            else:
                property = option_property


        if format is not None:
            if format == int:
                if type(property) == str:
                    property = int(property, 0)
                else:
                    property = int(property)

        return property


    def bind(self, master, master_itf, slave, slave_itf):
        """Binds 2 components together.

        The binding can actually also involve 1 or 2 ports of the component to model bindings with something
        outside the component.

        Parameters
        ----------
        master : Component
            Master component. Can also be self if the master is outside this component.
        master_itf : str
            Name of the port where the binding should be done on master side.
        slave : Component
            Slave component. Can also be self if the slave is outside this component.
        slave_itf : str
            Name of the port where the binding should be done on slave side.
        """
        self.bindings.append([master, master_itf, slave, slave_itf])


    def load_property_file(self, path):
        """Loads a JSON property file.

        The file is imported as a python disctionary equivalent to the JSON file

        Returns
        -------
        dict
            The resulting dictionary
        """

        with open(self.get_file_path(path), 'r') as fd:
            return json.load(fd)


    def get_config(self):
        """Generates and return the system configuration.

        The whole hierarchy of components, bindings and properties are converted to a dictionnary
        representing the system, which can be serialized to a JSON file.

        Returns
        -------
        dict
            The resulting configuration
        """
        if not self.build_done:
            self.__build()

        config = {}

        for json_config_file in self.json_config_files:
            config = self.__merge_properties(config, js.import_config_from_file(json_config_file, find=True, interpret=True, gen=True).get_dict())

        for component_name, component in self.components.items():
            component_config = { component_name: component.get_config() }
            config = self.__merge_properties(config, component_config)

        #config = self.merge_options(config, self.comp_options, self.properties)

        config = self.__merge_properties(config, self.properties, self.comp_options)

        if len(self.bindings) != 0:
            if config.get('bindings') is None:
                config['bindings'] = []
            for binding in self.bindings:
                master_name = 'self' if binding[0] == self else binding[0].name
                slave_name = 'self' if binding[2] == self else binding[2].name
                config['bindings'].append([ '%s->%s' % (master_name, binding[1]), '%s->%s' % (slave_name, binding[3])])

        if len(self.components.values()) != 0:
            config = self.__merge_properties(config, { 'components' : list(self.components.keys()) })

        if len(self.master_ports) != 0 or len(self.slave_ports) != 0:
            config = self.__merge_properties(config, { 'ports' : self.master_ports + self.slave_ports })

        return config


    def get_file_path(self, json):
        """Return absolute config file path.

        The specified file is search from PYTHONPATH.

        Returns
        -------
        string
            The absolute path or None is the file is not found
        """

        if os.environ.get('PYTHONPATH') is not None:
            for dirpath in os.environ.get('PYTHONPATH').split(':'):
                path = os.path.join(dirpath, json)
                if os.path.exists(path):
                    return path

        return None


    def add_properties(self, properties):
        self.properties = self.__merge_properties(self.properties, properties)

    def vcd_group(self, closed=True, skip=False):
        self.vcd_group_create = not skip
        self.vcd_group_closed = closed

    def gen_gtkw_tree(self, tree, traces, filter_traces=False):
        
        if filter_traces:
            comp_traces = []
            subcomps_traces = []

            for trace in traces:
                if trace[0][0] == self.name:
                    if len(trace[0][1:]) == 0:
                        comp_traces.append([trace[0][1:], trace[1], trace[2]])
                    subcomps_traces.append([trace[0][1:], trace[1], trace[2]])
        else:
            comp_traces = []
            subcomps_traces = traces

        self.gen_gtkw_conf(tree, comp_traces)

        if self.vcd_group_create or tree.gen_full_tree:
            tree.begin_group(self.name, closed=self.vcd_group_closed)

        self.__gen_gtkw_wrapper(tree, comp_traces)

        for component in self.components.values():

            component.gen_gtkw_tree(tree, traces=subcomps_traces, filter_traces=self.is_top or filter_traces)

        if self.vcd_group_create or tree.gen_full_tree:
            tree.end_group(self.name, closed=self.vcd_group_closed)

    def __gen_traces(self, tree, traces):
        groups = {}
        for trace in traces:
            if len(trace[1]) == 1:
                tree.add_trace(self, trace[1][0], '.'.join(trace[0]) + trace[2])
            else:
                if groups.get(trace[1][0]) is None:
                    groups[trace[1][0]] = []
                
                groups[trace[1][0]].append([trace[0], trace[1][1:], trace[2]])

        for group_name, group in groups.items():
            tree.begin_group(group_name)
            self.__gen_traces(tree, group)
            tree.end_group(group_name)

    def __gen_gtkw_wrapper(self, tree, traces):
        if tree.gen_full_tree:
            self.__gen_traces(tree, traces)
        else:
            self.gen_gtkw(tree, traces)

    def gen_gtkw_conf(self, tree, traces):
        pass

    def gen_gtkw(self, tree, traces):
        pass

    def __set_options(self, options):
        for option in options:

            if True:
                comp_options = self.comp_options

                for item in option[:-2]:

                    if item in ['*', '**']:
                        continue

                    if comp_options.get(item) is None:
                        comp_options[item] = {}

                    comp_options = comp_options.get(item)

                name = option[-2]
                value = option[-1]

                if type(value) == dict or type(value) == collections.OrderedDict():
                    value = value.copy()

                if comp_options.get(name) is None:
                    comp_options[name] = value
                elif type(comp_options.get(name)) == list:
                    comp_options[name].append(value)
                else:
                    comp_options[name] = [ comp_options[name], value ]

            else:

                if len(option) == 1:
                    name = option[-2]
                    value = option[-1]
                    self.comp_options[name] = value

        self.options = options

    def __add_master_port(self, name):
        self.master_ports.append(name)


    def __add_slave_port(self, name):
        self.slave_ports.append(name)


    def __build(self):
        for component in self.components.values():
            component.__build()

        if len(self.bindings) != 0:
            for binding in self.bindings:
                binding[0].__add_master_port(binding[1])
                binding[2].__add_slave_port(binding[3])

        self.build_done = True
        

    def __merge_properties(self, dst, src, options=None, is_root=True):

        if type(src) == dict or type(src) == collections.OrderedDict:

            # Copy the source dictionary so that we can append the options
            # which do not appears in src
            src_merged = src.copy()

            # Copy all the options which do not appear in src
            if options is not None:
                for name, value in options.items():
                    if src_merged.get(name) is None and not is_root:
                        src_merged[name] = value

            # And merge dst, src and options
            for name, value in src_merged.items():
                if dst.get(name) is None:
                    new_dst = {}
                else:
                    new_dst = dst.get(name)

                if options is not None:
                    new_options = options.get(name)
                else:
                    new_options = None

                dst[name] = self.__merge_properties(new_dst, value, new_options, is_root=False)

            return dst

        elif type(src) == list:
            result = src.copy()
            if options is not None:
                if type(options) == list:
                    result += options
                else:
                    result.append(options)

            return result

        else:
            if options is not None:
                return options
            else:
                return src
