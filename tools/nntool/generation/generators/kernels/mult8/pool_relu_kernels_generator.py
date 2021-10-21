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
from utils.node_id import NodeId

from generation.at_types.at_params import (NO_ACTIVATION, gen_active_at_params,
                                           gen_pool_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generator_decorators import (QREC_MULT8,
                                             generation_function)
from graph.types import ActivationFusionBase, PoolingParameters
from graph.types.pooling import AveragePoolParameters, MaxPoolParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (MaxPoolParameters, AveragePoolParameters, ActivationFusionBase), qrec_types=(QREC_MULT8, ))
def pool_act_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if isinstance(node, ActivationFusionBase):
        cnodes = node.contained_nodes()
        if isinstance(cnodes[0], PoolingParameters):
            act_q = gen.G.quantization[NodeId(node, cnodes[1])]
            gen.kernels.append(PoolKernel(node.name, cname, cnodes[0], cnodes[1], qrec, act_q,
                                          at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
            return True
        return False
    gen.kernels.append(PoolKernel(node.name, cname, node, None,
                                  qrec, at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
    return True


def gen_cnn_pool_act_sq8(code_block, cname, ctrl, feat, width, height, at_pool_params, actoper):
    code_block.write('CNN_PoolAct_SQ8("{}", {}, {}, {}, {},'.format(
        cname, ctrl, feat, width, height))
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {}, {});'.format(at_pool_params.PoolOper,
                                                                   at_pool_params.Fpx,
                                                                   at_pool_params.Fpy,
                                                                   at_pool_params.Dpx,
                                                                   at_pool_params.Dpy,
                                                                   at_pool_params.Spx,
                                                                   at_pool_params.Spy,
                                                                   at_pool_params.PoolPad,
                                                                   actoper))
    code_block.deindent()


class PoolKernel(AutotilerKernel):
    def __init__(self, node_name, cname, pool_params, act_params, qrec, act_q=None, gen_ctrl=None, at_ver=3, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl
        if pool_params.ker_in_order and pool_params.ker_in_order[0] == ["h", "w", "c"]:
            self.gen_ctrl.hwc = 1
        if not qrec.out_qs[0].signed:
            self.gen_ctrl.output_datasize = - qrec.out_qs[0].dtype_bits//8
        if not qrec.in_qs[0].signed:
            self.gen_ctrl.input_datasize = - qrec.in_qs[0].dtype_bits//8

        if act_params is not None:
            self.at_act_params = gen_active_at_params(act_params, force_relu=force_relu,
                                                      asymmetric=act_q.in_qs[0].zero_point != 0)
        else:
            self.at_act_params = NO_ACTIVATION

        pad_compatibilities = []
        self.at_pool_params = gen_pool_at_params(
            pool_params, pad_compatibilities)
        self.in_dim = pool_params.in_dims[0]
        self.out_dim = pool_params.out_dims[0]
        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)
            gen_ctrl = self.gen_ctrl.ctrl_name
        else:
            gen_ctrl = "0"

        gen_cnn_pool_act_sq8(code_block, self.cname, gen_ctrl, self.in_dim.c,
                             self.in_dim.w, self.in_dim.h,
                             self.at_pool_params,
                             self.at_act_params.ReLUOper)
        return code_block
