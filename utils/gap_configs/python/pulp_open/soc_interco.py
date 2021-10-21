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
import ips.interco.router as router

class Soc_interco(st.Component):

    def __init__(self, parent, name, soc, cluster):
        super(Soc_interco, self).__init__(parent, name)

        ll_ico = router.Router(self, 'll_ico')

        ll_ico.add_mapping('apb'           , **soc.get_property('peripherals/mapping'))
        ll_ico.add_mapping('rom'           , base=soc.get_property('apb_ico/mappings/rom/base'), size=soc.get_property('apb_ico/mappings/rom/size'))
        ll_ico.add_mapping('axi_master'    , **cluster.get_property('mapping'))
        ll_ico.add_mapping('l2_priv0'      , **soc.get_property('l2/priv0/mapping'))
        ll_ico.add_mapping('l2_priv0_alias', **soc.get_property('l2/priv0_alias/mapping'))
        ll_ico.add_mapping('l2_priv1'      , **soc.get_property('l2/priv1/mapping'))
        ll_ico.add_mapping('l2_shared'     , **soc.get_property('l2/shared/mapping'))

        self.bind(self, 'debug', ll_ico, 'input')
        self.bind(self, 'axi_slave', ll_ico, 'input')

        hb_ico = router.Router(self, 'hb_ico', remove_offset=soc.get_property('l2/shared/mapping/base'))

        l2_shared_size = soc.get_property('l2/shared/mapping/size', int)
        l2_shared_nb_regions = soc.get_property('l2/shared/nb_regions')
        region_base = soc.get_property('l2/shared/mapping/base', int)
        region_size = int(l2_shared_size / l2_shared_nb_regions)

        for i in range(0, soc.get_property('l2/shared/nb_regions')):
            hb_ico.add_mapping('l2_shared_%d' % i, base=region_base, size=region_size, remove_offset=region_base)
            self.bind(hb_ico, 'l2_shared_%d' % i, self, 'l2_shared_%d' % i)
            region_base += region_size

        fc_fetch_ico = router.Router(self, 'fc_fetch_ico', latency=5)
        fc_data_ico = router.Router(self, 'fc_data_ico')

        fc_fetch_ico.add_mapping('l2_shared', **soc.get_property('l2/shared/mapping'))
        fc_fetch_ico.add_mapping('xip', **soc.get_property('l2/xip/mapping'))
        self.bind(fc_fetch_ico, 'xip', self, 'fc_fetch_input')
        fc_data_ico.add_mapping('xip', **soc.get_property('l2/xip/mapping'))
        self.bind(fc_data_ico, 'xip', self, 'fc_data_input')
        fc_fetch_ico.add_mapping('ll_ico')

        fc_data_ico.add_mapping('l2_shared', **soc.get_property('l2/shared/mapping'))
        fc_data_ico.add_mapping('axi_proxy', base=0x20000000, size=0x10000000)
        fc_data_ico.add_mapping('ll_ico')
        
        udma_rx_ico = router.Router(self, 'udma_rx_ico')
        udma_rx_ico.add_mapping('l2_shared', **soc.get_property('l2/shared/mapping'))
        self.bind(udma_rx_ico, 'l2_shared', hb_ico, 'input')
        udma_rx_ico.add_mapping('ll_ico')
        self.bind(udma_rx_ico, 'll_ico', ll_ico, 'input')
        
        udma_tx_ico = router.Router(self, 'udma_tx_ico', latency=4)
        udma_tx_ico.add_mapping('l2_shared', **soc.get_property('l2/shared/mapping'))
        self.bind(udma_tx_ico, 'l2_shared', hb_ico, 'input')
        udma_tx_ico.add_mapping('ll_ico')
        self.bind(udma_tx_ico, 'll_ico', ll_ico, 'input')
        self.bind(self, 'udma_tx', udma_tx_ico, 'input')

        self.bind(self, 'input', hb_ico, 'input')
        self.bind(self, 'fc_fetch', fc_fetch_ico, 'input')
        self.bind(self, 'fc_data', fc_data_ico, 'input')
        self.bind(fc_fetch_ico, 'l2_shared', hb_ico, 'input')
        self.bind(fc_fetch_ico, 'll_ico', ll_ico, 'input')
        self.bind(fc_data_ico, 'l2_shared', hb_ico, 'input')
        self.bind(fc_data_ico, 'll_ico', ll_ico, 'input')
        self.bind(fc_data_ico, 'axi_proxy', self, 'axi_proxy')

        self.bind(udma_rx_ico, 'l2_shared', hb_ico, 'input')
        self.bind(udma_rx_ico, 'll_ico', ll_ico, 'input')
        self.bind(udma_rx_ico, 'l2_shared', hb_ico, 'input')
        self.bind(udma_rx_ico, 'll_ico', ll_ico, 'input')

        self.bind(ll_ico, 'apb', self, 'apb')
        self.bind(ll_ico, 'rom', self, 'apb')
        self.bind(ll_ico, 'l2_priv0', self, 'l2_priv0')
        self.bind(ll_ico, 'l2_priv0_alias', self, 'l2_priv0')
        self.bind(ll_ico, 'l2_priv1', self, 'l2_priv1')
        self.bind(ll_ico, 'l2_shared', hb_ico, 'input')
        self.bind(ll_ico, 'axi_master', self, 'axi_master')
    