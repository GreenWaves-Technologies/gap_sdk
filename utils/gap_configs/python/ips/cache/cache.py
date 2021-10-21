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

class Cache(st.Component):
    """
    Cache

    Attributes
    ----------
    size : int
        The size of the memory
    
    """

    def __init__(self, parent, name, nb_sets_bits, nb_ways_bits, line_size_bits, refill_latency=0, refill_shift=0, nb_ports=1, add_offset=0):

        super(Cache, self).__init__(parent, name)

        self.add_properties({
            'vp_component': 'cache.cache_impl',
            'nb_sets_bits': nb_sets_bits,
            'nb_ways_bits': nb_ways_bits,
            'line_size_bits': line_size_bits,
            'nb_ports': nb_ports,
            'refill_latency': refill_latency,
            'add_offset': add_offset,
            'refill_shift': refill_shift
        })


    def gen_gtkw2(self, tree, comp_traces):


        if tree.get_view() != 'overview':

            tree.add_trace(self, 'refill', 'refill', '[31:0]', tag='icache')

            for way in range(0, 1<<self.get_property('nb_ways_bits')):
                tree.begin_group('way_%d' % way)

                for line in range(0, 1<<self.get_property('nb_sets_bits')):
                    tree.add_trace(self, 'tag_%d' % line, 'set_%d.line_%d' % (way, line), '[31:0]', tag='icache')

                tree.end_group('way_%d' % way)

