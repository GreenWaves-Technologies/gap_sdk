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

from quantization.qtype import QType
from quantization.symmetric.symmetric_quantizer import SymmetricQuantizer
from utils.node_id import NodeId
from utils.stats_funcs import STATS_BITS
from graph.types import ConvFusionParameters

class TuneError(Exception):
    pass

def get_qtype(qparam1, qparam2):
    try:
        bits_idx = STATS_BITS.index(qparam1 + qparam2)
    except ValueError:
        raise TuneError("bit width is not valid")
    return QType(STATS_BITS[bits_idx], qparam2, True)

def tuneq(G, qrecs, step_num, param, qparam1, qparam2, index=0):
    del index
    step = G.graph_state.steps[step_num]
    node = step['node']
    if param == 'dp':
        raise ValueError("dp is deprecated. all layers are now double precision.")

    if param == "out":
        qtype = get_qtype(qparam1, qparam2)
        SymmetricQuantizer.propagate(G, qrecs, node, qtype)
    else:
        if isinstance(node, ConvFusionParameters):
            for subnode in node.contained_nodes():
                qrec = qrecs[NodeId(node, subnode)]
                if hasattr(qrec, param + '_q'):
                    setattr(qrec, param + '_q', get_qtype(qparam1, qparam2))
                    return
            raise TuneError("parameter " + param + " not found")

        qrec = qrecs[NodeId(node, None)]
        if not hasattr(qrec, param + '_q'):
            raise TuneError("parameter " + param + " not found")
        setattr(qrec, param + '_q', get_qtype(qparam1, qparam2))
