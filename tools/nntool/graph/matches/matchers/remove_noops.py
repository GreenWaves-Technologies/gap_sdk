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

from graph.types import NNEdge, NoOPParameters
from graph.types.others import (ConcatParameters, ReshapeParameters,
                                SplitParameters, TransposeParameters)
from utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before

LOG = logging.getLogger("nntool." + __name__)



@match_name("remove_noops")
@description("Remove noop nodes and other nodes that are doing nothing such as Concats "
             "or Splits with one input and output or transposes with no transpose or "
             "reshapes that don't reshape, etc.")
@run_before('*')
@groups('symmetric', 'scaled')
class RemoveNoOPs(Matcher):

    @staticmethod
    def one_inedge(G, node, idx=None):
        in_edges = G.in_edges(node)
        return len(in_edges) == 1 and (idx is None or in_edges[0].to_idx == idx)

    @staticmethod
    def one_outedge(G, node, idx=None):
        out_edges = G.out_edges(node)
        return len(out_edges) == 1 and (idx is None or out_edges[0].from_idx == idx)

    @staticmethod
    def one_in_and_outedge(G, node, idx=None):
        return RemoveNoOPs.one_inedge(G, node, idx=idx) and RemoveNoOPs.one_outedge(G, node, idx=idx)

    @staticmethod
    def node_does_nothing(G, node):
        return (isinstance(node, NoOPParameters) or
                isinstance(node, TransposeParameters) and node.transpose is None or
                isinstance(node, ReshapeParameters) and node.old_shape == node.shape or
                (isinstance(node, (ConcatParameters, SplitParameters)) and RemoveNoOPs.one_in_and_outedge(G, node, idx=0)))

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        for node in [node for node in G.nodes() if self.node_does_nothing(G, node)]:
            has_modified_graph = True
            in_edge = G.in_edges(node.name)[0]
            G.remove_edge(in_edge)
            for out_edge in G.out_edges(node.name):
                G.remove_edge(out_edge)
                G.add_edge(NNEdge(in_edge.from_node, out_edge.to_node,
                                  from_idx=in_edge.from_idx, to_idx=out_edge.to_idx))
            LOG.info(f'removing {node.name} that does nothing')
            G.remove(node)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
