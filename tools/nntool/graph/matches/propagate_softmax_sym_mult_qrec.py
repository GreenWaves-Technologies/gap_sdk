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
from graph.matches.matcher import Matcher
from graph.types import SoftMaxParameters, OutputParameters
from quantization.multiplicative.mult_quantization import MultQuantizationRecord
from utils.graph import GraphView
from utils.node_id import NodeId
from .equalize_sym_mult_concats import propagate_qtype_up

class PropagateSoftmaxSymQrec(Matcher):
    NAME = "propagate_softmax_sym_qrec"
    DESCRIPTION = """Set input qrec of softmaxes to pow2 and propagate up"""

    def match(self, G: GraphView, set_identity: bool = True):
        if not G.quantization:
            return
        softmaxes = [node for node in G.nodes() if isinstance(node, SoftMaxParameters)]
        qrecs = [G.quantization[NodeId(node)] for node in softmaxes]
        if not all(isinstance(qrec, MultQuantizationRecord) for qrec in qrecs):
            return
        for softmax, qrec in zip(softmaxes, qrecs):
            in_q = qrec.in_qs[0]
            in_q.scale_to_pow2()
            for edge in G.in_edges(softmax.name):
                propagate_qtype_up(G, in_q, edge)
            for edge in G.out_edges(softmax.name):
                assert isinstance(edge.to_node, OutputParameters), "Softmax is supported only at the end of the graph"
                out_qrec = G.quantization[NodeId(edge.to_node)]
                out_qrec.in_qs[0] = qrec.out_qs[0]
                out_qrec.out_qs[0] = qrec.out_qs[0]

        if set_identity:
            self.set_identity(G)

        return False
