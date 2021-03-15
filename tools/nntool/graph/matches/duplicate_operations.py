# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
import logging
from itertools import groupby
from functools import partial

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
from graph.matches.matcher import Matcher
from graph.types.base import ComparableParameters, NNEdge
from utils.graph import GraphView
LOG = logging.getLogger("nntool." + __name__)


class MatchDuplicateOperations(Matcher):
    NAME = "match_duplicate_operations"
    DESCRIPTION = """Removes operations that are duplicates on the same edge"""

    def match(self, G: GraphView, set_identity: bool = True):
        def same_source_edge(x):
            return f"{x.from_node.__hash__()}##{x.from_idx}"

        def same_dest_edge(x):
            return f"{x.to_node.__hash__()}##{x.to_idx}"
        
        modified_graph = False
        same_source_edges = [list(edge_list) for _, edge_list in
                             groupby(sorted(G.edges(), key=same_source_edge), same_source_edge)]
        # all have the same origin
        same_source_edges = [elem for elem in same_source_edges
                             if len(elem) > 1]
        same_dest_edges = []

        for same_source_edge in same_source_edges:
            same_source_edge = [edge for edge in same_source_edge if isinstance(edge.to_node, ComparableParameters)]
            while same_source_edge:
                first = same_source_edge.pop(0)
                others = list(filter(partial(lambda x, y: y.to_node.is_same_operation_as(x.to_node), first), same_source_edge))
                if others:
                    same_dest_edges.append(tuple([first] + others))
                    for other in others:
                        same_source_edge.remove(other)

        # all are multiple edges that go to something comparable

        for edge_set in same_dest_edges:
            first = edge_set[0]
            first_node = first.to_node
            dup_nodes = []
            for other in edge_set[1: : ]:
                modified_graph = True
                dest_node = other.to_node
                dup_nodes.append(dest_node.name)
                out_edges = G.out_edges(dest_node.name)
                G.remove(dest_node)
                for out_edge in out_edges:
                    G.add_edge(NNEdge(from_node=first_node, to_node=out_edge.to_node,
                                      from_idx=out_edge.from_idx, to_idx=out_edge.to_idx))
            LOG.info(f'removed duplicates {",".join(dup_nodes)} to {first_node.name}')

        if set_identity:
            self.set_identity(G)

        return modified_graph
