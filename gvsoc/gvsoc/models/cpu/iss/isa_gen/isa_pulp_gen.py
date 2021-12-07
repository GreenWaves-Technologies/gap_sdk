#
# Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
#                    University of Bologna
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

from isa_riscv_gen import *


class Riscv_core(object):

    def __init__(self):
        pass

    def gen(self, header_file, source_file):
        try:
            os.makedirs(os.path.dirname(header_file))
        except Exception:
            pass

        try:
            os.makedirs(os.path.dirname(source_file))
        except Exception:
            pass

        with open(header_file, 'w') as isaFileHeader:
            with open(source_file, 'w') as isaFile:
                self.isa.gen(isaFile, isaFileHeader)



class Riscy(Riscv_core):

    def __init__(self):
        super(Riscy, self).__init__()

        self.isa = Isa(
            'riscv',
            [
                IsaDecodeTree('pulp_v2', [priv_pulp_v2]),
                IsaDecodeTree('i', [rv32i]),
                IsaDecodeTree('m', [rv32m]),
                IsaDecodeTree('c', [rv32c]),
                IsaDecodeTree('priv', [priv]),
                IsaDecodeTree('pulp_v2', [pulp_v2]),
                IsaDecodeTree('pulp_nn', [pulp_nn]),
                IsaDecodeTree('rnnext', [pulp_v2_rnnext]),
                IsaDecodeTree('f', [rv32f]),
                IsaDecodeTree('sfloat', [Xf16, Xf16alt, Xf8, Xfvec, Xfaux]),
                IsaDecodeTree('gap8', [gap8]),
                IsaDecodeTree('gap9', [gap9]),
                IsaDecodeTree('int64', [int64]),
                #IsaTree('fpud', rv32d),
                #IsaTree('gap8', gap8),
                #IsaTree('priv_pulp_v2', priv_pulp_v2),
                #IsaTree('priv_1_9', priv_1_9)
                #IsaTree('rv32a', rv32a),
                #IsaTree('pulp_zeroriscy', pulp_zeroriscy),
            ]
        )

        for insn in self.isa.get_insns():

            if "load" in insn.tags:
                insn.get_out_reg(0).set_latency(2)
            elif "fdiv" in insn.tags:
                insn.get_out_reg(0).set_latency(9)
            elif "sfdiv" in insn.tags:
                insn.get_out_reg(0).set_latency(4)
            elif "mul" in insn.tags:
                insn.get_out_reg(0).set_latency(2)
            elif "mulh" in insn.tags:
                insn.set_latency(5)



class Zeroriscy(Riscv_core):

    def __init__(self):
        super(Zeroriscy, self).__init__()
        self.isa = Isa(
            'riscv',
            [
                IsaDecodeTree('pulp_v2', [priv_pulp_v2]),
                IsaDecodeTree('i', [rv32i]),
                IsaDecodeTree('m', [rv32m]),
                IsaDecodeTree('c', [rv32c]),
                IsaDecodeTree('priv', [priv]),
                IsaDecodeTree('pulp_v2', [pulp_v2]),
                IsaDecodeTree('rnnext', [pulp_v2_rnnext])
            ]
        )

        for insn in self.isa.get_insns():

            if "load" in insn.tags:
                insn.get_out_reg(0).set_latency(2)
            elif "fdiv" in insn.tags:
                insn.get_out_reg(0).set_latency(9)
            elif "sfdiv" in insn.tags:
                insn.get_out_reg(0).set_latency(4)
            elif "mul" in insn.tags:
                insn.get_out_reg(0).set_latency(3)
            elif "mulh" in insn.tags:
                insn.set_latency(5)
