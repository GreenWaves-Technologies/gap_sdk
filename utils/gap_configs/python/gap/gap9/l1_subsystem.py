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
from ips.memory.memory import Memory
from ips.interco.router import Router
from ips.interco.converter import Converter
from ips.cluster.l1_interleaver import L1_interleaver
import math


class L1_subsystem(st.Component):
    """
    Cluster L1 subsystem (memory banks + interconnects)

    Attributes
    ----------
    cluster: Cluster
        The cluster class.
    
    """

    def __init__(self, parent, name, cluster):
        super(L1_subsystem, self).__init__(parent, name)

        #
        # Properties
        #

        nb_pe = cluster.get_property('nb_pe', int)
        l1_banking_factor = cluster.get_property('l1/banking_factor')
        nb_l1_banks = 1<<int(math.log(nb_pe * l1_banking_factor, 2.0))
        l1_bank_size = int(cluster.get_property('l1/mapping/size', int) / nb_l1_banks)
        l1_interleaver_nb_masters = nb_pe + 4 + 1 # 1 port per PE + 4 for DMA + 1 for NE16
        first_external_pcer = cluster.get_property('iss_config/first_external_pcer')
        power_models = cluster.get_property('l1/power_models')

        #
        # Components
        #

        # Memory banks
        l1_banks = []
        for i in range(0, nb_l1_banks):
            mem = Memory(self, 'bank%d' % i, size=l1_bank_size, power_trigger=True if i == 0 else False)
            l1_banks.append(mem)
            mem.add_properties(self.load_property_file(power_models))

        # Per-PE interconnects
        pe_icos = []
        for i in range(0, nb_pe):
            pe_icos.append(Router(self, 'pe%d_ico' % i))

        # L1 interleaver
        interleaver = L1_interleaver(self, 'interleaver', nb_slaves=nb_l1_banks, nb_masters=l1_interleaver_nb_masters, interleaving_bits=2)

        # EXT2LOC
        ext2loc = Converter(self, 'ext2loc', output_width=4, output_align=4)

        # EXT2LOC test-and-set
        ext2loc_ts = Converter(self, 'ext2loc_ts', output_width=4, output_align=4)


        #
        # Bindings
        #

        # Per-PE interconnects
        for i in range(0, nb_pe):
            self.bind(self, 'data_pe_%d' % i, pe_icos[i], 'input')

            pe_icos[i].add_mapping('l1', **cluster._reloc_mapping(cluster.get_property('l1/mapping')), id=0)
            self.bind(pe_icos[i], 'l1', interleaver, 'in_%d' % i)
            
            pe_icos[i].add_mapping('l1_alias', **cluster._reloc_mapping_alias(cluster.get_property('l1/mapping')), id=0)
            self.bind(pe_icos[i], 'l1_alias', interleaver, 'in_%d' % i)
            
            pe_icos[i].add_mapping('l1_ts', **cluster._reloc_mapping(cluster.get_property('l1/ts_mapping')), id=0)
            self.bind(pe_icos[i], 'l1_ts', interleaver, 'ts_in_%d' % i)
            
            pe_icos[i].add_mapping('l1_ts_alias', **cluster._reloc_mapping_alias(cluster.get_property('l1/ts_mapping')), id=0)
            self.bind(pe_icos[i], 'l1_ts_alias', interleaver, 'ts_in_%d' % i)
            
            pe_icos[i].add_mapping('dma', **cluster._reloc_mapping(cluster.get_property('demux_peripherals/dma/mapping')), id=1)
            self.bind(pe_icos[i], 'dma', self, 'dma_%d' % i)
            
            pe_icos[i].add_mapping('dma_alias', **cluster._reloc_mapping_alias(cluster.get_property('demux_peripherals/dma/mapping')), id=1)
            self.bind(pe_icos[i], 'dma_alias', self, 'dma_%d' % i)

            pe_icos[i].add_mapping('event_unit', **cluster._reloc_mapping(cluster.get_property('demux_peripherals/event_unit/mapping')), id=1)
            self.bind(pe_icos[i], 'event_unit', self, 'event_unit_%d' % i)
            
            pe_icos[i].add_mapping('event_unit_alias', **cluster._reloc_mapping_alias(cluster.get_property('demux_peripherals/event_unit/mapping')), id=1)
            self.bind(pe_icos[i], 'event_unit_alias', self, 'event_unit_%d' % i)

            pe_icos[i].add_mapping('event_unit_alias', **cluster._reloc_mapping_alias(cluster.get_property('demux_peripherals/event_unit/mapping')), id=1)
            self.bind(pe_icos[i], 'event_unit_alias', self, 'event_unit_%d' % i)

            pe_icos[i].add_mapping('cluster_ico', id=1)
            self.bind(pe_icos[i], 'cluster_ico', self, 'cluster_ico')

            self.bind(self, 'ext_counter_%d[%d]' % (i, first_external_pcer + 0), pe_icos[i], 'nb_read[1]')
            self.bind(self, 'ext_counter_%d[%d]' % (i, first_external_pcer + 1), pe_icos[i], 'nb_write[1]')
            self.bind(self, 'ext_counter_%d[%d]' % (i, first_external_pcer + 2), pe_icos[i], 'read_stalls[1]')
            self.bind(self, 'ext_counter_%d[%d]' % (i, first_external_pcer + 3), pe_icos[i], 'write_stalls[1]')
            self.bind(self, 'ext_counter_%d[%d]' % (i, first_external_pcer + 4), pe_icos[i], 'stalls[0]')

        # L1 interleaver
        for i in range(0, nb_l1_banks):
            self.bind(interleaver, 'out_%d' % i, l1_banks[i], 'input')

        self.bind(self, 'ne16_in', interleaver, 'in_%d' % (nb_pe + 4))

        for i in range(0, 4):
            self.bind(self, 'dma_in_%d' % i, interleaver, 'in_%d' % (nb_pe + i))

        # EXT2LOC
        self.bind(self, 'ext2loc', ext2loc, 'input')
        self.bind(ext2loc, 'out', interleaver, 'in_%d' % nb_pe)

        # EXT2LOC test-and-set
        self.bind(self, 'ext2loc_ts', ext2loc_ts, 'input')
        self.bind(ext2loc_ts, 'out', interleaver, 'ts_in_%d' % nb_pe)
