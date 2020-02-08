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

from quantization.qtype import QType
from quantization.simple_auto_quantify import SimpleQuantizer
from utils.node_id import NodeId
from utils.stats_funcs import STATS_BITS


class TuneError(Exception):
    pass

def get_qtype(qparam1, qparam2):
    try:
        bits_idx = STATS_BITS.index(qparam1 + qparam2)
    except ValueError:
        raise TuneError("bit width is not valid")
    return QType(STATS_BITS[bits_idx], qparam2, True)

def tuneq(G, qrecs, step_num, param, qparam1, qparam2, index=0):
    step = G.graph_state.steps[step_num]
    node = step['node']
    if param == 'dp':
        raise ValueError("dp is deprecated. all layers are now double precision.")

    qrec = qrecs[NodeId(node, None)]
    if param == "out":
        qtype = get_qtype(qparam1, qparam2)
        SimpleQuantizer.propagate(G, qrecs, node, qtype)
    else:
        if not hasattr(qrec, param + '_q'):
            raise TuneError("parameter " + param + " not found")
        setattr(qrec, param + '_q', get_qtype(qparam1, qparam2))
