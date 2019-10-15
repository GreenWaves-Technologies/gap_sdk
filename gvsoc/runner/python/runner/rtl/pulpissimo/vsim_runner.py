#
# Copyright (C) 2018 ETH Zurich, University of Bologna
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

import runner.rtl.vsim_runner as runner

class Runner(runner.Runner):

    def run(self):

        self.set_param('ENTRY_POINT', '0x%x' % self.get_stim().get_entry())

        return super(Runner, self).run()
