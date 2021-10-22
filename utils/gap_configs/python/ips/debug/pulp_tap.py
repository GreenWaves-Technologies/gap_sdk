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

class Pulp_tap(st.Component):

    def __init__(self, parent, name, confreg_instr=0, confreg_length=0, idcode=0):

        super(Pulp_tap, self).__init__(parent, name)

        self.add_properties({
            'vp_component': 'pulp.adv_dbg_unit.adv_dbg_unit_impl',
            'confreg_instr': confreg_instr,
            'confreg_length': confreg_length,
            'idcode': idcode,
        })