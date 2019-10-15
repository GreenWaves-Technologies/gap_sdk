# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import os
import numpy as np

from utils.node_id import NodeId

from graph.types import FilterParameters

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
