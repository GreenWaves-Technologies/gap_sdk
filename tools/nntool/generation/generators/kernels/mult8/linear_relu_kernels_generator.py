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

from generation.at_types.at_params import (NO_ACTIVATION, gen_active_at_params,
                                           gen_linear_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from graph.types import ConvFusionParameters, FcParameters
from utils.node_id import NodeId

from ..autotiler_kernel import (AutotilerKernel, gen_include_paths,
                                gen_includes, gen_sources,
                                kernel_include_paths, kernel_includes,
                                kernel_sources)

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (ConvFusionParameters, FcParameters), qrec_types=(QREC_MULT8, ))
def linear_relu_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if isinstance(node, FcParameters):
        gen.kernels.append(LinearReluKernel(node.name, cname, node, qrec, None, None,
                                            at_ver=gen.opts['at_ver'], gen_ctrl=node.get_gen_ctrl(),
                                            force_relu=gen.force_relu))
    elif isinstance(node, ConvFusionParameters) and node.fusion_type == "linear_active":
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        gen.kernels.append(LinearReluKernel(node.name, cname, cnodes[0], quants[0],
                                            cnodes[1], quants[1], at_ver=gen.opts['at_ver'],
                                            gen_ctrl=node.get_gen_ctrl(), force_relu=gen.force_relu))
    else:
        return False
    return True

def gen_at_linear_relu(code_block, cname, biases_ds, mulbiases_ds,
                       in_dim, out_dim, linear_oper, act_oper, gen_ctrl, at_ver=3):
    del at_ver
    code_block.write('CNN_LinearAct_SQ8("{}", {}, {}, {}, {}, {}, {}, {});',
                     cname,
                     gen_ctrl,
                     biases_ds,
                     mulbiases_ds,
                     in_dim,
                     out_dim,
                     linear_oper,
                     act_oper)

def gen_at_linear_relu_ne16(code_block, cname, in_size, out_size, filter_bits, biases_ds, mulbiases_ds,
                            in_dim, out_dim, linear_oper, act_oper, gen_ctrl, at_ver=3):
    del at_ver
    code_block.write('CNN_LinearAct_NE16("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, {});',
                     cname,
                     gen_ctrl,
                     in_size,
                     out_size,
                     biases_ds,
                     mulbiases_ds,
                     filter_bits,
                     in_dim,
                     out_dim,
                     linear_oper,
                     act_oper)

@kernel_sources(
    '$(TILER_CNN_KERNEL_PATH_SQ8)/CNN_Bias_Linear_SQ8.c')
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
class LinearReluKernel(AutotilerKernel):
    def __init__(self, node_name, cname, linear_params, linear_q, act_params, act_q, at_ver=3, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        assert linear_params is not None, "linear should always be included"
        at_linear_params = gen_linear_at_params(linear_params)
        in_dim = linear_params.in_dims[0]
        out_dim = linear_params.out_dims[0]
        filter_q = linear_q.in_qs[1]
        in_q = linear_q.in_qs[0]
        out_q = linear_q.out_qs[0]
        bias_q = linear_q.in_qs[2]
        mulbiases_q = linear_q.cache['mul_biases_q']

        if act_params is not None:
            at_act_params = gen_active_at_params(act_params, force_relu=force_relu)
            if in_dim is None:
                in_dim = act_params.in_dims[0]
            if out_dim is None:
                out_dim = act_params.out_dims[0]
            if in_q is None:
                in_q = act_q.in_qs[0]
            out_q = act_q.out_qs[0]
        else:
            at_act_params = NO_ACTIVATION

        self.at_linear_params = at_linear_params
        self.in_dim = in_dim.size()
        self.out_dim = out_dim.size()
        self.in_q = in_q
        self.bias_q = bias_q
        self.mulbiases_q = mulbiases_q
        self.out_q = out_q
        self.filter_q = filter_q
        self.at_act_params = at_act_params
        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver
        self.ne16 = linear_q.cache.get('ne16')

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)
            gen_ctrl = self.gen_ctrl.ctrl_name
        else:
            gen_ctrl = "0"

        if self.ne16:
            gen_at_linear_relu_ne16(code_block, self.cname,
                                    self.in_q.bits//8 if self.in_q.signed else -self.in_q.bits//8,
                                    self.out_q.bits//8 if self.out_q.signed else -self.out_q.bits//8,
                                    self.filter_q.bits, self.bias_q.bits//8, self.mulbiases_q.bits//8,
                                    self.in_dim, self.out_dim,
                                    self.at_linear_params.LinearOper,
                                    self.at_act_params.ReLUOper,
                                    at_ver=self.at_ver, gen_ctrl=gen_ctrl)
        else:
            gen_at_linear_relu(code_block, self.cname, self.bias_q.bits//8, self.mulbiases_q.bits//8,
                               self.in_dim, self.out_dim,
                               self.at_linear_params.LinearOper,
                               self.at_act_params.ReLUOper,
                               at_ver=self.at_ver, gen_ctrl=gen_ctrl)

        return code_block
