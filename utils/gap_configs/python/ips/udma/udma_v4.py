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

class Udma(st.Component):
    def __init__(self, parent, name, config_file):

        super(Udma, self).__init__(parent, name)

        self.vcd_group(self, skip=True)

        self.add_properties(self.load_property_file(config_file))
        

    def gen_gtkw(self, tree, traces):

        if tree.get_view() == 'overview':
            map_file = tree.new_map_file(self, 'udma_state')
            map_file.add_value(1, 'CadetBlue', 'ACTIVE')

            udma_signals = [
                ['hyper0', 'hyper0.active', '[7:0]'],
                ['hyper1', 'hyper1.active', '[7:0]']
            ]
            tree.add_vector(self, self.name, traces=udma_signals, map_file=map_file, tag='overview')
