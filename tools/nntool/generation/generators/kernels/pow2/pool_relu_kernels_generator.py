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

from generation.at_generators import (NO_ACTIVATION, NO_POOL,
                                      gen_active_at_params, gen_at_pool_relu,
                                      gen_pool_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_POW2
from graph.dim import PadDim
from graph.types import ConvFusionParameters
from utils.node_id import NodeId

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (ConvFusionParameters,), qrec_types=(QREC_POW2, ))
def pool_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams, qrec
    if isinstance(node, ConvFusionParameters) and node.fusion_type == "pool_active":
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        gen.kernels.append(PoolReluKernel(node.name, cname, cnodes[0], quants[0],
                                          cnodes[1], quants[1], at_ver=gen.opts['at_ver'],
                                          gen_ctrl=node.get_gen_ctrl()))
        return True
    return False


class PoolReluKernel(AutotilerKernel):
    def __init__(self, node_name, cname, pool_params, pool_q,
                 act_params, act_q, code_block=None, at_ver=3, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        in_q = out_q = None
        in_dim = out_dim = None
        pad_compatibilities = []

        if pool_params is not None:
            at_pool_params = gen_pool_at_params(pool_params, pad_compatibilities)
            if in_dim is None:
                in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
            if in_q is None:
                in_q = pool_q.in_qs[0]
            out_q = pool_q.out_qs[0]
        else:
            at_pool_params = NO_POOL

        if act_params is not None:
            at_act_params = gen_active_at_params(act_params)
            if in_dim is None:
                in_dim = act_params.in_dims[0]
            if out_dim is None:
                out_dim = act_params.out_dims[0]
            if in_q is None:
                in_q = act_q.in_qs[0]
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

        if code_block is None:
            code_block = CodeBlock()

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                self.gen_ctrl.PadType = at_pad_ctrl

        if in_q.bits != out_q.bits:
            raise NotImplementedError("only homogenious operations are supported at present")
        if at_pool_params == NO_POOL:
            raise NotImplementedError(
                "activation layer on its own should not be matched by this kernel")

        self.at_pool_params = at_pool_params
        self.in_dim = in_dim
        self.out_dim = out_dim
        self.in_q = in_q
        self.out_q = out_q
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

        gen_at_pool_relu(code_block, self.cname, self.in_q, self.out_q,
                         self.in_dim, self.out_dim, self.at_pool_params,
                         self.at_act_params, gen_ctrl=self.gen_ctrl,
                         at_ver=self.at_ver)

        return code_block
