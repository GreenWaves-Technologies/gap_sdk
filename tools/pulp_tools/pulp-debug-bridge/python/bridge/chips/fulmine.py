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

from bridge.default_debug_bridge import *


class fulmine_debug_bridge(debug_bridge):

    def __init__(self, config, binaries=[], verbose=0):
        super(fulmine_debug_bridge, self).__init__(config=config, binaries=binaries, verbose=verbose)

        # We have to use the soc tap on fulmine
        if config.get('**/debug_bridge/cable/tap') is not None:
            config.get('**/debug_bridge/cable').set('tap', 1)

    def start(self):
        return self.write_32(0x10200008, 0x1)
