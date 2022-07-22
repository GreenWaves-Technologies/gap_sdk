#
# Copyright (C) 2020 GreenWaves Technologies
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

import gsystree as st

class Pmu(st.Component):

    def __init__(self, parent, name, config_file=None):

        super(Pmu, self).__init__(parent, name)

        self.add_properties({
            'vp_component': 'pulp.pmu.pmu_v4_impl',
        })

        if config_file is not None:
            self.add_properties(self.load_property_file(config_file))