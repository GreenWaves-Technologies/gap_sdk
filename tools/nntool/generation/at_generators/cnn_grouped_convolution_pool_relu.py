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

GEN_GROUPED_CONV_POOL_RELU = "CNN_GroupedConvolutionPoolReLU"

# ConvOper:       Type of convolution, Regular convolution: KOP_CONV,
#                 Regular convolution with double precision output: KOP_CONV_DP,
#                 Depth wise convolution: KOP_CONV_DW
# GroupIn:        Size of the group for input features
# GroupOut:       Size of the group for output features
# Fcx:            Convolution filter x dimension
# Fcy:            Convolution filter y dimension
# Dcx:            Convolution filter dilation factor, x dimension
# Dcy:            Convolution filter dilation factor, y dimension
# Scx:            Convolution filter stride x dimension
# Scy:            Convolution filter stride y dimension
# ConvPad:        0: No padding, 1: Zero padding

GroupedConvATParam = namedtuple('GroupedConvATParam', [
    "ConvOper",
    "GroupIn",
    "GroupOut",
    "Fcx",
    "Fcy",
    "Dcx",
    "Dcy",
    "Scx",
    "Scy",
    "ConvPad"
])

# extern void CNN_GroupedConvolutionPoolReLU(
# 	char         *Name,

# 	CNN_GenControl_T *Ctrl,

#   GroupIn:        Size of the group for input features
#   GroupOut:       Size of the group for output features

# 	int In_DataSize,
# 	int Filter_DataSize,
# 	int Bias_DataSize,
# 	int Out_DataSize,

# 	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
# 	int Filter_InL3,
# 	int Bias_InL3,
# 	int Out_InL3,

# 	int InFeat,
# 	int OutFeat,
# 	int Width,
# 	int Height,

# 	KernelOper_T ConvOper,
# 	int Fcx,
# 	int Fcy,
# 	int Dcx,
# 	int Dcy,
# 	int Scx,
# 	int Scy,
# 	int          ConvPad,

# 	KernelOper_T PoolOper,
# 	int Fpx,
# 	int Fpy,
# 	int Dpx,
# 	int Dpy,
# 	int Spx,
# 	int Spy,
# 	int          PoolPad,

#        	KernelOper_T ReLUOper
# 	);

# pylint: disable=too-many-arguments


def gen_at_grouped_conv_pool_relu(code_block, name, in_q, out_q,
                                  filt_q, bias_q, in_dim, out_dim,
                                  at_conv, at_pool, at_active, gen_ctrl=None,
                                  at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                         GEN_GROUPED_CONV_POOL_RELU, name, gen_ctrl,
                         at_conv.GroupIn, at_conv.GroupOut,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_dim.c, out_dim.c, in_dim.w, in_dim.h)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {}, {}, {},',
                         GEN_GROUPED_CONV_POOL_RELU, name, gen_ctrl,
                         at_conv.GroupIn, at_conv.GroupOut,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_q.q, filt_q.q, bias_q.q, out_q.q,
                         in_dim.c, out_dim.c, in_dim.w, in_dim.h)
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     at_conv.ConvOper, at_conv.Fcx, at_conv.Fcy,
                     at_conv.Dcx, at_conv.Dcy, at_conv.Scx, at_conv.Scy,
                     at_conv.ConvPad)
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {}, {});',
                     at_pool.PoolOper, at_pool.Fpx, at_pool.Fpy,
                     at_pool.Dpx, at_pool.Dpy, at_pool.Spx, at_pool.Spy,
                     at_pool.PoolPad, at_active.ReLUOper)
    code_block.deindent()
