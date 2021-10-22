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
from ips.cache.cache import Cache
from ips.interco.interleaver import Interleaver
import math

class Hierarchical_cache(st.Component):

    def __init__(self, parent, name, config):

        super(Hierarchical_cache, self).__init__(parent, name)

        #
        # Properties
        #

        self.add_properties(config)

        nb_cores = self.get_property('nb_cores')
        has_cc = self.get_property('has_cc')
        nb_l1_banks = self.get_property('nb_l1_banks')
        nb_l1_banks_log2 = int(math.log(nb_l1_banks, 2.0))
        nb_pes = nb_cores - 1 if has_cc else nb_cores
        l1_line_size_bits = self.get_property('l1/line_size_bits', int)
        l1_cache_line_size = 1 << l1_line_size_bits


        #
        # Components
        #

        # L0 caches
        l0_caches = []
        for i in range(0, nb_pes):
            l0_caches.append(Cache(self, 'l0_bank%d' % i, **self.get_property('l0')))

        if has_cc:
            l0_caches.append(Cache(self, 'l0_bank%d' % (nb_cores-1), **self.get_property('l0_cc')))
        
        # L1 caches
        l1_caches = []
        for i in range(0, nb_l1_banks):
            l1_caches.append(Cache(self, 'l1_bank%d' % i, **self.get_property('l1'), refill_shift=nb_l1_banks_log2, add_offset=i*l1_cache_line_size))

        # L1 interleaver
        interleaver = Interleaver(self, 'interleaver', nb_slaves=nb_l1_banks, interleaving_bits=l1_line_size_bits)


        #
        # Bindings
        #

        # L0 caches
        for i in range(0, nb_cores):
            self.bind(self, 'input_%d' % i, l0_caches[i], 'input')
            self.bind(l0_caches[i], 'refill', interleaver, 'input')
            self.bind(self, 'enable', l0_caches[i], 'enable')
            self.bind(self, 'flush', l0_caches[i], 'flush')

        # L1 cache
        for i in range(0, nb_l1_banks):
            self.bind(l1_caches[i], 'refill', self, 'refill')
            self.bind(self, 'enable', l1_caches[i], 'enable')
            self.bind(self, 'flush', l1_caches[i], 'flush')

        # Interleaver
        for i in range(0, nb_l1_banks):
            self.bind(interleaver, 'out_%d' % i, l1_caches[i], 'input')

