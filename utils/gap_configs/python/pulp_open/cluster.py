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
from ips.iss.iss import Iss
from ips.cache.hierarchical_cache import Hierarchical_cache
from pulp_open.l1_subsystem import L1_subsystem
from ips.event_unit.event_unit_v3 import Event_unit
from ips.interco.router import Router
from ips.mchan.mchan_v7 import Mchan
from ips.timer.timer_v2 import Timer
from ips.cluster.cluster_control_v2 import Cluster_control
from ips.icache_ctrl.icache_ctrl_v2 import Icache_ctrl


def get_cluster_name(cid: int):
    """
    Return cluster name from cluster ID

    Parameters
    ----------
    cid : int
        Cluster ID

    Returns
    -------
    string
        The cluster name
    
    """

    if cid == 0:
        return 'cluster'
    else:
        return 'cluster_%d' % (cid)


class Cluster(st.Component):
    """
    Cluster subsystem

    Attributes
    ----------
    cid : int
        Cluster ID
    
    """

    def __init__(self, parent, name, config_file, cid: int=0):
        super(Cluster, self).__init__(parent, name)

        #
        # Properties
        #

        self.add_properties(self.load_property_file(config_file))

        nb_pe               = self.get_property('nb_pe', int)
        cluster_size        = self.get_property('mapping/size', int)
        self.cluster_offset = cluster_size * cid
        self.cluster_base   = self.get_property('mapping/base', int)
        self.cluster_alias  = self.get_property('alias', int)
        dma_irq_0           = self.get_property('pe/irq').index('dma_0')
        dma_irq_1           = self.get_property('pe/irq').index('dma_1')
        dma_irq_ext         = self.get_property('pe/irq').index('dma_ext')
        timer_irq_0         = self.get_property('pe/irq').index('timer_0')
        timer_irq_1         = self.get_property('pe/irq').index('timer_1')
        first_external_pcer = self.get_property('iss_config/first_external_pcer')


        #
        # Components
        #

        # L1 subsystem
        l1 = L1_subsystem(self, 'l1', self)

        # Cores
        pes = []
        for i in range(0, nb_pe):
            pes.append(Iss(self, 'pe%d' % i, **self.get_property('iss_config'), cluster_id=cid, core_id=i))

        # Icache
        icache = Hierarchical_cache(self, 'icache', self.get_property('icache/config'))

        # Event unit
        event_unit = Event_unit(self, 'event_unit', self.get_property('peripherals/event_unit/config'))

        # Cluster interconnect
        cluster_ico = Router(self, 'cluster_ico', latency=2)

        # Periph interconnect
        periph_ico = Router(self, 'periph_ico')

        # Demux periph interconnect
        demux_periph_ico = Router(self, 'demux_periph_ico')

        # MCHAN
        mchan = Mchan(self, 'dma', nb_channels=nb_pe+1)

        # Timer
        timer = Timer(self, 'timer')

        # Cluster control
        cluster_control = Cluster_control(self, 'cluster_ctrl', nb_core=nb_pe)

        # Icache controller
        icache_ctrl = Icache_ctrl(self, 'icache_ctrl')
    

        #
        # Bindings
        #

        # L1 subsystem
        for i in range(0, nb_pe):
            self.bind(l1, 'dma_%d' % i, mchan, 'in_%d' % i)
            self.bind(l1, 'dma_alias_%d' % i, mchan, 'in_%d' % i)
            self.bind(l1, 'event_unit_%d' % i, event_unit, 'demux_in_%d' % i)
            self.bind(l1, 'event_unit_alias_%d' % i, event_unit, 'demux_in_%d' % i)

        self.bind(l1, 'cluster_ico', cluster_ico, 'input')

        # Cores
        for i in range(0, nb_pe):
            self.bind(pes[i], 'data', l1, 'data_pe_%d' % i)
            self.bind(pes[i], 'fetch', icache, 'input_%d' % i)
            self.bind(pes[i], 'irq_ack', event_unit, 'irq_ack_%d' % i)
            self.bind(self, 'halt_pe%d' % i, pes[i], 'halt')

            for j in range(first_external_pcer, first_external_pcer+5):
                self.bind(pes[i], 'ext_counter[%d]' % j, l1, 'ext_counter_%d[%d]' % (i, j))

        # Icache
        self.bind(icache, 'refill', cluster_ico, 'input')

        # Event unit
        for i in range(0, nb_pe):
            self.bind(event_unit, 'clock_%d' % i, pes[i], 'clock')
            self.bind(event_unit, 'irq_req_%d' % i, pes[i], 'irq_req')

        # Cluster interconnect
        self.bind(self, 'input', cluster_ico, 'input')

        cluster_ico.add_mapping('error', **self._reloc_mapping(self.get_property('mapping')))

        cluster_ico.add_mapping('soc')
        self.bind(cluster_ico, 'soc', self, 'soc')

        cluster_ico.add_mapping('l1', **self._reloc_mapping(self.get_property('l1/mapping')))
        self.bind(cluster_ico, 'l1', l1, 'ext2loc')

        cluster_ico.add_mapping('l1_ts', **self._reloc_mapping(self.get_property('l1/ts_mapping')))
        self.bind(cluster_ico, 'l1_ts', l1, 'ext2loc_ts')

        cluster_ico.add_mapping('periph_ico', **self.get_property('peripherals/mapping'))
        self.bind(cluster_ico, 'periph_ico', periph_ico, 'input')

        cluster_ico.add_mapping('periph_ico_alias', **self.get_property('peripherals/alias'), add_offset=int(self.get_property('peripherals/mapping/base'), 0) - int(self.get_property('peripherals/alias/base'), 0))
        self.bind(cluster_ico, 'periph_ico_alias', periph_ico, 'input')

        # Periph interconnect
        periph_ico.add_mapping('error', **self._reloc_mapping(self.get_property('mapping')))

        periph_ico.add_mapping('cluster_ico')
        self.bind(periph_ico, 'cluster_ico', cluster_ico, 'input')

        periph_ico.add_mapping('event_unit', **self._reloc_mapping(self.get_property('peripherals/event_unit/mapping')))
        self.bind(periph_ico, 'event_unit', event_unit, 'input')

        periph_ico.add_mapping('cluster_ctrl', **self._reloc_mapping(self.get_property('peripherals/cluster_ctrl/mapping')))
        self.bind(periph_ico, 'cluster_ctrl', cluster_control, 'input')

        periph_ico.add_mapping('icache_ctrl', **self._reloc_mapping(self.get_property('peripherals/icache_ctrl/mapping')))
        self.bind(periph_ico, 'icache_ctrl', icache_ctrl, 'input')

        periph_ico.add_mapping('timer', **self._reloc_mapping(self.get_property('peripherals/timer/mapping')))
        self.bind(periph_ico, 'timer', timer, 'input')

        periph_ico.add_mapping('dma', **self._reloc_mapping(self.get_property('peripherals/dma/mapping')))
        self.bind(periph_ico, 'dma', mchan, 'in_%d' % nb_pe)

        size = int(self.get_property('peripherals/dbg_unit/size'), 0)
        base = int(self.get_property('peripherals/dbg_unit/base'), 0)
        for i in range(0, nb_pe):
            pe_base = base + cid*cluster_size + size * i
            periph_ico.add_mapping('dbg_unit_%d' % i, base=pe_base, size=size, remove_offset=pe_base)
            self.bind(periph_ico, 'dbg_unit_%d' % i, pes[i], 'dbg_unit')

        # MCHAN
        self.bind(mchan, 'ext_irq_itf', self, 'dma_irq')
        self.bind(mchan, 'ext_itf', cluster_ico, 'input')

        for i in range(0, 4):
            self.bind(mchan, 'loc_itf_%d' % i, l1, 'dma_in_%d' % i)

        for i in range(0, nb_pe):
            self.bind(mchan, 'event_itf_%d' % i, event_unit, 'in_event_%d_pe_%d' % (dma_irq_0, i))
            self.bind(mchan, 'irq_itf_%d' % i, event_unit, 'in_event_%d_pe_%d' % (dma_irq_1, i))
            self.bind(mchan, 'ext_irq_itf', event_unit, 'in_event_%d_pe_%d' % (dma_irq_ext, i))
    
        # Timer
        self.bind(self, 'ref_clock', timer, 'ref_clock')
        for i in range(0, nb_pe):
            self.bind(timer, 'irq_itf_0', event_unit, 'in_event_%d_pe_%d' % (timer_irq_0, i))
            self.bind(timer, 'irq_itf_1', event_unit, 'in_event_%d_pe_%d' % (timer_irq_1, i))

        # Cluster control
        for i in range(0, nb_pe):
            self.bind(cluster_control, 'bootaddr_%d' % i, pes[i], 'bootaddr')
            self.bind(cluster_control, 'fetchen_%d' % i, pes[i], 'fetchen')
            self.bind(cluster_control, 'halt_%d' % i, pes[i], 'halt')
            self.bind(pes[i], 'halt_status', cluster_control, 'core_halt_%d' % i)

        # Icache controller
        self.bind(icache_ctrl, 'enable', icache, 'enable')
        self.bind(icache_ctrl, 'flush', icache, 'flush')
        self.bind(icache_ctrl, 'flush_line', icache, 'flush_line')
        self.bind(icache_ctrl, 'flush_line_addr', icache, 'flush_line_addr')

        for i in range(0, nb_pe):
            self.bind(icache_ctrl, 'flush', pes[i], 'flush_cache')


    def _reloc_mapping(self, mapping):
        """Relocate a mapping to this cluster, depending on the cluster ID.

        The offset of the cluster is added to base, remove_offset and add_offset so that
        these addresses corresponds to the memory map of the cluster.

        Parameters
        ----------
        mapping: Mapping
            The mapping to be relocated.

        """

        mapping = mapping.copy()

        if mapping.get('base') is not None:
            mapping['base'] = '0x%x' % (int(mapping['base'], 0) + self.cluster_offset)

        if mapping.get('remove_offset') is not None:
            mapping['remove_offset'] = '0x%x' % (int(mapping['remove_offset'], 0) + self.cluster_offset)
            
        if mapping.get('add_offset') is not None:
            mapping['add_offset'] = '0x%x' % (int(mapping['add_offset'], 0) + self.cluster_offset)

        return mapping


    def _reloc_mapping_alias(self, mapping):
        """Relocate a mapping to fit the cluster alias.

        The base address of the cluster is removed to base, remove_offset and add_offset and the alias
        is added so that these addresses corresponds to the cluster alias.

        Parameters
        ----------
        mapping: Mapping
            The mapping to be relocated.

        """

        mapping = mapping.copy()

        if mapping.get('base') is not None:
            mapping['base'] = '0x%x' % (int(mapping['base'], 0) - self.cluster_base + self.cluster_alias)

        if mapping.get('remove_offset') is not None:
            mapping['remove_offset'] = '0x%x' % (int(mapping['remove_offset'], 0) - self.cluster_base + self.cluster_alias)
            
        if mapping.get('add_offset') is not None:
            mapping['add_offset'] = '0x%x' % (int(mapping['add_offset'], 0) - self.cluster_base + self.cluster_alias)

        return mapping


    def gen_gtkw_conf(self, tree, traces):
        if tree.get_view() == 'overview':
            self.vcd_group(self, skip=True)
        else:
            self.vcd_group(self, skip=False)
