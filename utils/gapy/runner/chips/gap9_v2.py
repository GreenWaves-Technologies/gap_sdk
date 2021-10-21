#
# Copyright (C) 2019 GreenWaves Technologies
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

import traces
import os
import runner.chips.gap9_v2_efuse as efuse
from elftools.elf.elffile import ELFFile
import math

class Runner(object):

    def set_padfun(self, efuse_map, pad, fun):
        reg_id = int(pad / 16)
        reg_pad = (pad % 16)

        efuse_map.get_efuse('info_1').get_field('padfun%d_setup' % reg_id).set(1)
        efuse_map.get_efuse('padfun%d' % reg_id).get_field('pad%d' % reg_pad).set2(fun)

    def __init__(self, args, config, system):
        self.config = config

    def gen_efuse_stim(self, filename):
        traces.info('Creating efuse stimuli')
        efuse_map = efuse.Efuse_map()

        if self.config.get_bool('**/runner/efuses/enabled'):

            # Set all padfun to 1 by default except for JTAG pads to reflect edfault HW values
            for i in range(0, 96):
                #self.set_padfun(efuses, i,  1)
                self.set_padfun(efuse_map, i,  0)  # TODO temporary put to 0 to not break all tests

            self.set_padfun(efuse_map, 81,  0)
            self.set_padfun(efuse_map, 82,  0)
            self.set_padfun(efuse_map, 83,  0)
            self.set_padfun(efuse_map, 84,  0)
            self.set_padfun(efuse_map, 85,  0)

            efuse_map.get_efuse('info_1').get_field('platform').set(2)  # RTL platform
            efuse_map.get_efuse('info_1').get_field('icache_enabled').set(1)

            # By default, only activate fast clock and fed other blocks like timer at 24Mhz/16
            fast_osc_freq_div = 24576062.0 / 16
            efuse_map.get_efuse('info_1').get_field('osc_ctrl_setup').set(1)
            efuse_map.get_efuse('info_1').get_field('osc_ctrl').set(1)
            efuse_map.get_efuse('info_1').get_field('fast_clk_div_pow2_setup').set(1)
            efuse_map.get_efuse('fast_clk_div_pow2').set(4 | (1<<3))
            efuse_map.get_efuse('info_2').get_field('wake_osc_ctrl_setup').set(1)
            efuse_map.get_efuse('info_2').get_field('wake_osc_ctrl').set(1)
            efuse_map.get_efuse('info_2').get_field('wake_fast_clk_div_pow2_setup').set(1)
            efuse_map.get_efuse('wake_fast_clk_div_pow2').set(4 | (1<<3))

            # Activate oscillator stability wait loop
            efuse_map.get_efuse('info_1').get_field('wait_xtal').set(1)
            efuse_map.get_efuse('wait_xtal_period').set(500)  # Xtal is monitored in open loop at max 50MHz and we want to monitor every 10us
            efuse_map.get_efuse('wait_xtal_delta').set(10)    # Delta is 1%
            efuse_map.get_efuse('wait_xtal_min').set(5)       # Stop as soon as 5 stable periods are found
            efuse_map.get_efuse('wait_xtal_max').set(50000)
            # Also put right values in fixed wait loop in case a test wants to activate it
            cycles = int(2000*fast_osc_freq_div/1000000)

            efuse_map.get_efuse('fll_wait_cycles').set(cycles) # 100us
            efuse_map.get_efuse('fll_wake_wait_cycles').set(cycles) # 100us

            # No wait for ref clock
            efuse_map.get_efuse('info_2').get_field('ref_clk_wait').set(1)
            efuse_map.get_efuse('ref_clk_wait_cycles').set(0)
            efuse_map.get_efuse('info_2').get_field('ref_clk_wait_deep_sleep').set(1)
            efuse_map.get_efuse('ref_clk_wait_cycles_deep_sleep').set(0)
            efuse_map.get_efuse('info_1').get_field('timer_source').set(2)

            # Enable JTAG
            efuse_map.get_efuse('info_1').get_field('feature_disable_set').set(1)
            efuse_map.get_efuse('feature_disable').set(0)

            boot_mode  = self.config.get_str('**/runner/boot/mode')
            if boot_mode == 'flash':
                device = self.config.get(self.config.get_str('**/runner/boot/device'))
                device_type = device.get_str('datasheet/type')

                # Do not check bootmode pads by default to boot from what is specified in efuses
                pads_nocheck = 0 if self.config.get_bool('**/runner/boot/jtag_force') else 1
                efuse_map.get_efuse('info_2').get_field('bootmode0_nocheck').set(pads_nocheck)
                efuse_map.get_efuse('info_2').get_field('bootmode1_nocheck').set(pads_nocheck)

                if device_type == 'hyper':
                    # Boot on UDMA hyper
                    efuse_map.get_efuse('info_3').get_field('flash_cs_setup').set(1)
                    efuse_map.get_efuse('info_3').get_field('flash_cs').set(1)
                    efuse_map.get_efuse('info_3').get_field('flash_itf_setup').set(1)
                    efuse_map.get_efuse('info_3').get_field('flash_itf').set(0)
                    efuse_map.get_efuse('info_1').get_field('bootmode').set(1)
                    efuse_map.get_efuse('info_2').get_field('clkdiv_setup').set(1)
                    efuse_map.get_efuse('info_2').get_field('clkdiv').set(0)
                    # Pads for hyper 0
                    self.set_padfun(efuse_map, 0,  0)
                    self.set_padfun(efuse_map, 1,  0)
                    self.set_padfun(efuse_map, 2,  0)
                    self.set_padfun(efuse_map, 3,  0)
                    self.set_padfun(efuse_map, 4,  0)
                    self.set_padfun(efuse_map, 5,  0)
                    self.set_padfun(efuse_map, 6,  0)
                    self.set_padfun(efuse_map, 7, 0)
                    self.set_padfun(efuse_map, 8, 0)
                    self.set_padfun(efuse_map, 9, 0)
                    self.set_padfun(efuse_map, 10, 0)
                    self.set_padfun(efuse_map, 11, 0)
                    self.set_padfun(efuse_map, 12, 0)


                elif device_type == 'spi':
                    # Boot on UDMA spi
                    efuse_map.get_efuse('info_1').get_field('bootmode').set(2)
                    efuse_map.get_efuse('info_2').get_field('clkdiv_setup').set(1)
                    efuse_map.get_efuse('info_2').get_field('clkdiv').set(0)
                    # Flash is on CS 0 ITF 1 (CS is inverted in efuse)
                    efuse_map.get_efuse('info_3').get_field('flash_cs_setup').set(1)
                    efuse_map.get_efuse('info_3').get_field('flash_cs').set(1)
                    efuse_map.get_efuse('info_3').get_field('flash_itf_setup').set(1)
                    efuse_map.get_efuse('info_3').get_field('flash_itf').set(1)

                    # SPI wait time after configuring control register, should take 200ns but RTL model take 10us to update it
                    efuse_map.get_efuse('info_2').get_field('spi_conf_wait').set(1)
                    efuse_map.get_efuse('spi_conf_wait_cycles').set(math.ceil(0.00001*fast_osc_freq_div))

                    # SPI status register value
                    efuse_map.get_efuse('info_2').get_field('flash_status_set').set(2)
                    efuse_map.get_efuse('flash_status').set(0x1b880200)   # Activate octospi mode and DTR and unprotect all sectors

                    # SPI flash latency
                    efuse_map.get_efuse('info_2').get_field('flash_latency_set').set(1)
                    efuse_map.get_efuse('info_2').get_field('flash_latency_value').set(22)

                    # Flash commands
                    efuse_map.get_efuse('info_2').get_field('flash_commands_set').set(1)
                    efuse_map.get_efuse('flash_commands').set((0x06 << 0) | (0x71 << 8) | (0x0B << 16) | (0xAB << 24))


                elif device_type == 'mram':
                    # Boot on MRAM
                    efuse_map.get_efuse('info_1').get_field('bootmode').set(3)
                    efuse_map.get_efuse('info_1').get_field('mram_reset_wait').set(1)
                    efuse_map.get_efuse('info_2').get_field('wake_mram_reset_wait').set(1)
                    efuse_map.get_efuse('mram_reset_wait_cycles').set(math.ceil(0.000003*fast_osc_freq_div))
                    efuse_map.get_efuse('wake_mram_reset_wait_cycles').set(math.ceil(0.000003*fast_osc_freq_div))
                    efuse_map.get_efuse('info_2').get_field('clkdiv_setup').set(1)
                    efuse_map.get_efuse('info_2').get_field('clkdiv').set(5)
                    efuse_map.get_efuse('info_3').get_field('flash_wait').set(1)
                    efuse_map.get_efuse('flash_wait').set(math.ceil(0.00002*fast_osc_freq_div))

            elif boot_mode == 'jtag':
                efuse_map.get_efuse('info_2').get_field('bootmode0_nocheck').set(0)
                efuse_map.get_efuse('info_2').get_field('bootmode1_nocheck').set(0)

                efuse_map.get_efuse('info_1').get_field('bootmode').set(0)

            elif boot_mode == 'spislave':
                # Do not check bootmode pads by default to boot from what is specified in efuses
                efuse_map.get_efuse('info_2').get_field('bootmode0_nocheck').set(1)
                efuse_map.get_efuse('info_2').get_field('bootmode1_nocheck').set(1)

                efuse_map.get_efuse('info_1').get_field('bootmode').set(4)


            # Lock FLL soc and periph
            efuse_map.get_efuse('info_1').get_field('fll_global_setup').set(1)
            efuse_map.get_efuse('info_1').get_field('fll_dco0_setup').set(1)
            # FLL DRR (DCO min | DCO max)
            efuse_map.get_efuse('fll_drr').set((0 << 0) | (0x1ff << 16))
            # Pre-lock FLL CCR1 (CLK0 DIV | CLK1 DIV)
            efuse_map.get_efuse('fll_ccr1_pre_lock').set((0 << 0) | (0 << 8))
            # Post-lock FLL CCR1 (CLK0 DIV | CLK1 DIV)
            efuse_map.get_efuse('fll_ccr1_post_lock').set((0 << 0) | (3 << 8))
            # FLL CCR2 (CLK0 SEL | CLK1 SEL | CLK2_SEL | CLK3_SEL | CKG0)
            efuse_map.get_efuse('fll_ccr2').set((0x1 << 0) | (0x1 << 4) | (0x1 << 8) | (0x2 << 12) | (1 << 16))
            # DCO0 CR1 (DCO EN | CLOSE LOOP | LOOP GAIN | LOCK TOL | ITG | ASSERT CYCLES)
            efuse_map.get_efuse('fll_f0cr1').set((1 << 0) | (1 << 1) | (4 << 4) | (10 << 8) | (24 << 16) | (6 << 26))
            # DCO0 CR2 (MFI | DCO CODE)
            efuse_map.get_efuse('fll_f0cr2').set((166 << 0) | (0x1A << 16))

            # FLL DRR (DCO min | DCO max)
            efuse_map.get_efuse('wakeup_fll_drr').set((0 << 0) | (0x1ff << 16))
            # Pre-lock FLL CCR1 (CLK0 DIV | CLK1 DIV)
            efuse_map.get_efuse('wakeup_fll_ccr1_pre_lock').set((0 << 0) | (0 << 8))
            # Post-lock FLL CCR1 (CLK0 DIV | CLK1 DIV)
            efuse_map.get_efuse('wakeup_fll_ccr1_post_lock').set((0 << 0) | (1 << 8))
            # FLL CCR2 (CLK0 SEL | CLK1 SEL | CLK2_SEL | CLK3_SEL | CKG0)
            efuse_map.get_efuse('wakeup_fll_ccr2').set((0x1 << 0) | (0x1 << 4) | (0x1 << 8) | (0x2 << 12) | (1 << 16))
            # DCO0 CR1 (DCO EN | CLOSE LOOP | LOOP GAIN | LOCK TOL | ITG | ASSERT CYCLES)
            efuse_map.get_efuse('wakeup_fll_f0cr1').set((1 << 0) | (1 << 1) | (4 << 4) | (10 << 8) | (24 << 16) | (6 << 26))
            # DCO0 CR2 (MFI | DCO CODE)
            efuse_map.get_efuse('wakeup_fll_f0cr2').set((166 << 0) | (0x1A << 16))


        efuse_map.apply_from_config(self.config)

        efuse_map.gen(traces, filename)
