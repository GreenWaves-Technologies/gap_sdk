#!/usr/bin/env python3

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

from isa_pulp_gen import *


class Gap9_core(Riscv_core):

    def __init__(self):
        super(Gap9_core, self).__init__()

        self.isa = Isa(
            'riscv',
            [
                IsaDecodeTree('pulp_v2', [priv_pulp_v2]),
                IsaDecodeTree('i', [rv32i]),
                IsaDecodeTree('m', [rv32m]),
                IsaDecodeTree('c', [rv32c]),
                IsaDecodeTree('priv', [priv]),
                IsaDecodeTree('pulp_v2', [pulp_v2]),
                IsaDecodeTree('f', [rv32f]),
                IsaDecodeTree('sfloat', [Xf16, Xf16alt, Xf8, Xfvec, Xfaux]),
                IsaDecodeTree('gap9', [gap9]),
                IsaDecodeTree('int64', [int64]),
            ]
        )

        for insn in self.isa.get_tree('pulp_v2').get_insns():
            insn.set_power_group(1)

        for insn in self.isa.get_insns():

            if "load" in insn.tags:
                insn.get_out_reg(0).set_latency(2)
            elif "mul" in insn.tags:
                insn.get_out_reg(0).set_latency(2)
            elif "mulh" in insn.tags:
                insn.set_latency(5)



class Gap9_cluster_core(Gap9_core):

    def __init__(self):
        
        super(Gap9_cluster_core, self).__init__()

        # Declare the 3 kind of shared resources with appropriate latency and bandwidth
        self.isa.add_resource('fpu_base', instances=4)
        self.isa.add_resource('fpu_sqrt', instances=1)
        self.isa.add_resource('int64', instances=4)

        # And attach resources to instructions
        for insn in self.isa.get_tree('f').get_insns() + self.isa.get_tree('sfloat').get_insns():

            # All float operations are handled by the same unit
            self.__attach_resource(insn, 'fpu_base', latency=1, bandwidth=1, tags=[
                'fmadd', 'fadd', 'fmul', 'fconv', 'fother',
                'sfmadd', 'sfadd', 'sfmul', 'sfconv', 'sfother', 
            ])

            # Except div, rem and sqrt which goes to the sqrt unit
            self.__attach_resource(insn, 'fpu_sqrt', latency=14, bandwidth=14, tags=[
                'fdiv'
            ])

            # Except div, rem and sqrt which goes to the sqrt unit
            self.__attach_resource(insn, 'fpu_sqrt', latency=10, bandwidth=10, tags=[
                'sfdiv'
            ])

        # All int64 operations go to a specific unit
        for insn in self.isa.get_tree('int64').get_insns():

            self.__attach_resource(insn, 'int64', latency=1, bandwidth=1)


    def __attach_resource(self, insn, resource, latency, bandwidth, tags=[]):
        if len(tags) == 0:
            insn.attach_resource(resource, latency, bandwidth)
        else:
            for tag in tags:
                if tag in insn.tags:
                    insn.attach_resource(resource, latency, bandwidth)



class Gap9_fc_core(Gap9_core):

    def __init__(self):
        
        super(Gap9_fc_core, self).__init__()

        for insn in self.isa.get_insns():

            if "fdiv" in insn.tags:
                insn.get_out_reg(0).set_latency(15)
            elif "sfdiv" in insn.tags:
                insn.get_out_reg(0).set_latency(15)
