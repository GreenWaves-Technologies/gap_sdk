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
from collections import OrderedDict


class Efuse_field(object):

    def __init__(self, name, bit, width):
        self.name = name
        self.bit = bit
        self.width = width
        self.efuse = None

    def set(self, value):
        self.efuse.set_field_value(value, self.bit, self.width)

    def set2(self, value):
        self.efuse.set_field_value2(value, self.bit, self.width)

    def get(self):
        return self.efuse.get_field_value(self.bit, self.width)

    def __str__(self):
        return '%s=0x%x' % (self.name, self.get())


class Efuse(object):

    fields = [
    ]

    def __init__(self, name, id):

        self.name = name
        self.id = id

        self.fields_dict = {}
        for field in self.fields:
            self.fields_dict[field.name] = field
            field.efuse = self

        self.value = 0


    def get_field(self, name):
        field = self.fields_dict.get(name)
        if field is None:
            raise RuntimeError('Unknown efuse field (efuse: %s, field: %s)' % (self.name, name))
        
        return field

    def get(self):
        return self.value

    def set(self, value):
        self.value = value

    def set_field_value(self, value, bit, width):
        self.value = (self.value & ~(((1<<width) - 1) << bit)) | (value << bit)

    def set_field_value2(self, value, bit, width):
        self.value = (self.value & ~(((1<<width) - 1) << bit)) | (value << bit)

    def get_field_value(self, bit, width):
        return (self.value >> bit) & ((1<<width) - 1)

    def __str__(self):
        result = '%s: value=0x%x' % (self.name, self.value)

        if len(self.fields) != 0:
            fields_value = []
            for field in self.fields:
                fields_value.append(str(field))


            result += ' fields=( ' + ', '.join(fields_value) + ' )'

        return result


class Info_1(Efuse):

    fields = [
        Efuse_field('platform'                , 0,  3),
        Efuse_field('bootmode'                , 3,  8),
        Efuse_field('encrypted'               , 11, 1),
        Efuse_field('wait_xtal'               , 12, 1),
        Efuse_field('icache_enabled'          , 13, 1),
        Efuse_field('fll_global_setup'        , 14, 1),
        Efuse_field('fll_dco0_setup'          , 15, 1),
        Efuse_field('padfun0_setup'           , 16, 1),
        Efuse_field('padfun1_setup'           , 17, 1),
        Efuse_field('padfun2_setup'           , 18, 1),
        Efuse_field('padfun3_setup'           , 19, 1),
        Efuse_field('padfun4_setup'           , 20, 1),
        Efuse_field('padfun5_setup'           , 21, 1),
        Efuse_field('pmu_wait_reset_skip'     , 22, 1),
        Efuse_field('timer_source'            , 23, 2),
        Efuse_field('fast_clk_div_pow2_setup' , 25, 1),
        Efuse_field('osc_ctrl_setup'          , 26, 1),
        Efuse_field('osc_ctrl'                , 27, 3),
        Efuse_field('feature_disable_set'     , 30, 1),
        Efuse_field('mram_reset_wait'         , 31, 1),
    ]

    def __init__(self, name, offset):
        super(Info_1, self).__init__(name, offset)


class Info_2(Efuse):

    fields = [
        Efuse_field('clkdiv_setup'            , 0, 1),
        Efuse_field('clkdiv'                  , 1, 5),
        Efuse_field('jtag_lock'               , 6, 1),
        Efuse_field('ref_clk_wait'            , 7, 1),
        Efuse_field('ref_clk_wait_deep_sleep' , 8, 1),
        Efuse_field('bootmode0_nocheck'       , 9, 1),
        Efuse_field('bootmode1_nocheck'       , 10, 1),
        Efuse_field('mram_trim'               , 11, 1),
        Efuse_field('wake_fast_clk_div_pow2_setup' , 12, 1),
        Efuse_field('wake_osc_ctrl_setup'          , 13, 1),
        Efuse_field('wake_osc_ctrl'                , 14, 2),
        Efuse_field('spi_conf_wait'           , 16, 1),
        Efuse_field('wake_wait_xtal'          , 17, 1),
        Efuse_field('fll_wait'                , 18, 1),
        Efuse_field('fll_wake_wait'           , 19, 1),
        Efuse_field('flash_status_set'        , 21, 2),
        Efuse_field('flash_commands_set'      , 23, 1),
        Efuse_field('flash_latency_set'       , 24, 1),
        Efuse_field('flash_latency_value'     , 25, 5),
        Efuse_field('wake_mram_reset_wait'    , 30, 1),
    ]

    def __init__(self, name, offset):
        super(Info_2, self).__init__(name, offset)


class Info_3(Efuse):

    fields = [
        Efuse_field('flash_cs_setup'       , 0,   1),
        Efuse_field('flash_cs'             , 1,   1),
        Efuse_field('flash_itf_setup'      , 2,   1),
        Efuse_field('flash_itf'            , 3,   2),
        Efuse_field('flash_offset_setup'   , 5,   1),
        Efuse_field('hyper_delay_setup'    , 6,   1),
        Efuse_field('hyper_delay'          , 7,   3),
        Efuse_field('hyper_latency_setup'  , 10,  1),
        Efuse_field('hyper_latency'        , 11,  5),
        Efuse_field('hyper_cs_polarity'    , 16,  5),
        Efuse_field('flash_wakeup'         , 17,  1),
        Efuse_field('flash_reset'          , 18,  1),
        Efuse_field('flash_init'           , 19,  1),
        Efuse_field('flash_wait'           , 20,  1),
        Efuse_field('flash_cmd_1'          , 21,  1),
        Efuse_field('flash_cmd_2'          , 22,  1),
        Efuse_field('flash_cmd_3'          , 23,  1),
        Efuse_field('flash_cmd_4'          , 24,  1),
        Efuse_field('flash_cmd_1_ds'       , 25,  1),
        Efuse_field('flash_cmd_2_ds'       , 26,  1),
        Efuse_field('flash_cmd_3_ds'       , 27,  1),
        Efuse_field('flash_cmd_4_ds'       , 28,  1),
        Efuse_field('flash_reset_wait'     , 29,  1),
        Efuse_field('flash_wakeup_wait'    , 30,  1),
    ]

    def __init__(self, name, offset):
        super(Info_3, self).__init__(name, offset)


class Info_4(Efuse):

    fields = [
        Efuse_field('flash_gpio_pulse_gen'       , 0,   1),
        Efuse_field('flash_gpio_pulse_wait'      , 1,   1),
        Efuse_field('flash_gpio_pulse_pol'       , 2,   1),
        Efuse_field('flash_gpio_pulse_id'        , 3,   7),
        Efuse_field('neva_cfg'                   , 10,   1),
    ]

    def __init__(self, name, offset):
        super(Info_4, self).__init__(name, offset)


class Info_5(Efuse):

    fields = [
        Efuse_field('flash_pad'       , 3,  2),
    ]

    def __init__(self, name, offset):
        super(Info_5, self).__init__(name, offset)



class Padfun(Efuse):

    def __init__(self, name, offset):
        self.fields = [
            Efuse_field('pad0'            , 0 ,  2),
            Efuse_field('pad1'            , 2 ,  2),
            Efuse_field('pad2'            , 4 ,  2),
            Efuse_field('pad3'            , 6 ,  2),
            Efuse_field('pad4'            , 8 ,  2),
            Efuse_field('pad5'            , 10,  2),
            Efuse_field('pad6'            , 12,  2),
            Efuse_field('pad7'            , 14,  2),
            Efuse_field('pad8'            , 16,  2),
            Efuse_field('pad9'            , 18,  2),
            Efuse_field('pad10'           , 20,  2),
            Efuse_field('pad11'           , 22,  2),
            Efuse_field('pad12'           , 24,  2),
            Efuse_field('pad13'           , 26,  2),
            Efuse_field('pad14'           , 28,  2),
            Efuse_field('pad15'           , 30,  2),
        ]

        super(Padfun, self).__init__(name, offset)



class Efuse_map(object):

    efuses = [
        Info_1 ( 'info_1'                         , 0),
        Info_2 ( 'info_2'                         , 1),
        Info_3 ( 'info_3'                         , 2),
        Efuse  ( 'fll_drr'                        , 3),
        Efuse  ( 'fll_ccr1_pre_lock'              , 4),
        Efuse  ( 'fll_ccr1_post_lock'             , 5),
        Efuse  ( 'fll_ccr2'                       , 6),
        Efuse  ( 'fll_f0cr1'                      , 7),
        Efuse  ( 'fll_f0cr2'                      , 8),
        Padfun ( 'padfun0'                        , 9),
        Padfun ( 'padfun1'                        , 10),
        Padfun ( 'padfun2'                        , 11),
        Padfun ( 'padfun3'                        , 12),
        Padfun ( 'padfun4'                        , 13),
        Padfun ( 'padfun5'                        , 14),
        Efuse  ( 'feature_disable'                , 15),
        Efuse  ( 'wait_xtal_period'               , 32),
        Efuse  ( 'wait_xtal_delta'                , 33),
        Efuse  ( 'wait_xtal_min'                  , 34),
        Efuse  ( 'wait_xtal_max'                  , 35),
        Efuse  ( 'ref_clk_wait_cycles'            , 36),
        Efuse  ( 'ref_clk_wait_cycles_deep_sleep' , 37),
        Efuse  ( 'fast_clk_div_pow2'              , 38),
        Efuse  ( 'wakeup_fll_drr'                 , 39),
        Efuse  ( 'wakeup_fll_ccr1_pre_lock'       , 40),
        Efuse  ( 'wakeup_fll_ccr1_post_lock'      , 41),
        Efuse  ( 'wakeup_fll_ccr2'                , 42),
        Efuse  ( 'wakeup_fll_f0cr1'               , 43),
        Efuse  ( 'wakeup_fll_f0cr2'               , 44),
        Efuse  ( 'wake_fast_clk_div_pow2'         , 45),
        Efuse  ( 'mram_reset_wait_cycles'         , 46),
        Efuse  ( 'wake_mram_reset_wait_cycles'    , 47),
        Efuse  ( 'spi_conf_wait_cycles'           , 48),
        Efuse  ( 'flash_offset'                   , 49),
        Efuse  ( 'fll_wait_cycles'                , 50),
        Efuse  ( 'fll_wake_wait_cycles'           , 51),
        Efuse  ( 'flash_reset_wait'               , 53),
        Efuse  ( 'flash_cmd_1'                    , 54),
        Efuse  ( 'flash_cmd_2'                    , 55),
        Efuse  ( 'flash_cmd_3'                    , 56),
        Efuse  ( 'flash_cmd_4'                    , 57),
        Efuse  ( 'flash_wait'                     , 58),
        Efuse  ( 'flash_wakeup_wait'              , 59),
        Efuse  ( 'flash_status'                   , 60),
        Efuse  ( 'flash_commands'                 , 61),
        Info_4 ( 'info_4'                         , 62),
        Efuse  ( 'flash_gpio_pulse_wait'          , 63),
        Efuse  ( 'neva_cfg'                       , 64),
        Efuse  ( 'mram_trim_size'                 , 65),
        Efuse  ( 'mram_trim_start'                , 66),
    ]

    nb_regs = 128


    def __init__(self):
        self.efuses_dict = {}
        self.efuses_list = [Efuse('empty', -1)] * self.nb_regs
        for efuse in self.efuses:
            self.efuses_dict[efuse.name] = efuse
            self.efuses_list[efuse.id] = efuse


    def apply_from_config(self, config):
        
        efuses_conf  = config.get('**/runner/efuses/content')

        for name, value in efuses_conf.get_items().items():
            try:
                self.get_efuse(name).set(value.get_int())
            except:
                for field_name, field_value in value.get_items().items():
                    self.get_efuse(name).get_field(field_name).set(field_value.get_int())


    def get_efuse(self, name):
        efuse = self.efuses_dict.get(name)
        if efuse is None:
            raise RuntimeError('Unknown efuse (name: %s)' % name)

        return efuse


    def __str__(self):
        result = ''
        for efuse in self.efuses:
            result += str(efuse) + '\n'
        return result


    def gen(self, traces, filename):
        traces.info('  Generating to file: ' + filename)

        with open(filename, 'w') as file:
            for id in range (0, self.nb_regs):
                value = self.efuses_list[id].get()
                traces.info('  Writing register (index: %d, value: 0x%x)' % (id, value))
                file.write('{0:032b}\n'.format(value))
