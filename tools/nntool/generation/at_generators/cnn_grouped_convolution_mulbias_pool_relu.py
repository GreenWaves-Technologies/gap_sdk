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

GEN_GROUPED_MULCONV_POOL_RELU = "CNN_GroupedConvolutionMulBiasPoolReLU"

def gen_at_grouped_mulconv_pool_relu(code_block, name, in_q, out_q,
                                     filt_q, bias_q, mul_biases_q, in_dim, out_dim,
                                     at_conv, at_pool, at_active, gen_ctrl=None,
                                     at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name

    if at_ver < 3:
        raise NotImplementedError("mulbias before ver 3 not supported")

    code_block.write('{}("{}", {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 1, 1, 1, 1, 1, {}, {}, {}, {},',
                     GEN_GROUPED_MULCONV_POOL_RELU, name, gen_ctrl,
                     at_conv.GroupIn, at_conv.GroupOut,
                     at_bits(in_q), at_bits(filt_q), at_bits(bias_q),
                     at_bits(mul_biases_q), at_bits(out_q),
                     in_q.q, filt_q.q, bias_q.q, mul_biases_q.q, out_q.q,
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
