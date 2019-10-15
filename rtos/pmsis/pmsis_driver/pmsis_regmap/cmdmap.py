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

class Cmdfield(regmap_c_header.Cmdfield):

    def __init__(self, name, width, offset, value, desc, parent=None, reset=None, help=None):
        self.name = name
        self.parent = parent
        self.width = width
        self.offset = offset
        self.desc = desc
        self.reset = reset
        self.value = value
        self.help = help

class Cmd(regmap_c_header.Cmd):

    def __init__(self, name, code, width, desc, parent=None, reset=None, help=None):
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

class Cmdmap(regmap_c_header.Cmdmap):
    def __init__(self, name, parent=None):
        self.name = name
        self.parent = parent
        self.commands = collections.OrderedDict([])
        self.cmdmaps = collections.OrderedDict([])

    def add_cmd(self, command):
        self.commands[command.name] = command

    def add_cmdmap(self, commandmap):
        self.cmdmaps[commandmap.name] = commandmap

    def get_command(self, name):
        return self.commands.get(name)
