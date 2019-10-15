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
 
import vp_core as vp

class component(vp.component):

    def __init__(self, name, config, debug, parent=None):

        js_config = self.get_json_config(config)

        iss_class = js_config.get_child_str('iss_class')
        if iss_class is None:
          iss_class = 'iss_riscy'

        setattr(self, 'implementation', 'cpu/iss/%s' % iss_class)

        super(component, self).__init__(name, config, debug, parent)
