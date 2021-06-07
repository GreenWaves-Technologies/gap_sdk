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

import os

from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generator_decorators import QREC_POW2, generation_function
from generation.generators.globals.global_names import MULSCALE
from graph.types import FilterParameters, MultiplicativeBiasParameters


@generation_function("globals", (FilterParameters,), qrec_types=(QREC_POW2,))
def filter_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    del fnode
    if isinstance(node, MultiplicativeBiasParameters) and node.has_mul_bias:
        mul_biases_q = qrec.mul_biases_q

        cname = gen.naming_convension.get_global_name(pnode.name, pnode.step_idx,
                                                      pnode, MULSCALE)
        file_name = os.path.join(gen.opts['tensor_directory'],
                                 cname+".tensor")
        gen.name_cache.set(node, MULSCALE, cname)

        contents = mul_biases_q.quantize(node.mul_biases).astype(mul_biases_q.dtype,
                                                                 order='C',
                                                                 casting='no',
                                                                 copy=True)
        const_info = ConstantInfo(file_name, mul_biases_q, contents=contents)

        gen.globals.append(GlobalArgInfo(mul_biases_q.ctype, cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info))
    return True
