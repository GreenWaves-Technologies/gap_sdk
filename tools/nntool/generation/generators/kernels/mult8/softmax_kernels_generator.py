# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import logging

from generation.at_types.at_params import gen_softmax_at_params
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import SoftMaxParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)

GEN_SOFTMAX = "CNN_SoftMax_SQ8"
# extern void CNN_SoftMax(
#         char *Name,
#         CNN_GenControl_T *Ctrl,
#         int Dim,
#         KernelOper_T SoftMaxOper
#         );

def gen_at_softmax(code_block, name, in_dim, at_softmax_params, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {});',
                     GEN_SOFTMAX, name, gen_ctrl,
                     in_dim.size(), at_softmax_params)


@generation_function("kernels", (SoftMaxParameters, ), qrec_types=(QREC_MULT8, ))
def softmax_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    gen.kernels.append(SoftmaxKernel(cname, node, qrec, at_ver=gen.opts['at_ver']))
    return True


class SoftmaxKernel(AutotilerKernel):
    def __init__(self, cname, params, qrec, gen_ctrl=None, at_ver=3):
        del qrec
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        self.at_softmax_params = gen_softmax_at_params(params)
        self.in_dim = params.in_dims[0]
        self.cname = cname
        self.node_name = params.name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)

        gen_at_softmax(code_block, self.cname, self.in_dim,
                       self.at_softmax_params.SoftMaxOper, at_ver=self.at_ver)
        return code_block
