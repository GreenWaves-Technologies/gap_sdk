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

from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
# pylint: disable=wildcard-import, unused-wildcard-import
from generation.generators.globals.global_names import *
from quantization.multiplicative.mulbias import gen_mul_biases

from .gen_constant import gen_constant


def gen_scales(gen, pnode, fnode, fqrec):
    cname_mul_scale, file_name_mul_scale = gen_constant(
        gen, pnode, fnode, MULSCALE)
    cname_mul_shift, file_name_mul_shift = gen_constant(
        gen, pnode, fnode, MULSHIFT)

    mul_biases_q = fqrec.cache['mul_biases_q']

    const_info_mul_scale = ConstantInfo(
        file_name_mul_scale, mul_biases_q, contents=gen_mul_biases(fqrec, fnode))
    const_info_mul_shift = ConstantInfo(
        file_name_mul_shift, mul_biases_q.shift_qtype, contents=mul_biases_q.qnorms)

    gen.globals.append(GlobalArgInfo(mul_biases_q.ctype, cname_mul_scale,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info_mul_scale))

    gen.globals.append(GlobalArgInfo(mul_biases_q.shift_ctype, cname_mul_shift,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info_mul_shift))
