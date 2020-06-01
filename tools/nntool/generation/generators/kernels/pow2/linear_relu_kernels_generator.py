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

from generation.at_generators import (NO_ACTIVATION,
                                      gen_active_at_params, gen_linear_at_params, gen_at_linear_relu)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_POW2
from graph.types import (FcParameters, ConvFusionParameters)
from utils.node_id import NodeId

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (ConvFusionParameters, FcParameters), qrec_types=(QREC_POW2, ))
def linear_relu_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if isinstance(node, FcParameters):
        gen.kernels.append(LinearReluKernel(node.name, cname, node, qrec, None, None,
                                            at_ver=gen.opts['at_ver'], gen_ctrl=node.get_gen_ctrl()))
    elif isinstance(node, ConvFusionParameters) and node.fusion_type == "linear_active":
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        gen.kernels.append(LinearReluKernel(node.name, cname, cnodes[0], quants[0],
                                            cnodes[1], quants[1], at_ver=gen.opts['at_ver'],
                                            gen_ctrl=node.get_gen_ctrl()))
    else:
        return False
    return True


class LinearReluKernel(AutotilerKernel):
    def __init__(self, node_name, cname, linear_params, linear_q, act_params, act_q, at_ver=3, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        assert linear_params is not None, "linear should always be included"
        at_linear_params = gen_linear_at_params(linear_params)
        in_dim = linear_params.in_dims[0]
        out_dim = linear_params.out_dims[0]
        filter_q = linear_q.weights_q
        in_q = linear_q.in_qs[0]
        out_q = linear_q.out_qs[0]
        bias_q = linear_q.biases_q

        if act_params is not None:
            at_act_params = gen_active_at_params(act_params)
            out_q = act_q.out_qs[0]
            if at_ver < 3:
                if act_params.activation == "relu6" and out_q.q != 0:
                    self.gen_ctrl.ReluN = 6 << out_q.q
                    self.gen_ctrl.ReluNNoNorm = 1
            else:
                if act_params.activation == "relun":
                    self.gen_ctrl.ReluN = act_params.activation_params
        else:
            at_act_params = NO_ACTIVATION

        self.at_linear_params = at_linear_params
        self.in_dim = in_dim
        self.out_dim = out_dim
        self.in_q = in_q
        self.bias_q = bias_q
        self.out_q = out_q
        self.filter_q = filter_q
        self.at_act_params = at_act_params
        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)

        gen_at_linear_relu(code_block, self.cname, self.in_q, self.out_q,
                           self.filter_q, self.bias_q,
                           self.in_dim, self.out_dim, self.at_linear_params, self.at_act_params,
                           at_ver=self.at_ver, gen_ctrl=self.gen_ctrl)

        return code_block
