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

from generation.at_types.at_params import NO_ACTIVATION, gen_activation_op
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generator_decorators import (QREC_FLOAT, generation_function)
from graph.types import (ActivationParameters, MatMulOpFusionParameters,
                         MatMulOpParameters)

from ..autotiler_kernel import (AutotilerKernel, gen_include_paths,
                                gen_includes, gen_sources,
                                kernel_include_paths, kernel_includes,
                                kernel_sources)

LOG = logging.getLogger("nntool." + __name__)

MAT_MUL_OPER = "KOP_MATMUL"


@generation_function(
    "kernels",
    (MatMulOpParameters, MatMulOpFusionParameters),
    qrec_types=(QREC_FLOAT, ))
def mat_mul_kernel_generator_fp16(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams, qrec
    if isinstance(node, MatMulOpFusionParameters):
        cnodes = node.contained_nodes()
        act_node = cnodes[-1] if isinstance(cnodes[-1],
                                            ActivationParameters) else None
    else:
        act_node = None
    gen.kernels.append(
        MatMulKernel(
            node.name, cname, node, act_node, at_ver=gen.opts['at_ver'],
            force_relu=gen.force_relu))
    return True


def gen_mat_mul_fp16(code_block, cname, ctrl, colM1, lineM1, colM2, lineM2, act_oper):
    code_block.write('CNN_MatMulAct_fp16("{}", {}, {}, {}, {}, {}, 0, 0, 1, 1, {}, {});'.format(
        cname, ctrl,
        colM1, lineM1,
        colM2, lineM2,
        MAT_MUL_OPER,
        act_oper))


@kernel_sources(
    '$(TILER_CNN_KERNEL_PATH_FP16)/CNN_MatAlgebra_fp16.c')
@kernel_include_paths(
    '$(TILER_CNN_KERNEL_PATH_FP16)')
@kernel_includes(
    'CNN_BasicKernels_fp16.h')
@gen_sources(
    '$(TILER_CNN_GENERATOR_PATH)/CNN_Generator_Util.c',
    '$(TILER_CNN_GENERATOR_PATH_FP16)/CNN_Generators_fp16.c')
@gen_include_paths(
    '$(TILER_CNN_GENERATOR_PATH)',
    '$(TILER_CNN_GENERATOR_PATH_FP16)')
@gen_includes(
    'CNN_Generators_fp16.h')
class MatMulKernel(AutotilerKernel):
    def __init__(self, node_name, cname, matmul_params, act_params, at_ver=3, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

        if act_params is not None:
            self.at_act_params = gen_activation_op(
                act_params.activation, force_relu=force_relu)
        else:
            self.at_act_params = NO_ACTIVATION

        self.matmul_params = matmul_params
        self.lineM1 = matmul_params.in_dims[0][0]
        self.colM1 = matmul_params.in_dims[0][1]
        self.lineM2 = matmul_params.in_dims[1][0]
        self.colM2 = matmul_params.in_dims[1][1]

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)
            gen_ctrl = self.gen_ctrl.ctrl_name
        else:
            gen_ctrl = "0"

        gen_mat_mul_fp16(code_block, self.cname, gen_ctrl, self.colM1,
                        self.lineM1, self.colM2, self.lineM2, self.at_act_params)

        return code_block
