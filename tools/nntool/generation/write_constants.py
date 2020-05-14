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

from utils.node_id import NodeId

from graph.types import FilterParameters, ConstantInputParameters, MultiplicativeBiasParameters


def write_constants(G, naming_convension, tensor_directory=None):
    if tensor_directory is None:
        tensor_directory = "."
    else:
        os.makedirs(tensor_directory, mode=0o750, exist_ok=True)
    for step_idx, pnode, _, fnode in G.nodes_iterator():
        anode = pnode if not fnode else fnode
        if isinstance(anode, FilterParameters):
            cname = naming_convension.get_global_name(pnode.name, step_idx, pnode, "weights")
            qrec = G.quantization[NodeId(pnode, fnode)]
            weights_q = qrec.weights_q
            with open(os.path.join(tensor_directory, cname + ".tensor"), 'wb') as t_fp:
                weights_q.quantize(anode.weights)\
                    .astype(weights_q.dtype, order='C', casting='no', copy=True)\
                    .tofile(t_fp)

            # biases are always generated even if they are 0
            if anode.has_bias:
                biases_q = qrec.biases_q
                biases = biases_q.quantize(anode.biases)\
                    .astype(biases_q.dtype, order='C', casting='no', copy=True)
            else:
                biases = np.zeros(weights_q.dtype, dtype=np.float32, order='C')

            cname = naming_convension.get_global_name(pnode.name, step_idx, pnode, "biases")
            with open(os.path.join(tensor_directory, cname + ".tensor"), 'wb') as t_fp:
                biases.tofile(t_fp)

            if isinstance(anode, MultiplicativeBiasParameters) and anode.has_mul_bias:
                mul_biases_q = qrec.mul_biases_q
                mul_biases = mul_biases_q.quantize(anode.mul_biases)\
                    .astype(mul_biases_q.dtype, order='C', casting='no', copy=True)

                cname = naming_convension.get_global_name(pnode.name, step_idx, pnode, "mul_biases")
                with open(os.path.join(tensor_directory, cname + ".tensor"), 'wb') as t_fp:
                    mul_biases.tofile(t_fp)
        elif isinstance(anode, ConstantInputParameters):
            out_edge = G.out_edges(anode.name)[0]
            eparams = out_edge.params
            cname = naming_convension.get_edge_name(eparams.creating_node.name,
                                                    eparams.creating_step,
                                                    eparams.edge_type,
                                                    eparams.edge_order)
            qrec = G.quantization[NodeId(pnode, fnode)]
            constant_q = qrec.out_qs[0]
            with open(os.path.join(tensor_directory, cname + ".tensor"), 'wb') as t_fp:
                weights_q.quantize(anode.value)\
                    .astype(constant_q.dtype, order='C', casting='no', copy=True)\
                    .tofile(t_fp)
