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

import generators.v1.system_gen as system_gen
from generators.v1.comp_gen import *
import collections
import json_tools as js



def get_config(tp):

  chip              = tp.get_child_str('chip')

  config = js.import_config({'includes': [ "defaults.json" ] })

  config.merge(Config(
    config=system_gen.get_config(tp)
  ).get_js_config())

  config.merge(js.import_config({'includes2': [ "chips/%s/defaults.json" % (chip) ] }))


  return config
