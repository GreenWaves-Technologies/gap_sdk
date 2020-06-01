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

GEN_LINEAR_RELU = "CNN_LinearReLU"
# extern void CNN_LinearReLU(
#         char *Name,

# 	CNN_GenControl_T *Ctrl,

#         int In_DataSize,
#         int Filter_DataSize,
#         int Bias_DataSize,
#         int Out_DataSize,

#         int In_InL3,
#         int Filter_InL3,
#         int Bias_InL3,
#         int Out_InL3,

#         int InDim,
#         int OutDim,

#         KernelOper_T LinearOper,
#         KernelOper_T ReLUOper
#         );

# pylint: disable=too-many-arguments


LinearATParam = namedtuple('LinearATParam', [
    "LinearOper"
])


def gen_linear_at_params(_):
    return LinearATParam(
        LinearOper="KOP_LINEAR"
    )


def gen_at_linear_relu(code_block, name, in_q, out_q,
                       filt_q, bias_q, in_dim, out_dim,
                       at_linear, at_active, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {},',
                         GEN_LINEAR_RELU, name, gen_ctrl,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_dim.size(), out_dim.size())
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, {}, {},',
                         GEN_LINEAR_RELU, name, gen_ctrl,
                         at_bits(in_q), at_bits(filt_q), at_bits(bias_q), at_bits(out_q),
                         in_q.q, filt_q.q, bias_q.q, out_q.q,
                         in_dim.size(), out_dim.size())
    code_block.indent()
    code_block.write('{}, {});',
                     at_linear.LinearOper, at_active.ReLUOper)
    code_block.deindent()
