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

from collections import namedtuple
from .utils import at_bits

GEN_3D_TRANSPOSE = "CNN_3DTensorPermute"

ThreeDTensorTransposeATParam = namedtuple('ThreeDTensorTransposeATParam', [
    'MatPermOper'
])

def gen_3d_transpose_at_params(params):
    if params.transpose_dimension == 2:
        perm = params.permute(['H', 'W'])
        permop = "KOP_MATPERM_CHW2C{}".format("".join(perm))
    else:
        perm = params.permute(['C', 'H', 'W'])
        permop = "KOP_MATPERM_CHW2{}".format("".join(perm))
    return ThreeDTensorTransposeATParam(
        MatPermOper=permop
    )

def gen_at_3d_transpose(code_block, name, in_q, out_q,
                        in_shape, at_transpose_params, gen_ctrl=None,
                        at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {});',
                     GEN_3D_TRANSPOSE, name, gen_ctrl, at_bits(in_q), at_bits(out_q),
                     in_q.q, out_q.q, in_shape[0], in_shape[1], in_shape[2],
                     at_transpose_params.MatPermOper)
