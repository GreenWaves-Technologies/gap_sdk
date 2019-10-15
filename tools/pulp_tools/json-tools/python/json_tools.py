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

#
# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
#

import json
from collections import OrderedDict
import os
import sys
import io

def get_paths(path=None, paths=None):

  env_path = os.environ.get('BUILDER_CONFIGS_PATH')
  if env_path is None:
      env_path = os.environ.get('SDK_CONFIGS_PATH')
      if env_path is None:
        env_path = os.environ.get('PULP_CONFIGS_PATH')

  all_paths = env_path.split(':')
  if paths is not None:
    all_paths = all_paths + paths
  if path is not None:
    all_paths = all_paths + [path]
  return all_paths


def is_string(config):
    if sys.version_info >= (3,0,0):
        return type(config) == str
    else:
        return type(config) == str or isinstance(config, unicode)


def find_config(config, paths):

  for path in paths:
    full_path = os.path.join(path, config)
    if os.path.exists(full_path):
      return full_path

  return None



def import_config(config, interpret=False, path=None):
    return config_object(config, interpret, path)


def get_config_file(file_path, interpret=False, find=False, path=None):

    if find:
        paths = get_paths(path=path)
        new_file_path = find_config(file_path, paths)
        if new_file_path is None:
            raise Exception("Didn't find JSON file from any specified path (file: %s, paths: %s)" % (file_path, ":".join(paths)))
        file_path = new_file_path

    with io.open(file_path, 'r', encoding='utf-8') as fd:
        config_dict = json.load(fd, object_pairs_hook=OrderedDict)
        return config_dict

def import_config_from_file(file_path, interpret=False, find=False, path=None):
    config_dict = get_config_file(file_path, interpret, find, path)
    return import_config(config_dict, interpret, path=os.path.dirname(file_path))


class config(object):

    def get_str(self, name=None):
        if name is None:
            return None
        else:
            return self.get_child_str(name)

    def set_from_list(self, name_list, value):
        pass

    def get_child_str(self, name):
        config = self.get_child(name)
        if config is None:
            return None
        return config.get()

    def browse(self, callback, *kargs, **kwargs):
        pass

    def get_int(self):
        pass

    def get(self, name):
        pass

    def get_child(self, name):
        pass

    def get_bool(self):
        return False

    def get_child_bool(self, name):
        config = self.get(name)
        if config is None:
            return None
        return config.get_bool()

    def get_child_int(self, name):
        config = self.get(name)
        if config is None:
            return None
        return config.get_int()

    def get_child_dict(self, name):
        config = self.get(name)
        if config is None:
            return None
        return config.get_dict()

    def get_elem(self, index):
        pass

    def get_size(self, index):
        pass

    def get_from_list(self, name_list):
        pass

    def merge(self, new_value):
        return new_value

    def get_tree(self, config, interpret=False, path=None):
        if type(config) == list:
            return config_array(config)
        elif type(config) == dict or type(config) == OrderedDict:
            return config_object(config, interpret, path)
        elif is_string(config):
            return config_string(config)
        elif type(config) == bool:
            return config_bool(config)
        else:
            return config_number(config)

    def get_string(self):
        return self.dump_to_string()

    def dump_to_string(self, indent=2):
        if indent is not None:
            return json.dumps(self.get_dict(), indent=indent)
        else:
            return json.dumps(self.get_dict())

    def dump_help(self, name=None, root=None):
        pass


class config_object(config):

    def __init__(self, config, interpret=False, path=None):
        self.items = OrderedDict()

        for key, value in config.items():

            if interpret and (key == 'includes' or key == 'includes2' or key == 'include'):

                if key == 'include':
                    value = [value]

                for include in value:
                    self.merge(import_config_from_file(include, interpret=interpret, find=True, path=path))

            else:
                if self.items.get(key) is not None:
                    self.items[key] = self.items[key].merge(self.get_tree(value, interpret, path))
                else:
                    self.items[key] = self.get_tree(value, interpret, path)

    def browse(self, callback, *kargs, **kwargs):
        callback(self, *kargs, **kwargs)
        for key, value in self.items.items():
            value.browse(callback, *kargs, **kwargs)

    def get_str(self, name):

        config = self.get(name)
        if config is None:
            return None
        return config.get()

    def get_bool(self, name):

        config = self.get(name)
        if config is None:
            return None
        return config.get()

    def get_int(self, name):

        config = self.get(name)
        if config is None:
            return None
        return config.get()

    def merge(self, new_value):
        for key, value in new_value.items.items():
            if self.items.get(key) is None:
                self.items[key] = value
            else:
                self.items[key] = self.items[key].merge(value)

        return self

    def get_from_list(self, name_list):
        if len(name_list) == 0:
            return self

        result = None
        name_pos = 0

        name = None
        for item in name_list:
            if item != "*" and item != "**":
                name = item
                break
            name_pos += 1

        for key, value in self.items.items():
            if name == key:
                result = value.get_from_list(name_list[name_pos + 1:])
                if name_pos == 0 or result is not None:
                    return result
            elif name_list[0] == "*":
                result = value.get_from_list(name_list[1:])
                if result is not None:
                    return result
            elif name_list[0] == "**":
                result = value.get_from_list(name_list)
                if result is not None:
                    return result

        return result

    def set_from_list(self, name_list, value):

        if len(name_list) == 1:
            key = name_list.pop(0)
            prev_value = self.items.get(key)
            new_value = self.get_tree(value)
            if prev_value is not None:
                self.items[key] = prev_value.merge(new_value)
            else:
                self.items[key] = new_value
        else:
            key = name_list[0]

            if self.items.get(key) is not None:
                name_list.pop(0)
                self.items[key].set_from_list(name_list, value)
            elif key in ['**', '*']:
                next_key = name_list[1]
                if self.items.get(next_key) is not None:
                    name_list.pop(0)
                    name_list.pop(0)
                    self.items[next_key].set_from_list(name_list, value)
                else:
                    if key == '*':
                        name_list.pop(0)

                    for name, obj in self.items.items():
                        obj.set_from_list(name_list.copy(), value)

            else:
                name_list.pop(0)
                self.items[key] = config_object(OrderedDict())
                self.items[key].set_from_list(name_list, value)

    def get_child(self, name):
        return self.get(name)


    def get(self, name):
        return self.get_from_list(name.split('/'))

    def set(self, name, value):
        self.set_from_list(name.split('/'), value)

    def user_set(self, key, value):

        # In case the path is not absolute and does not contain *
        # add it implicitly to let the user specify a property
        # without having to put **/ at the beginning
        if key[0] != '*' and key[0] != '/' and key.find('/') != -1:
            key = '**/' + key

        if key[0] == '/':
            key = key[1:]

        if key.find('/') != -1:
            root, prop = key.rsplit('/', 1)
            if self.get(root) is not None:
                self.get(root).set(prop, value)
            else:
                self.set(key, value)
        else:
            self.set(key, value)

    def get_dict(self, serialize=False):
        if serialize:
            result = {}
        else:
            result = OrderedDict()
        for key,value in self.items.items():
            result[key] = value.get_dict(serialize=serialize)
        return result

    def get_items(self):
        return self.items

    def dump_help(self, name=None, root=None):
        prop_help = self.items.get('help')
        if prop_help is not None:
          print ('')
          print ('  ' + name + ' group:')
          for key in prop_help.get_dict().keys():
            full_name = key
            if name is not None:
              full_name = '%s/%s' % (name, key)
            print ('    %-40s %s' % (full_name, prop_help.get_str(key)))

        for key, prop in self.items.items():
          full_name = key
          if name is not None:
            full_name = '%s/%s' % (name, key)
          prop.dump_help(name=full_name)

class config_array(config):

    def __init__(self, config):
        self.elems = []
        for elem in config:
            self.elems.append(self.get_tree(elem))

    def get_from_list(self, name_list):
        if len(name_list) == 0:
            return self
        return None

    def set_from_list(self, name_list, value):
        if len(name_list) == 0:
            self.merge(self.get_tree(value))
        else:
            for elem in self.elems:
                elem.set_from_list(name_list, value)

    def browse(self, callback, *kargs, **kwargs):
        for elem in self.elems:
            elem.browse(callback, *kargs, **kwargs)

    def get_size(self):
        return len(self.elems)

    def get_elem(self, index):
        return self.elems[index]

    def get_dict(self, serialize=False):
        result = []
        for elem in self.elems:
            result.append(elem.get_dict(serialize=serialize))
        return result

    def get(self):
        return self.elems

    def merge(self, new_value):
        if type(new_value) != config_array:
            new_value = config_array([new_value.get_dict()])

        for elem in new_value.elems:
            self.elems.append(elem)

        return self

    def dump_help(self, name=None, root=None):
        for elem in self.elems:
          elem.dump_help(name=name)



class config_string(config):

    def __init__(self, config):
        self.value = config

    def get_from_list(self, name_list):
        if len(name_list) == 0:
            return self
        return None

    def get(self):
        return self.value

    def get_bool(self):
        return self.value == 'True' or self.value == 'true'

    def get_dict(self, serialize=False):
        return self.value

    def get_int(self):
        return int(self.get(), 0)

    def set_from_list(self, name_list, value):
        if len(name_list) == 0:
            self.value = self.get_tree(value)

class config_number(config):

    def __init__(self, config):
        self.value = config

    def get_from_list(self, name_list):
        if len(name_list) == 0:
            return self
        return None

    def get(self):
        return self.value

    def get_dict(self, serialize=False):
        return self.value

    def get_int(self):
        return self.get()

    def set_from_list(self, name_list, value):
        if len(name_list) == 0:
            self.value = self.get_tree(value)


class config_bool(config):

    def __init__(self, config):
        self.value = config

    def get_from_list(self, name_list):
        if len(name_list) == 0:
            return self
        return None

    def get(self):
        return self.value

    def get_dict(self, serialize=False):
        return self.value

    def get_bool(self):
        return self.get()

    def set_from_list(self, name_list, value):
        if len(name_list) == 0:
            self.value = self.get_tree(value)
