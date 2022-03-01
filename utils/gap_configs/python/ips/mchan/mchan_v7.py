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

class Mchan(st.Component):

    def __init__(self, parent, name, nb_channels=0, core_queue_depth=2, global_queue_depth=8, is_64=False, max_nb_ext_read_req=8,
            max_nb_ext_write_req=8, max_burst_length=256, nb_loc_ports=4, tcdm_addr_width=20, power_models_file=None):
        super(Mchan, self).__init__(parent, name)

        self.vcd_group(self, skip=True)

        self.add_properties({
            'vp_component': 'pulp.mchan.mchan_v7_impl',
            'nb_channels': nb_channels,
            'core_queue_depth': core_queue_depth,
            'global_queue_depth': global_queue_depth,
            'is_64': is_64,
            'max_nb_ext_read_req': max_nb_ext_read_req,
            'max_nb_ext_write_req': max_nb_ext_write_req,
            'max_burst_length': max_burst_length,
            'nb_loc_ports': nb_loc_ports,
            'tcdm_addr_width': tcdm_addr_width,
        })

        if power_models_file is not None:
            self.add_property('power_models', self.load_property_file(power_models_file))


    def gen_gtkw(self, tree, traces):

        if tree.get_view() == 'overview':
            map_file = tree.new_map_file(self, 'dma_state')
            map_file.add_value(1, 'CadetBlue', 'ACTIVE')

            dma_signals = []

            for i in range(0, 16):
                dma_signals.append(['channel_%d' % i, 'channel_%d' % i, '[7:0]'])

            tree.add_vector(self, self.name, traces=dma_signals, map_file=map_file, tag='overview')
