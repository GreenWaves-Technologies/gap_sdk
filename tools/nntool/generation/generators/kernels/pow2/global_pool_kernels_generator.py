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
from collections import namedtuple
from functools import reduce

from generation.at_generators.utils import at_bits
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import (QREC_POW2,
                                                        generation_function)
from graph.types import GlobalPoolParameters
from utils.largest_factor import balanced_divisors

from ..autotiler_kernel import AutotilerKernel

GEN_GLOBALPOOL = "CNN_GlobalPool"
LOG = logging.getLogger("nntool." + __name__)

POOL_OPERS = {
    "average": "KOP_GLOBAL_AVGPOOL",
    "max": "KOP_GLOBAL_MAXPOOL",
}


@generation_function("kernels",
                     (GlobalPoolParameters, ),
                     qrec_types=(QREC_POW2, ))
def global_pool_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    gen.kernels.append(GlobalPoolKernel(node.name, cname, node, qrec, at_ver=gen.opts['at_ver']))
    return True

GlobalPoolATParam = namedtuple('GlobalPoolATParam', [
    "GlobalPoolOper"
])

def gen_globalpool_at_params(params):
    oper = POOL_OPERS.get(params.pool_type)
    if not oper:
        raise NotImplementedError("generation for %s reduction is not implemented")
    return GlobalPoolATParam(
        GlobalPoolOper=oper
    )

def gen_at_globalpool(code_block, name, in_q, out_q,
                      c, h, w, at_globalpool, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name
        #raise NotImplementedError("genctrl is not yet implemented")

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {}, {});',
                         GEN_GLOBALPOOL, name, gen_ctrl,
                         at_bits(in_q), at_bits(out_q), c, c,
                         h, w, at_globalpool.GlobalPoolOper)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {}, {});',
                         GEN_GLOBALPOOL, name, gen_ctrl,
                         at_bits(in_q), at_bits(
                             out_q), in_q.q, out_q.q, c, c,
                         h, w, at_globalpool.GlobalPoolOper)

class GlobalPoolKernel(AutotilerKernel):
    def __init__(self, node_name, cname, params, qrec, gen_ctrl=None, at_ver=3):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        self.at_globalpool_params = gen_globalpool_at_params(params)
        in_dim = params.in_dims[0]
        reduce_sz = reduce(lambda x, y: x * y, (sz for idx, sz in enumerate(in_dim.shape)
                                                if idx not in params.axis))
        #self.c = in_dim.size()/reduce_sz
        self.c = reduce_sz
        (self.h, self.w) = balanced_divisors(in_dim.size()/reduce_sz)
        self.in_q = qrec.in_qs[0]
        self.out_q = qrec.out_qs[0]
        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)

        gen_at_globalpool(code_block, self.cname, self.in_q, self.out_q,
                          self.c, self.h, self.w, self.at_globalpool_params,
                          at_ver=self.at_ver)
        return code_block
