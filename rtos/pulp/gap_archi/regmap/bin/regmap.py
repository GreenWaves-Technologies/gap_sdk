#
# Copyright (C) 2019 GreenWaves Technologies
#

import regmap_c_header
import regmap_table
import regmap_rst
import regmap_json
import collections
import regmap_ipxact


class Cmdfield(regmap_rst.Cmdfield, regmap_c_header.Cmdfield):

    def __init__(self, name, width, offset, value, desc=None, parent=None, reset=None, help=None):
        self.name = name
        self.parent = parent
        self.width = width
        self.offset = offset
        self.desc = desc
        self.reset = reset
        self.value = value
        self.help = help

class Cmd(regmap_rst.Cmd, regmap_c_header.Cmd):

    def __init__(self, name, code, width, desc=None, parent=None, reset=None, help=None):
        self.name = name
        self.parent = parent
        self.code = code
        self.width = width
        self.desc = desc
        self.reset = reset
        self.help = help
        self.fields = collections.OrderedDict([])

    def add_cmdfield(self, cmdfield):
        self.fields[cmdfield.name] = cmdfield

    def get_cmdfield(self, name):
        return self.fields.get(name)

    def get_full_name(self):
        if self.parent is None:
            return self.name

        group = self.parent.get_group_path()

        if group is not None:
            return group + ':' + self.name
        else:
            return self.name

    def get_code(self):
        return self.code

class Cmdmap(regmap_rst.Cmdmap, regmap_c_header.Cmdmap):
    def __init__(self, name, parent=None):
        self.name = name
        self.parent = parent
        self.commands = collections.OrderedDict([])
        self.cmdmaps = collections.OrderedDict([])

    def add_cmd(self, command):
        self.commands[command.name] = command

    def get_cmd(self, name):
        return self.commands.get(name)

    def add_cmdmap(self, commandmap):
        self.cmdmaps[commandmap.name] = commandmap

    def get_cmds(self):
        return self.commands.values()

    def get_command(self, name):
        return self.commands.get(name)


class Constant(regmap_c_header.Constant):

    def __init__(self, name, type, value=None, parent=None):
        self.name = name
        self.type = type
        self.value = value
        self.parent = parent

    def get_full_name(self):
        if self.parent is None:
            return self.name

        group = self.parent.get_group_path()

        if group is not None:
            return group + ':' + self.name
        else:
            return self.name


class Regfield(regmap_c_header.Regfield, regmap_table.Regfield, regmap_rst.Regfield, regmap_json.Regfield, regmap_ipxact.Regfield):

    def __init__(self, name=None, width=None, bit=None, access=None, desc=None, full_name=None, reset=None, reg_reset=None):
        self.name = name
        self.width = width
        self.bit = bit
        self.access = access
        self.desc = desc
        self.reset = reset
        self.reg_reset = reg_reset
        self.full_name = full_name



class Register(regmap_c_header.Register, regmap_table.Register, regmap_rst.Register, regmap_json.Register, regmap_ipxact.Register):

    def __init__(self, name=None, offset=None, width=None, desc=None, parent=None, reset=None, help=None, do_reset=True):
        self.name = name
        self.parent = parent
        self.offset = offset
        self.width = width
        self.desc = desc
        self.reset = reset
        self.help = help
        self.fields = collections.OrderedDict([])
        self.do_reset = do_reset
        self.template = name

    def get_field_template(self):
        return self.template

    def get_fields(self):
        return self.fields.values()

    def add_regfield(self, regfield):
        self.fields[regfield.name] = regfield

    def get_regfield(self, name):
        return self.fields.get(name)

    def get_full_name(self):
        if self.parent is None:
            return self.name

        group = self.parent.get_group_path()

        if group is not None:
            return group + ':' + self.name
        else:
            return self.name

    def get_offset(self):
        return self.offset
        parent_offset = self.parent.get_offset()

        if parent_offset is not None:
            return parent_offset + self.offset
        else:
            return self.offset



class Regmap(regmap_c_header.Regmap, regmap_table.Regmap, regmap_rst.Regmap, regmap_json.Regmap, regmap_ipxact.Regmap):

    def __init__(self, name, parent=None, offset=None, range=None):
        self.name = name
        self.parent = parent
        self.registers = collections.OrderedDict([])
        self.constants = collections.OrderedDict([])
        self.cmdmaps = collections.OrderedDict([])
        self.regmaps = collections.OrderedDict([])
        self.offset = offset
        self.input_file = None
        self.range = range

    def set_input_file(self, input_file):
        self.input_file = input_file

    def get_registers(self):
        return self.registers.values()

    def add_register(self, register):
        self.registers[register.name] = register
        return register

    def add_regmap(self, regmap):
        self.regmaps[regmap.name] = regmap
        return regmap

    def add_cmdmap(self, cmdmap):
        self.cmdmaps[cmdmap.name] = cmdmap
        return cmdmap

    def add_constant(self, constant):
        self.constants[constant.name] = constant
        return constant

    def get_register(self, name):
        return self.registers.get(name)

    def get_group_path(self):
        if self.parent is None:
            return self.name

        group = self.parent.get_group_path()

        if group is not None:
            return group + ':' + self.name
        else:
            return self.name
