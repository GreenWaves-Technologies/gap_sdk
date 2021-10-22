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

from generators.v1.comp_gen import *
import generators.v1.chip_gen as chip_gen

class Chip(chip_gen.Chip):

    def __init__(self, tp):
        super(Chip, self).__init__(tp)

        nb_cluster        = tp.get_child_int('cluster/nb_cluster')
        nb_pe             = tp.get_child_int('cluster/nb_pe')

        self.chip.fast_clock_out = self.chip.soc.fast_clock_out
        self.chip.soc.fast_clk_ctrl = self.chip.fast_clk_ctrl
        self.chip.soc.ref_clk_ctrl = self.chip.ref_clk_ctrl

        for cluster in range(0, nb_cluster):
            for pe in range(0, nb_pe):
                self.chip.soc.set('halt_cluster%d_pe%d' % (cluster, pe), self.chip.get(chip_gen.get_cluster_name(cluster)).new_itf('halt_pe%d' % pe))
