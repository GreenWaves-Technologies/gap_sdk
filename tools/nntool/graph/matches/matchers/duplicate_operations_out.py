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

from graph.types.base import ComparableParameters, NNEdge
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, description, groups, match_name, run_before

LOG = logging.getLogger("nntool." + __name__)


@match_name("duplicate_operations_out")
@description("""Removes operations that are duplicates on the same out edge""")
@run_before("*")
@groups('*')
class MatchDuplicateOperationsOut(Matcher):

    def explore(self, G, nodes, result=None):
        if result is None:
            result = []
            return self.explore(G, [edge.to_node for edge in G.out_edges(nodes[0].name)], result=result)

        out_edges_bundle = [G.indexed_out_edges(node.name) for node in nodes]
        if any(len(out_edges) != 1 or len(out_edges[0]) != 1 for out_edges in out_edges_bundle):
            return result
        # node == nodes[0] added since node could be a multi input expression
        if any(not isinstance(node, ComparableParameters) or not node.is_same_operation_as(G, nodes[0]) or node == nodes[0]
               for node in nodes[1::]):
            return result
        if not result:
            result = [[node] for node in nodes]
        else:
            for idx, res in enumerate(result):
                res.append(nodes[idx])
        return self.explore(G, [out_edges[0][0].to_node for out_edges in out_edges_bundle], result=result)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        modified_graph = False
        candidates = [node for node in G.nodes()
                      if len(G.indexed_out_edges(node.name)) == 1 and len(G.out_edges(node.name)) > 1]
        while candidates:
            node = candidates.pop(0)
            strings = self.explore(G, [node])
            if not strings:
                continue
            modified_graph = True
            primary = strings.pop(0)
            for pnode in primary:
                if pnode in candidates:
                    candidates.remove(pnode)
            out_edges = []
            for other in strings:
                out_edges.extend(G.out_edges(other[-1].name))
                for other_node in other:
                    if other_node in candidates:
                        candidates.remove(other_node)
                    G.remove(other_node)
                    nid = NodeId(other_node)
                    if G.quantization and nid in G.quantization:
                        del G.quantization[nid]
                LOG.info(
                    f'removed duplicates from {primary[0].name} {",".join(node.name for node in other)}')
            pend = primary[-1]
            for edge in out_edges:
                G.add_edge(
                    NNEdge(from_node=pend, to_node=edge.to_node, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return modified_graph
