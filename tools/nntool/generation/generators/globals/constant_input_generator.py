# Copyright (C) 2020, 2021  GreenWaves Technologies, SAS

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
from generation.at_types.tc_arg_info import (GlobalArgInfo, GlobalResetArgInfo,
                                             InputArgInfo)
from generation.generator_decorators import (QREC_FLOAT, QREC_MULT8, QREC_POW2,
                                             generation_function)
from graph.types import ConstantInputParameters
from graph.types.fusions import ConvFusionParameters, LinearFusionParameters
from graph.types.linear import FcParameters
from utils.node_id import NodeId
from utils.numpy_helpers import interleave, packbits


@generation_function("globals", (ConstantInputParameters,), qrec_types=(QREC_POW2, QREC_MULT8, QREC_FLOAT))
def constant_input_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    del node, fnode
    qtype = qrec.out_qs[0]
    if qtype.attr.dont_generate_value:
        return True
    # the name cache will be updated when all the edges are analysed by local_generator
    # the name of the constant is attached to the output edge so find it
    out_edge = gen.G.out_edges(pnode.name)[0]
    eparams = out_edge.params
    cname = gen.naming_convension.get_edge_name(eparams.creating_node,
                                                eparams.creating_step,
                                                eparams.edge_type,
                                                eparams.edge_order)
    if not pnode.is_constant:
        # This is an initializer which may have a reset
        if pnode.reset_name and not next((tc for tc in gen.globals
                                          if tc.arg_name == pnode.reset_name), None):
            gen.globals.append(GlobalResetArgInfo(
                pnode.reset_name, 'AT_MEM_L2', 'AT_MEM_UNDEF'))
        if pnode.is_global:
            home_location = gen.opts['default_input_home_location']
            exec_location = gen.opts['default_input_exec_location']
            gen.globals.append(InputArgInfo(qtype.ctype, cname,
                                            home_location=home_location,
                                            exec_location=exec_location,
                                            allocate=pnode.at_options.allocate,
                                            is_inout=pnode.is_mutated))
    elif pnode.is_global:
        file_name = os.path.join(gen.opts['tensor_directory'],
                                 cname+".tensor")
        value = pnode.value_as(qtype, generation=True)

        if qtype.attr.concatenated_nodes:
            values = [value]
            concatenated_nodes = [gen.G[node_name] for node_name in qtype.attr.concatenated_nodes]
            concated_qrecs = [gen.G.quantization.get(NodeId(pn, None))
                              for pn in concatenated_nodes]
            for other_node, concated_qrec in zip(concatenated_nodes, concated_qrecs):
                values += [other_node.value_as(
                    concated_qrec.out_qs[0], generation=True)]
            value = np.hstack(tuple(values))
        elif qtype.attr.interleaved_values:
            value = interleave(value, *qtype.attr.interleaved_values)

        if qtype.attr.resize:
            padding = tuple((0, new-orig) for orig, new in zip(*qtype.attr.resize))
            value = np.pad(value, padding)

        if qtype.attr.bit_pack and qtype.attr.bit_pack != 8:
            # pack value into qtype.attr.bit_pack bit items. Requires uint8 input
            assert value.dtype == np.uint8, "bit pack only works on uint8 datatypes"
            value = packbits(value, qtype.attr.bit_pack)

        if qtype.attr.ne16_biases:
            to_node = gen.G.out_edges(pnode.name)[0].to_node
            if isinstance(to_node, (ConvFusionParameters, LinearFusionParameters)):
                cnodes = to_node.contained_nodes()
                quants = [gen.G.quantization[NodeId(
                    to_node, fnode)] for fnode in cnodes]
                filter_qrec = quants[0]
            else:
                filter_qrec = gen.G.quantization[NodeId(to_node)]
            mul_qbiases = filter_qrec.cache['mul_biases_q'].qbiases
            mul_qnorms = filter_qrec.cache['mul_biases_q'].qnorms
            value = np.where(mul_qnorms > 0,
                                value * mul_qbiases + (1 << (mul_qnorms-1).astype(np.int32)),
                                value * mul_qbiases)

        const_info = ConstantInfo(file_name, qtype, contents=value,
                                  numeric_format="fixed")
        gen.globals.append(GlobalArgInfo(qtype.ctype, cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info))
    return True
