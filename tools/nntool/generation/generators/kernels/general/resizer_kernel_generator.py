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
from generation.generators.generator_decorators import generation_function
from graph.types import ResizerParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)

RESIZE_KOP = {"bilinear": "KOP_BILINEAR_RESIZE",
              "nearest_neighbor": "KOP_NEAREST_NEIGHBOR_RESIZE"}


def gen_at_resizer(code_block, name, in_dim, new_shape, resize_kop):
    if in_dim.has_key('w') and in_dim.has_key('h'):
        code_block.write('GenerateResizeNew("{}", {}, {}, {}, {}, {});',
                         name, in_dim.w, in_dim.h, new_shape[1], new_shape[0], resize_kop)
    else:
        code_block.write('GenerateResizeNew("{}", {}, {}, {}, {}, {});',
                         name, in_dim.shape[1], in_dim.shape[0], new_shape[1], new_shape[0], resize_kop)


@generation_function("kernels", (ResizerParameters, ))
def resize_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams, qrec
    gen.kernels.append(ResizeKernel(cname, node))
    return True


class ResizeKernel(AutotilerKernel):
    def __init__(self, cname, params):
        self.in_dim = params.in_dims[0]
        self.cname = cname
        self.node_name = params.name
        self.type = params.op_name
        self.new_shape = params.new_shape

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        gen_at_resizer(code_block, self.cname, self.in_dim,
                       self.new_shape, RESIZE_KOP[self.type])
        return code_block
