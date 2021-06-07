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
from generation.generator_decorators import generation_function
from graph.types import ResizerParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)

RESIZE_KOP = {"bilinear": "KOP_BILINEAR_RESIZE",
              "nearest_neighbor": "KOP_NEAREST_NEIGHBOR_RESIZE"}


def gen_at_resizer(code_block, name, in_dim, new_shape, inout_t, resize_kop, q16mode):
    if in_dim.has_key('w') and in_dim.has_key('h'):
        in_dim_w, in_dim_h, in_dim_c = in_dim.w, in_dim.h, in_dim.c
    else:
        in_dim_w, in_dim_h, in_dim_c = in_dim.shape[2], in_dim.shape[1], in_dim.shape[0]

    win, wout = in_dim_w, new_shape[1]
    hin, hout = in_dim_h, new_shape[0]
    chin = in_dim_c
    if in_dim_w == 1 and in_dim_h == 1:
        # If both are 1s the autotiler cannot solve the tiling problem
        # --> exchange channels with one of the HW dimensions (the one that does not change)
        if 1 in new_shape:
            idx_one = new_shape.index(1)
            win, wout = (in_dim_w, new_shape[1]) if idx_one == 0 else (in_dim_c, in_dim_c)
            hin, hout = (in_dim_h, new_shape[0]) if idx_one == 1 else (in_dim_c, in_dim_c)
            chin = 1
        else:
            # If both HW change from 1x1 to HxW this is not going to work
            LOG.warning(f"Resize Node {name} has 1x1xc input but resizes both HW dimension, could not work in autotiler")

    GenKernel = "GenerateResizeMultiChannelQ16" if q16mode else "GenerateResizeMultiChannel"
    code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {});', GenKernel,
                     name, win, hin, wout, hout, chin, inout_t, resize_kop)


@generation_function("kernels", (ResizerParameters, ))
def resize_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    gen.kernels.append(ResizeKernel(cname, node, qrec))
    return True


class ResizeKernel(AutotilerKernel):
    def __init__(self, cname, params, qrec):
        self.in_dim = params.in_dims[0]
        self.cname = cname
        self.node_name = params.name
        self.inout_type = "SIGNED_INOUT" if qrec.in_qs[0].signed else "UNSIGNED_INOUT"
        self.type = params.op_name
        self.new_shape = params.new_shape
        self.q16 = True if qrec.in_qs[0].bits == 16 else False

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        gen_at_resizer(code_block, self.cname, self.in_dim,
                       self.new_shape, self.inout_type, RESIZE_KOP[self.type], self.q16)
        return code_block
