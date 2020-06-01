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

import numpy as np

from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from graph.types import (ConvFusionParameters, FilterParameters)

from utils.node_id import NodeId

from .mult8_infos_generator import gen_constant
from .global_names import *

@generation_function("globals", (FilterParameters, ConvFusionParameters), qrec_types=(QREC_MULT8,))
def mult8_filter_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    if fnode is not None:
        return False
    if isinstance(pnode, FilterParameters):
        gen_filter_globals(gen, pnode, pnode, qrec)
    elif isinstance(pnode, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if node.fusion_type in ("conv_active_pool", "conv_active", "linear_active", "conv_pool_active", "conv_pool"):
            gen_filter_globals(gen, pnode, cnodes[0], quants[0])
        else:
            return False
    else:
        return False
    return True


def gen_filter_globals(gen, pnode, fnode, fqrec):
    cname, file_name = gen_constant(gen, pnode, fnode, WEIGHTS)
    weights_q = fqrec.weights_q
    const_info = ConstantInfo(file_name, weights_q, contents=fqrec.gen_weights(fnode, fnode.weights))

    gen.globals.append(GlobalArgInfo(weights_q.ctype, cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info))

    # biases are always generated even if they are 0
    if fnode.has_bias:
        biases_q = fqrec.biases_q
        biases = fnode.biases
    else:
        biases_q = fqrec.out_qs[0]
        biases = np.zeros((fnode.out_dims[0].c))

    contents = fqrec.gen_biases(fnode, biases, fnode.weights)

    cname, file_name = gen_constant(gen, pnode, fnode, BIASES)
    const_info = ConstantInfo(file_name, biases_q, contents=contents)

    gen.globals.append(GlobalArgInfo(biases_q.ctype, cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info))

    cname_mul_scale, file_name_mul_scale = gen_constant(gen, pnode, fnode, MULSCALE)
    cname_mul_shift, file_name_mul_shift = gen_constant(gen, pnode, fnode, MULSHIFT)

    mul_biases_q = fqrec.mul_biases_q

    const_info_mul_scale = ConstantInfo(
        file_name_mul_scale, mul_biases_q, contents=fqrec.gen_mul_biases(fnode))
    const_info_mul_shift = ConstantInfo(
        file_name_mul_shift, mul_biases_q.shift_qtype, contents=fqrec.mul_biases_q.qnorms)

    gen.globals.append(GlobalArgInfo(mul_biases_q.ctype, cname_mul_scale,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info_mul_scale))

    gen.globals.append(GlobalArgInfo(mul_biases_q.shift_ctype, cname_mul_shift,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info_mul_shift))
