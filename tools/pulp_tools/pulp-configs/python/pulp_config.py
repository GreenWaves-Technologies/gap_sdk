#
# Copyright (C) 2018 ETH Zurich, University of Bologna and
# GreenWaves Technologies
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


import json_tools as js
import imp
import os
import shlex
import configparser
import collections


class Pulp_config(js.config_object):

    def __init__(self, name, config_dict, interpret=False, config_name=None):
        super(Pulp_config, self).__init__(config_dict, interpret=interpret)

        self.name = name
        self.config_name = config_name

    def get_name(self):
        if self.config_name is not None:
          return 'config_tag=%s' % self.config_name
        return self.name

    def __str__(self): return self.get_config_name()

    def get_config_name(self):
        if self.name.find('@') == -1:
            return self.name
        else:
            return self.name.split('@')[0]

    def get_config_tag(self):
      return self.config_name

    def get_name_from_items(self, items):
        result = []
        for item in items:
            value = self.get_str(item)
            if value == None: continue
            result.append("%s=%s" % (item, value))
        return ":".join(result)


def get_config_from_tag(name):
  config_path = '%s@config_file=chips/%s/%s.json' % (name, name, name)
  return get_config(config_path, interpret=True, config_name=name)

def get_config_from_string(name, config_string, interpret=False, **kwargs):

    return create_config(name, js.import_config(config_string), interpret=interpret)

def create_config(name, config, config_name=None, interpret=False, **kwargs):

    type_config = config.get_child_str("config_type")

    if type_config is None:

        return Pulp_config(name, config.get_dict(), interpret=interpret, config_name=config_name)

    else:

        if type_config == 'generator':
            
            generator = config.get_child_str("generator")

            file, path, descr = imp.find_module(generator, None)
            module = imp.load_module(generator, file, path, descr)

            return Pulp_config(name, module.get_config(config, **kwargs).get_dict(), interpret=interpret, config_name=config_name)

        else:

            raise Exception('Unknown config type: ' + type_config)

def get_config(file, name="", config_name=None, ini_configs=[], ini_configs_dict={}, config_opts=[], properties=[], interpret=False, **kwargs):

    template_properties = []
    config_properties = config_opts

    for prop in properties:
      if prop.find('config/') == 0:
        config_properties.append(prop.split('config/')[1])
      else:
        template_properties.append(prop)

    if file is not None and file.find('@') != -1:
      name, file = file.split('@')

    if file is not None and file.find('config_file') != -1:
      dummy, file = file.split('=', 1)

    opts = []

    if file.find(':'):
      opts = file.split(':')[1:]
      file = file.split(':')[0]


    config = js.import_config_from_file(file, find=True, interpret=interpret)

    for ini_config in ini_configs:
      parser = configparser.SafeConfigParser(ini_configs_dict, dict_type=collections.OrderedDict)
      parser.optionxform = str
      paths = parser.read(ini_config)
      if len(paths) == 0:
          raise Exception("Didn't manage to open file: %s" % (ini_config))
          
      user_config = collections.OrderedDict()
      for section in parser.sections():
        for item in parser.items(section):
          path = ('%s.%s' % (section, item[0])).split('.')
          config.set_from_list(path, item[1])

      for prop in template_properties:
        for key, value in prop.split('='):
          config.user_set(key, value)

      config = js.import_config(config.get_dict(),  interpret=interpret)

    result = create_config(name, config, interpret=interpret, config_name=config_name, **kwargs)

    for config_opt in config_opts + opts:
        key, value = config_opt.split('=', 1)
        result.user_set(key, value)

    return result





def get_configs(config_str=None, ini_configs=[], ini_configs_dict={}, interpret=True):
  result = []

  for config in shlex.split(config_str.replace(';', ' ')):

    full_config = config

    if config.find('@') != -1:
      config_name, config = config.split('@')

    for item in shlex.split(config.replace(':', ' ')):

      if item.find('config_file') != -1:
        key, file = item.split('=')
        config_tree = get_config(file, full_config, ini_configs=ini_configs, ini_configs_dict=ini_configs_dict, interpret=interpret)
        result.append(config_tree)

      else:
        key, file = item.split('=')
        config_tree.user_set(key, file)

  return result

def get_configs_from_env(path=None, interpret=True):
  config_str = os.environ.get('PULP_CURRENT_CONFIGS')
  if config_str is None:
    config_str = os.environ.get('PULP_CURRENT_CONFIG')
    if config_str is None:
      config_str = os.environ.get('PULP_CONFIGS')

  if config_str is None:
    raise Exception("Configurations must e given either through PULP_CURRENT_CONFIG or PULP_CONFIGS")

  configs = get_configs(config_str, interpret=interpret)

  args = os.environ.get('PULP_CURRENT_CONFIG_ARGS')
  if args is not None:
    for config in configs:
      for arg in shlex.split(args.replace(':', ' ')):
        key, value = arg.split('=', 1)
        config.user_set(key, value)

  return configs
