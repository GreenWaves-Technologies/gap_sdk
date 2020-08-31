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
from quantization.multiplicative.mult_quantization import MultQuantizationRecord
from utils.graph import Edge, GraphView
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

def reduce_qtypes(qtypes):
    max_idx = max(idx for idx, _ in qtypes)
    res = [None] * (max_idx + 1)
    for idx, qtype in qtypes:
        if res[idx] is None:
            res[idx] = qtype
        elif qtype != res[idx]:
            raise ValueError("qtypes are not compatible on index %s" % idx)
    return res


class FindMissingQuantization(Matcher):
    NAME = "find_missing_quantization"
    DESCRIPTION = """Propagate quantization to nodes that have none"""


    def match(self, G: GraphView, set_identity: bool = True):
        if not G.quantization:
            return
        for nid in [nid for nid, qrec in G.quantization.sorted_iterator(G) if qrec is None or not (qrec.in_qs and qrec.out_qs)]:
            if nid.fnode_name:
                LOG.warning("can't add quantization to fused node %s", nid.fnode_name)
                continue
            if nid.node_name not in G:
                # previous fusions may have removed nodes from the graph
                continue

            node = nid.get_node(G)
            predecessors = [NodeId(pred) for pred in G.predecessors(node.name)]
            successors = [NodeId(succ) for succs in G.successors(node.name) for succ in succs]
            go_back = not successors or (predecessors and all(pred in G.quantization for pred in predecessors))
            go_forward = not predecessors or (successors and all(succ in G.quantization for succ in successors))

            if not (go_back or go_forward):
                LOG.warning("node %s is not connected to anything and has no quantization", node.name)
                continue

            if go_forward:
                out_qrecs = set(G.quantization[nid] for nid in successors)
                if not all(isinstance(out_qrec, MultQuantizationRecord) for out_qrec in out_qrecs):
                    continue
                out_qtypes = reduce_qtypes([(edge.from_idx, G.quantization[NodeId(edge.to_node)].in_qs[edge.to_idx])
                                            for edge in G.out_edges(node.name)])
            else:
                out_qtypes = None
            if go_back:
                in_qrecs = set(G.quantization[nid] for nid in predecessors)
                if not all(isinstance(in_qrec, MultQuantizationRecord) for in_qrec in in_qrecs):
                    continue
                in_qtypes = reduce_qtypes([(edge.to_idx, G.quantization[NodeId(edge.from_node)].out_qs[edge.from_idx])
                                           for edge in G.in_edges(node.name)])
            else:
                in_qtypes = None

            if not in_qtypes:
                if not predecessors:
                    LOG.info("setting quantization on input node %s", node.name)
                    qrec = MultQuantizationRecord(in_qs=deepcopy(out_qtypes), out_qs=deepcopy(out_qtypes))
                else:
                    raise NotImplementedError("propagating qrecs not implemented")
            elif not out_qtypes:
                if not successors:
                    LOG.info("setting quantization on output node %s", node.name)
                    qrec = MultQuantizationRecord(in_qs=deepcopy(in_qtypes), out_qs=deepcopy(in_qtypes))
                else:
                    raise NotImplementedError("propagating qrecs not implemented")
            else:
                LOG.info("setting quantization on node %s", node.name)
                qrec = MultQuantizationRecord(in_qs=deepcopy(in_qtypes), out_qs=deepcopy(out_qtypes))

            G.quantization[nid] = qrec

        if set_identity:
            self.set_identity(G)

        return False
