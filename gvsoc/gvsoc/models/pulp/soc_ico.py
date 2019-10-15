#
# Copyright (C) 2018 ETH Zurich and University of Bologna
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

# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 
import vp_core as vp


class component(vp.component):

    def build(self):

        config = self.get_config()

        # Low-latency interconnect
        ll_ico = self.new(name='ll', component='interco/router', config=config.get_config('ll_ico'))

        self.new_port('debug')
        self.get_port('debug').bind_to(ll_ico.get_port('in'))

        # Peripheral interco
        apb_ico = self.new(name='apb', component='interco/router', config=config.get_config('apb_ico'))

        # High-bandwidth interconnect
        hb_ico = self.new(name='hb', component='interco/interleaver', config=config.get_config('hb_ico'))

        # FC intercos
        fc_fetch_ico = self.new(name='fc_fetch_ico', component='interco/router', config=config.get_config('fc_fetch_ico'))
        fc_data_ico = self.new(name='fc_data_ico', component='interco/router', config=config.get_config('fc_data_ico'))

        l2_priv0_conf = config.get_config('l2_priv0')
        l2_priv1_conf = config.get_config('l2_priv1')
        l2_shared_conf = config.get_config('l2_shared')

        l2_size = 0

        if l2_priv0_conf is not None:
            l2_size += l2_priv0_conf.size.get_int()
            self.new_port('l2_priv0')

        # L2 private bank 0
        #ll_ico.getPort('out').mapTo(base=self.ico_conf.l2_priv0.base.get_int(), size=self.ico_conf.l2_priv0.size.get_int(), 
        #    removeOffset=self.ico_conf.l2_priv0.base.get_int(), port=self.getPort('l2_priv0'))



        if l2_priv1_conf is not None:
            l2_size += l2_priv1_conf.size.get_int()
            self.new_port('l2_priv1')

        if l2_shared_conf is not None:
            l2_size += l2_shared_conf.size.get_int()
            for i in range(0, config.get_int('nb_l2_shared_banks')):
              self.new_port('l2_shared_%d' % i)
              hb_ico.get_port('out_%d' % i).bind_to(port=self.get_port('l2_shared_%d'%i))

#    # L2 shared banks
#    for i in range(0, self.ico_conf.nb_l2_shared_banks()):
#      hb_ico.getPort('out%d' % i).linkTo(port=self.getPort('l2_shared_%d'%i))
#

        # HWPE
        self.new_port('hwpe_in')
        self.get_port('hwpe_in').bind_to(hb_ico.get_port('in'))

        # FC
        self.new_port('fc_fetch')
        self.get_port('fc_fetch').bind_to(fc_fetch_ico.get_port('in'))
        fc_fetch_ico.get_port('out').bind_to(
            port=hb_ico.get_port('in'),
            config=vp.map_config(
                base=l2_shared_conf.base.get_int(),
                size=l2_shared_conf.size.get_int()
            )
        )
        fc_fetch_ico.get_port('out').bind_to(port=ll_ico.get_port('in'))

        self.new_port('fc_data')
        self.get_port('fc_data').bind_to(fc_data_ico.get_port('in'))
        fc_data_ico.get_port('out').bind_to(
            port=hb_ico.get_port('in'),
            config=vp.map_config(
                base=l2_shared_conf.base.get_int(),
                size=l2_shared_conf.size.get_int()
            )
        )
        fc_data_ico.get_port('out').bind_to(port=ll_ico.get_port('in'))

        # ROM
        rom_conf = config.get_config('rom')
        if rom_conf is not None:
            self.new_port('rom')
            ll_ico.get_port('out').bind_to(
                port=self.get_port('rom'),
                config=vp.map_config(
                    base=rom_conf.base.get_int(),
                    size=rom_conf.size.get_int(),
                    remove_offset=rom_conf.base.get_int()
                )
            )

        # LL ICO
        if l2_priv0_conf is not None:
            ll_ico.get_port('out').bind_to(
                port=self.get_port('l2_priv0'),
                config=vp.map_config(
                    base=l2_priv0_conf.base.get_int(),
                    size=l2_priv0_conf.size.get_int(),
                    remove_offset=l2_priv0_conf.base.get_int()
                )
            )
            if l2_priv0_conf.get('alias_base') is not None:
                ll_ico.get_port('out').bind_to(
                    port=self.get_port('l2_priv0'),
                    config=vp.map_config(
                        base=l2_priv0_conf.alias_base.get_int(),
                        size=l2_priv0_conf.size.get_int(),
                        remove_offset=l2_priv0_conf.alias_base.get_int()
                    )
                )

        if l2_priv1_conf is not None:
            ll_ico.get_port('out').bind_to(
                port=self.get_port('l2_priv1'),
                config=vp.map_config(
                    base=l2_priv1_conf.base.get_int(),
                    size=l2_priv1_conf.size.get_int(),
                    remove_offset=l2_priv1_conf.base.get_int()
                )
            )

        # APB
        ll_ico.get_port('out').bind_to(
            port=apb_ico.get_port('in'),
            config=vp.map_config(
                base=config.apb.base.get_int(),
                size=config.apb.size.get_int()
            )
        )


#    self.newPort('axi_master')
#    self.newPort('axi_slave')
#    self.newPort('fc_data')
#    self.newPort('fc_fetch')
#    self.newPort('udma_in')
#    self.newPort('debug')
#    self.newPort('in')
#
#    # UDMA interco
#    udma_ico = self.newComp(className=ico.gv_router.Gv_router, compName='udma', widthBits=2)
#
#    # AXI slave interco
#    axi_slave_ico = self.newComp(className=ico.gv_router.Gv_router, compName='axi_slave', widthBits=2)
#
#
#
#
#
#    # L2 private bank 1
#    ll_ico.getPort('out').mapTo(base=self.ico_conf.l2_priv1.base.get_int(), size=self.ico_conf.l2_priv1.size.get_int(), 
#      removeOffset=self.ico_conf.l2_priv1.base.get_int(), port=self.getPort('l2_priv1'))
#
#    # LL ico to HB ico
#    ll_ico.getPort('out').mapTo(base=self.ico_conf.l2_shared.base.get_int(), size=self.ico_conf.l2_shared.size.get_int(), 
#      port=hb_ico.getPort('in'))
#
#    self.getPort('fc_data').linkTo(fc_data_ico.getPort('in'))
#    fc_data_ico.getPort('out').mapTo(
#        base=self.ico_conf.l2_shared.base.get_int(),
#        size=self.ico_conf.l2_shared.size.get_int(),
#        port=hb_ico.getPort('in')
#    )
#    fc_data_ico.getPort('out').mapTo(port=ll_ico.getPort('in'))
#    fc_data_ico.getPort('out').mapTo(
#        base=0, size=self.ico_conf.l2_priv0.size.get_int(),
#        addOffset=self.ico_conf.l2_priv0.base.get_int(),
#        port=ll_ico.getPort('in')
#    )
#
#    # UDMA
#    self.getPort('udma_in').linkTo(udma_ico.getPort('in'))
#    udma_ico.getPort('out').mapTo(
#        base=self.ico_conf.l2_shared.base.get_int(),
#        size=self.ico_conf.l2_shared.size.get_int(),
#        port=hb_ico.getPort('in')
#    )
#    udma_ico.getPort('out').mapTo(port=ll_ico.getPort('in'))
#
#    # AXI slave
#    self.getPort('axi_slave').linkTo(axi_slave_ico.getPort('in'))
#    axi_slave_ico.getPort('out').mapTo(
#        base=self.ico_conf.l2_shared.base.get_int(),
#        size=self.ico_conf.l2_shared.size.get_int(),
#        port=hb_ico.getPort('in')
#    )
#    axi_slave_ico.getPort('out').mapTo(port=ll_ico.getPort('in'))
#
#    # AXI master
#    ll_ico.getPort('out').mapTo(self.getPort('axi_master'))
#
#
#    self.getPort('in').linkTo(ll_ico.getPort('in'))

        peripherals_conf = config.get_config('peripherals')
        if peripherals_conf is not None:
            periph_base = config.get_int('peripherals_base')
            for name, conf in peripherals_conf.items():
                self.new_port(name)
                apb_ico.get_port('out').bind_to(
                    port=self.get_port(name),
                    config=vp.map_config(
                        base=conf.offset.get_int() + periph_base,
                        size=conf.size.get_int(),
                        remove_offset=conf.offset.get_int() + periph_base
                    )
                )
