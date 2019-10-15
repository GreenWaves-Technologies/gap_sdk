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

import shlex
import os
import json_tools as js



def get_configs(config_str=None):
  result = []

  for config in shlex.split(config_str.replace(';', ' ')):

    if config.find('@') != -1:
      config_name, config = config.split('@')

    if config.find('config_file') != -1:
      key, file = config.split('=')
      config_tree = js.import_config_from_file(file, interpret=True)
      result.append(config_tree)

  return result

def get_configs_from_env():
  config_str = os.environ.get('PULP_CURRENT_CONFIG')
  if config_str is None:
    config_str = os.environ.get('PULP_CONFIGS')

  if config_str is None:
    raise Exception("Configurations must e given either through PULP_CURRENT_CONFIG or PULP_CONFIGS")

  return get_configs(config_str)
