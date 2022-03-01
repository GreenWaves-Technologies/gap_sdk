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

            sfu_signals = []

            for i in range(0, 8):
                sfu_signals.append(['mem_in_%d'%i, 'sfu0.mem_in_%d.busy'%i, '[7:0]'])

            for i in range(0, 8):
                sfu_signals.append(['stream_in_%d'%i, 'sfu0.stream_in_%d.busy'%i, '[7:0]'])

            for i in range(0, 12):
                sfu_signals.append(['pdm_in_%d'%i, 'sfu0.pdm_in_%d.busy'%i, '[7:0]'])

            for i in range(0, 8):
                sfu_signals.append(['mem_out_%d'%i, 'sfu0.mem_out_%d.busy'%i, '[7:0]'])

            for i in range(0, 8):
                sfu_signals.append(['stream_out_%d'%i, 'sfu0.stream_out_%d.busy'%i, '[7:0]'])

            for i in range(0, 3):
                sfu_signals.append(['pdm_out_%d'%i, 'sfu0.pdm_out_%d.busy'%i, '[7:0]'])

            for i in range(0, 6):
                sfu_signals.append(['limiter_%d'%i, 'sfu0.limiter_%d.busy'%i, '[7:0]'])

            for i in range(0, 4):
                sfu_signals.append(['gfu_%d'%i, 'sfu0.gfu_%d.busy'%i, '[7:0]'])

            for i in range(0, 4):
                sfu_signals.append(['mixer_%d'%i, 'sfu0.mixer_%d.busy'%i, '[7:0]'])

            for i in range(0, 4):
                sfu_signals.append(['splitter_%d'%i, 'sfu0.splitter_%d.busy'%i, '[7:0]'])

            for i in range(0, 4):
                sfu_signals.append(['pipe_%d'%i, 'sfu0.pipe_%d.busy'%i, '[7:0]'])

            for i in range(0, 4):
                sfu_signals.append(['resampler_%d'%i, 'sfu0.resampler_%d.busy'%i, '[7:0]'])

            for i in range(0, 4):
                sfu_signals.append(['polyphase_%d'%i, 'sfu0.polyphase_%d.busy'%i, '[7:0]'])

            for i in range(0, 3):
                sfu_signals.append(['asrc_%d'%i, 'sfu0.asrc_%d.busy'%i, '[7:0]'])


            tree.add_vector(self, 'sfu', traces=sfu_signals, map_file=map_file, tag='overview')

            ffc_signals = []
            for i in range(0, 4):
                ffc_signals.append(['ffc_%d'%i, 'ffc%d.busy'%i, '[7:0]'])
            tree.add_vector(self, 'ffc', traces=ffc_signals, map_file=map_file, tag='overview')

    def gen_gtkw_conf(self, tree, traces):
        if tree.get_view() == 'overview':
            self.vcd_group(self, skip=True)
        else:
            self.vcd_group(self, skip=False)
