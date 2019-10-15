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


from pandas import DataFrame, read_csv
import matplotlib.pyplot as plt
import pandas as pd
import regmap as rmap
import json_tools as js
import collections
import json

class Json_file(object):

    def __init__(self, name, path):
        self.file = open(path, 'w')
        self.name = name

    def close(self):
        self.file.close()


def import_json(regmap, config):

    for name, obj in config.get_items().items():
        __parse_elem_from_type(regmap, name, obj)


def import_json_file(regmap, config_path):

    config = js.import_config_from_file(config_path, find=True)
    import_json(regmap, config)


def __parse_elem_from_type(regmap_obj, name, config):
    if type(config) not in [js.config_object]:
        return

    obj_type = config.get_child_str('type')

    if obj_type is not None:

        if  obj_type == 'register':

          reg = regmap_obj.add_register(
            rmap.Register(
              name=name,
              offset=config.get_child_int('offset'),
              width=config.get_child_int('width'),
              desc=config.get_child_str('desc'),
              reset=config.get_child_int('reset'),
              help=config.get_child_str('help'),
              parent=regmap_obj
            )
          )

          content = config.get('content')
          if content is not None:
              for name, field in content.get_items().items():
                reg.add_regfield(
                  rmap.Regfield(
                    name=name,
                    full_name=field.get_child_str('name'),
                    width=field.get_child_int('width'),
                    bit=field.get_child_int('bit'),
                    access=field.get_child_str('access'),
                    desc=field.get_child_str('desc'),
                    reset=field.get_child_str('reset'),
                    reg_reset=reg.reset
                  )
                )

          return

        elif obj_type == 'group':
            group = regmap_obj.add_regmap(
                rmap.Regmap(
                    name=name,
                    offset=config.get_child_int('offset'),
                    parent=regmap_obj
                )
            )

            __parse_elem(group, name, config)

            return

        elif obj_type == 'template':
            group = regmap_obj.add_regmap(
                rmap.Regmap(
                    name=name,
                    offset=config.get_child_int('offset'),
                    parent=regmap_obj
                )
            )

            __parse_elem(group, name, config)

            return

        elif obj_type == 'custom':
            for name, obj in config.get_items().items():
                if obj.get_child_str('type') is not None:
                    regmap_obj.add_constant(
                        rmap.Constant(
                            name=name,
                            type=obj.get_child_str('type'),
                            value=obj.get_child_str('value'),
                            parent=regmap_obj
                        )
                    )

            return

    __parse_elem(regmap_obj, name, config)


def __parse_elems(regmap, config):

    for name, obj in config.get_items().items():
        __parse_elem_from_type(regmap, name, obj)

def __parse_elem(regmap, name, config):

    #self.offset = config.get_child_int('offset')
    #if self.offset is not None:
    #    self.offset = self.offset
    template = config.get_child_str('template')
    if template is not None:
        self.__get_template(template).clone(self)
        return

    __parse_elems(regmap, config)


class Regfield(object):

    def dump_to_dict(self, json):
        field_dict = collections.OrderedDict()

        if self.width is not None:
            field_dict['width'] = self.width

        if self.bit is not None:
            field_dict['bit'] = self.bit

        if self.access is not None:
            field_dict['access'] = self.access

        if self.desc is not None:
            field_dict['desc'] = self.desc

        if self.reset is not None:
            field_dict['reset'] = self.reset

        if self.reset is not None:
            field_dict['reset'] = self.reset

        if self.full_name is not None:
            field_dict['name'] = self.full_name

        return field_dict

class Register(object):

    def dump_to_dict(self, json):
        reg_dict = collections.OrderedDict()

        reg_dict['type'] = 'register'

        if self.help is not None:
            reg_dict['help'] = self.help

        if self.offset is not None:
            reg_dict['offset'] = self.offset

        if self.width is not None:
            reg_dict['width'] = self.width

        if self.desc is not None:
            reg_dict['desc'] = self.desc

        if self.reset is not None:
            reg_dict['reset'] = self.reset

        if len(self.fields) != 0:
            reg_dict['content'] = collections.OrderedDict()

            for name, field in self.fields.items():
                reg_dict['content'][name.lower()] = field.dump_to_dict(json)


        return reg_dict

class Regmap(object):

    def dump_to_dict(self, json):
        regmap_dict = collections.OrderedDict()

        if self.offset is not None:
            regmap_dict['offset'] = self.offset

        for name, register in self.registers.items():
            regmap_dict[name.lower()] = register.dump_to_dict(json)

        for name, group in self.regmaps.items():
            regmap_dict[name.lower()] = group.dump_to_dict(json)
            regmap_dict[name]['type'] = 'group'


        return regmap_dict


def dump_to_json(regmap, name, json_path):
    json_file = Json_file(name, json_path)
    regmap_dict = regmap.dump_to_dict(json_file)
    json_file.file.write(json.dumps(regmap_dict, indent='  '))
    json_file.close()
