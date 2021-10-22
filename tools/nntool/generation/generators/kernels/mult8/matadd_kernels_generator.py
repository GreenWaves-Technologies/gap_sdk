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

from functools import reduce
import logging
from utils.node_id import NodeId

from generation.at_types.at_params import (NO_ACTIVATION, gen_active_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generator_decorators import generation_function, QREC_MULT8
from graph.types import MatrixAddParameters, ActivationFusionBase

from ..autotiler_kernel import (AutotilerKernel, gen_include_paths,
                                gen_includes, gen_sources,
                                kernel_include_paths, kernel_includes,
                                kernel_sources)


LOG = logging.getLogger("nntool." + __name__)

MAT_ADD_OPER = "KOP_MATADD"


@generation_function("kernels", (MatrixAddParameters, ActivationFusionBase), qrec_types=(QREC_MULT8, ))
def matadd_kernel_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del out_eparams, qrec
    if isinstance(node, ActivationFusionBase):
        cnodes = node.contained_nodes()
        if isinstance(cnodes[0], MatrixAddParameters):
            if in_eparams[0].dims.size() != in_eparams[1].dims.size():
                raise ValueError(
                    "missing generator: the matrix add generator only handles adds of tensors of the same size")
            act_q = gen.G.quantization[NodeId(node, cnodes[1])]
            gen.kernels.append(MatAddKernel(node.name, cname, cnodes[0], cnodes[1], act_q, at_ver=gen.opts['at_ver'],
                                            force_relu=gen.force_relu))
            return True
        return False
    if in_eparams[0].dims.size() != in_eparams[1].dims.size():
        raise ValueError(
            "missing generator: the matrix add generator only handles adds of tensors of the same size")
    gen.kernels.append(MatAddKernel(node.name, cname, node, None,
                                    at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
    return True


def gen_mat_add_sq8(code_block, cname, ctrl, feat, width, height, act_oper):
    code_block.write('CNN_MatAddAct_SQ8("{}", {}, {}, {}, {}, {}, {});'.format(cname, ctrl,
                                                                               feat, width,
                                                                               height,
                                                                               MAT_ADD_OPER,
                                                                               act_oper))


def balanced_factors(num):
    factors = [(x, num//x) for x in range(2, int(num/2)+1) if num % x == 0]
    differences = [abs(x[0] - x[1]) for x in factors]
    min_idx = differences.index(min(differences))
    return factors[min_idx]


def make_three_dims(dims):
    if len(dims) == 1:
        factors = balanced_factors(dims[0])
        return (1, factors[0], factors[1])
    if len(dims) == 2:
        return (1, dims[0], dims[1])
    if len(dims) == 3:
        return dims
    prod = reduce(lambda x, y: x * y, dims[1:])
    factors = balanced_factors(prod)
    return (dims[0], factors[0], factors[1])


@kernel_sources(
    '$(TILER_CNN_KERNEL_PATH_SQ8)/CNN_MatAlgebra_SQ8.c')
@kernel_include_paths(
    '$(TILER_CNN_KERNEL_PATH)',
    '$(TILER_CNN_KERNEL_PATH_SQ8)')
@kernel_includes(
    'CNN_BasicKernels_SQ8.h')
@gen_sources(
    '$(TILER_CNN_GENERATOR_PATH)/CNN_Generator_Util.c',
    '$(TILER_CNN_GENERATOR_PATH_SQ8)/CNN_Generators_SQ8.c')
@gen_include_paths(
    '$(TILER_CNN_GENERATOR_PATH)',
    '$(TILER_CNN_GENERATOR_PATH_SQ8)')
@gen_includes(
    'CNN_Generators_SQ8.h')
class MatAddKernel(AutotilerKernel):
    def __init__(self, node_name, cname, matrixadd_params, act_params, act_q=None, at_ver=3, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

        if act_params is not None:
            self.at_act_params = gen_active_at_params(
                act_params, force_relu=force_relu, asymmetric=act_q.in_qs[0].zero_point != 0)
        else:
            self.at_act_params = NO_ACTIVATION

        self.matrixadd_params = matrixadd_params
        dimensions = make_three_dims(matrixadd_params.in_dims[0])
        self.feat_dim = dimensions[0]
        self.width = dimensions[1]
        self.height = dimensions[2]

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)
            gen_ctrl = self.gen_ctrl.ctrl_name
        else:
            gen_ctrl = "0"

        gen_mat_add_sq8(code_block, self.cname, gen_ctrl, self.feat_dim,
                        self.width, self.height, self.at_act_params.ReLUOper)

        return code_block
