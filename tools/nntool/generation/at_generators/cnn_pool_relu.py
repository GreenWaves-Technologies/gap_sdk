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

from .utils import at_bits

GEN_POOL_RELU = "CNN_PoolReLU"

# extern void CNN_PoolReLU(
# 	char         *Name,

# 	CNN_GenControl_T *Ctrl,

# 	int In_DataSize,
# 	int Out_DataSize,

# 	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
# 	int Out_InL3,

# 	int InFeat,
# 	int OutFeat,
# 	int Width,
# 	int Height,

# 	KernelOper_T PoolOper,
# 	int Fpx,
# 	int Fpy,
# 	int Dpx,
# 	int Dpy,
# 	int Spx,
# 	int Spy,
# 	int          PoolPad,

# 	KernelOper_T ReLUOper
# 	);

# pylint: disable=too-many-arguments

def gen_at_pool_relu(code_block, name, in_q, out_q, in_dim,
                     out_dim, at_pool, at_active, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_pool.PoolOper == 'KOP_NONE':
        if in_dim.is_named and in_dim.has_keys(['c', 'w', 'h']):
            dims = [in_dim.c, in_dim.h, in_dim.w, in_dim.c]
        else:
            dims = in_dim.shape.copy()
            dims = dims + [1] * (4 - len(dims))

        if out_dim.is_named and out_dim.has_key('c'):
            dims[3] = out_dim.c
        else:
            dims[3] = dims[0]
    else:
        dims = [in_dim.c, in_dim.h, in_dim.w, out_dim.c]

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {},',
                         GEN_POOL_RELU, name, gen_ctrl, at_bits(in_q), at_bits(out_q),
                         dims[0], dims[3], dims[2], dims[1])
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {},',
                         GEN_POOL_RELU, name, gen_ctrl, at_bits(
                             in_q), at_bits(out_q), in_q.q, out_q.q,
                         dims[0], dims[3], dims[2], dims[1])
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {}, {});',
                     at_pool.PoolOper, at_pool.Fpx, at_pool.Fpy,
                     at_pool.Dpx, at_pool.Dpy, at_pool.Spx, at_pool.Spy,
                     at_pool.PoolPad, at_active.ReLUOper)
    code_block.deindent()
