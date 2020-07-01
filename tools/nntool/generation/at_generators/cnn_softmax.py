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

GEN_SOFTMAX = "CNN_SoftMax"
# extern void CNN_SoftMax(
#         char *Name,
# 	CNN_GenControl_T *Ctrl,
#         int In_DataSize,
#         int Out_DataSize,
#         int In_InL3,
#         int Out_InL3,
#         int Dim,
#         KernelOper_T SoftMaxOper
#         );

# pylint: disable=too-many-arguments

SoftMaxATParam = namedtuple('SoftMaxATParam', [
    "SoftMaxOper"
])


def gen_softmax_at_params(_):
    return SoftMaxATParam(
        SoftMaxOper="KOP_SOFTMAX"
    )


def gen_at_softmax(code_block, name, in_q, out_q,
                   in_dim, at_softmax, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {});',
                     GEN_SOFTMAX, name, gen_ctrl,
                     at_bits(in_q), at_bits(out_q), in_q.q, out_q.q, in_dim.size(), at_softmax.SoftMaxOper)
