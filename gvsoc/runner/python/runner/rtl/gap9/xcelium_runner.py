# 
# Copyright (c) 2017 GreenWaves Technologies SAS
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
# Authors: Florent Rotenberg, GWT (florent.rotenberg@greenwaves-technologies.com)
#

import runner.rtl.xcelium_runner as runner

class Runner(runner.Runner):

    def __init__(self, config, js_config):

        super(Runner, self).__init__(config, js_config)
        