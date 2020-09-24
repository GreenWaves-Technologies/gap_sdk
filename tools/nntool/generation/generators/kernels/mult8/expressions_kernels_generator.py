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

from generation.code_block import CodeBlock
from generation.generators.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from graph.types import ExpressionFusionParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (ExpressionFusionParameters, ), qrec_types=(QREC_MULT8, ))
def expressions_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams, qrec
    func_name, _ = gen.expressions_get_names(node)
    gen.kernels.append(ExpressionKernel(node.name, func_name + "_gen", cname))
    return True


def gen_expression_sq8(code_block, gen_name, cname):
    code_block.write('{}("{}");'.format(gen_name, cname))


class ExpressionKernel(AutotilerKernel):
    def __init__(self, node_name, gen_name, cname):
        self.node_name = node_name
        self.gen_name = gen_name
        self.cname = cname

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        gen_expression_sq8(code_block, self.gen_name, self.cname)
        return code_block
