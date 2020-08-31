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
import logging
from copy import deepcopy

from graph.matches.matcher import Matcher
from graph.types import ConcatParameters, ReshapeParameters, TransposeParameters
from quantization.multiplicative.mult_quantization import MultQuantizationRecord, MultScalableFilterQuantizationRecord
from quantization.multiplicative.symmetric.symmetric_mult_qtype import SymmetricMultQType
from utils.graph import Edge, GraphView
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

CAN_PASS = (
    ReshapeParameters,
    TransposeParameters
)

def set_in_scale(qrec, index, scale):
    in_q = qrec.in_qs[index]
    assert isinstance(in_q, SymmetricMultQType), "not supported on other quantization types"
    in_q.scale = scale

def set_out_scale(qrec, index, scale):
    out_q = qrec.out_qs[index]
    assert isinstance(out_q, SymmetricMultQType), "not supported on other quantization types"
    if isinstance(qrec, MultScalableFilterQuantizationRecord):
        assert index == 0, "trying to set strange index on filter quantization record"
        out_q.scale = scale
        qrec.mul_biases_q.scale = qrec.in_qs[0].scale * qrec.weights_q.scale / out_q.scale
    else:
        out_q.scale = scale

def propagate_qtype_up(G, qtype, edge: Edge):
    LOG.info("propagating scale up from node %s to node %s", edge.to_node.name, edge.from_node.name)
    qrec_out = G.quantization[NodeId(edge.from_node)]
    set_out_scale(qrec_out, edge.from_idx, qtype.scale)
    qrec_in = G.quantization[NodeId(edge.to_node)]
    set_in_scale(qrec_in, edge.to_idx, qtype.scale)
    if isinstance(edge.from_node, CAN_PASS):
        for edge in G.in_edges(edge.from_node.name):
            propagate_qtype_up(G, qtype, edge)

class EqualizeSymmetricMultiplicativeQuantivedConcats(Matcher):
    NAME = "equalize_sm_concats"
    DESCRIPTION = """Equalize input quantization of concats with symmetric multiplicative quantization"""

    def match(self, G: GraphView, set_identity: bool = True):
        if not G.quantization:
            return
        concats = [node for node in G.nodes() if isinstance(node, ConcatParameters)]
        qrecs = [G.quantization[NodeId(node)] for node in concats]
        if not all(isinstance(qrec, MultQuantizationRecord) for qrec in qrecs):
            return
        for concat, qrec in zip(concats, qrecs):
            out_q = qrec.out_qs[0]
            for edge in G.in_edges(concat.name):
                in_q = qrec.in_qs[edge.to_idx]
                if in_q != out_q:
                    propagate_qtype_up(G, out_q, edge)

        if set_identity:
            self.set_identity(G)

        return False
