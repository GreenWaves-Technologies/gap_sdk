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
from .utils import at_bits, at_q
from .cnn_convolution_pool_relu import gen_activation_op

GEN_MATADD = "CNN_MatAdd"
GEN_MATADDDYN = "CNN_MatAddDynAdjust"
GEN_MATSCALE = "CNN_MatScale"


MatrixAddATParam = namedtuple('MatrixAddATParam', [
    "MatrixAddOper"
])

def gen_matrixadd_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD"
    )

def gen_at_matrixadd(code_block, name, in_q1, in_q2, out_q,
                     in_dim, out_dim, at_matrixadd, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                         GEN_MATADD, name, gen_ctrl,
                         at_bits(in_q1), at_bits(in_q2), at_bits(
                             out_q), in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                         GEN_MATADD, name, gen_ctrl,
                         at_bits(in_q1), at_bits(in_q2), at_bits(out_q),
                         in_q1.q, in_q2.q, out_q.q, in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)

# pylint: disable=too-many-arguments

def gen_matrixadddyn_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD_DYNADJUST"
    )
    
def gen_at_matrixadddyn(code_block, name, in_q1, in_q2, out_q,
                        in_dim, out_dim, at_matrixadd, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, {}, {}, {}, {}, {});',
                     GEN_MATADDDYN, name, gen_ctrl,
                     at_bits(in_q1), at_bits(in_q2), at_bits(out_q),
                     in_q1.q, in_q2.q, out_q.q,
                     in_dim.shape[0], out_dim.shape[0],
                     in_dim.shape[1], in_dim.shape[2], at_matrixadd.MatrixAddOper)

# pylint: disable=too-many-arguments

MatScaleATParam = namedtuple('MatScaleATParam', [
    "ScaleOper",
    "ReLUOper"
])

def gen_matscale_at_params(params):
    # KOP_MATSCALE_VECTOR, KOP_MATSCALE_SCALAR or KOP_MATSCALE_VECTOR_SCALAR
    if params.fusion_type == "vec_scalar":
        ms_op = 'KOP_MATSCALE_VECTOR_SCALAR'
    elif params.fusion_type == "vector":
        ms_op = 'KOP_MATSCALE_VECTOR'
    elif params.fusion_type == "scalar":
        ms_op = 'KOP_MATSCALE_SCALAR'
    else:
        raise NotImplementedError("unknown fusion type %s" % params.fusion_type)
    return MatScaleATParam(
        ScaleOper=ms_op,
        ReLUOper=gen_activation_op(params.activation)
    )

def gen_at_matscale(code_block, name, other_q, vector_q, scalar_q, out_q,
                    in_dim, out_dim, at_matscale, gen_ctrl=None):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {}, {}, {});',
                     GEN_MATSCALE, name, gen_ctrl,
                     at_bits(other_q), at_bits(vector_q), at_bits(scalar_q), at_bits(out_q),
                     at_q(other_q), at_q(vector_q), at_q(scalar_q), at_q(out_q),
                     in_dim.shape[0], out_dim.shape[0],
                     in_dim.shape[2], in_dim.shape[1], at_matscale.ScaleOper, at_matscale.ReLUOper)