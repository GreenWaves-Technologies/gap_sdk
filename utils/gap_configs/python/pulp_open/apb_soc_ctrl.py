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

class Apb_soc_ctrl(st.Component):

    def __init__(self, parent, name, soc):
        super(Apb_soc_ctrl, self).__init__(parent, name)

        self.add_properties({
            'vp_component':  'pulp.chips/pulp/apb_soc_impl',
            'cluster_power_event': soc.get_property('soc_events/soc_evt_cluster_pok'),
            'cluster_clock_gate_event': soc.get_property('soc_events/soc_evt_cluster_cg_ok')
        })

        self.add_properties(soc.get_property('peripherals/apb_soc_ctrl/config'))
        