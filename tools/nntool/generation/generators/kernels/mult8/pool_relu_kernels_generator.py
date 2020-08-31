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

from generation.at_types.at_params import (gen_pool_at_params, gen_active_at_params, NO_ACTIVATION)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import PoolingParameters, ActivationFusion

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (PoolingParameters, ActivationFusion), qrec_types=(QREC_MULT8, ))
def pool_act_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams, qrec
    if isinstance(node, ActivationFusion):
        cnodes = node.contained_nodes()
        if isinstance(cnodes[0], PoolingParameters):
            gen.kernels.append(PoolKernel(node.name, cname, cnodes[0], cnodes[1],
                                          at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
            return True
        return False
    gen.kernels.append(PoolKernel(node.name, cname, node, None, at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
    return True


def gen_cnn_pool_act_sq8(code_block, cname, ctrl, feat, width, height, at_pool_params, actoper):
    code_block.write('CNN_PoolAct_SQ8("{}", {}, {}, {}, {},'.format(cname, ctrl, feat, width, height))
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
    def __init__(self, node_name, cname, pool_params, act_params, gen_ctrl=None, at_ver=3, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        if act_params is not None:
            self.at_act_params = gen_active_at_params(act_params, force_relu=force_relu)
        else:
            self.at_act_params = NO_ACTIVATION

        pad_compatibilities = []
        self.at_pool_params = gen_pool_at_params(pool_params, pad_compatibilities)
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
