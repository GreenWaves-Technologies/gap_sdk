#
# Copyright (C) 2018 ETH Zurich, University of Bologna
# and GreenWaves Technologies
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

import regmap_c_header
import regmap_table
import regmap_rst
import regmap_json
import collections


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


class Regfield(regmap_c_header.Regfield, regmap_table.Regfield, regmap_rst.Regfield, regmap_json.Regfield):

    def __init__(self, name, width, bit, access, desc, full_name=None, reset=None, reg_reset=None):
        self.name = name
        self.width = width
        self.bit = bit
        self.access = access
        self.desc = desc
        self.reset = reset
        self.reg_reset = reg_reset
        self.full_name = full_name



class Register(regmap_c_header.Register, regmap_table.Register, regmap_rst.Register, regmap_json.Register):

    def __init__(self, name, offset, width, desc, parent=None, reset=None, help=None):
        self.name = name
        self.parent = parent
        self.offset = offset
        self.width = width
        self.desc = desc
        self.reset = reset
        self.help = help
        self.fields = collections.OrderedDict([])

    def add_regfield(self, regfield):
        self.fields[regfield.name] = regfield

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



class Regmap(regmap_c_header.Regmap, regmap_table.Regmap, regmap_rst.Regmap, regmap_json.Regmap):

    def __init__(self, name, parent=None, offset=None):
        self.name = name
        self.parent = parent
        self.registers = collections.OrderedDict([])
        self.constants = collections.OrderedDict([])
        self.regmaps = collections.OrderedDict([])
        self.offset = offset

    def add_register(self, register):
        self.registers[register.name] = register
        return register

    def add_regmap(self, regmap):
        self.regmaps[regmap.name] = regmap
        return regmap

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
