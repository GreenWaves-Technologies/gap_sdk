# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
