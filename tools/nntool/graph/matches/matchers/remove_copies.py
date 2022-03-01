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

from graph.types import (ConcatParameters, ConstantInputParameters,
                         CopyParameters, InputParameters, NNEdge,
                         NoOPParameters, OutputParameters, ReshapeParameters,
                         SplitParameters, TransposeParameters)
from utils.graph import GraphView
from utils.node_id import NodeId

from ..match_utils import search_down, search_up
from ..matcher import (Matcher, description, groups, match_name,
                       modifies_dimensions, run_after)

LOG = logging.getLogger("nntool." + __name__)


@match_name("remove_copies")
@description("Remove unnecessary copies")
@modifies_dimensions(True)
@groups('*')
@run_after('remove_noops')
class RemoveCopies(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        nodes_to_remove = []
        for node in G.nodes(node_classes=CopyParameters):
            out_edges = G.out_edges(node)
            if len(out_edges) > 1:
                continue
            if (search_down(
                    G,
                    out_edges[0],
                    (OutputParameters, InputParameters, ConstantInputParameters, SplitParameters, ConcatParameters),
                    can_pass_fn=lambda G, node: node.no_model_code,
                    follow_multi=True) and
                    search_up(
                        G,
                        G.in_edges(node)[0],
                        (InputParameters, OutputParameters, ConstantInputParameters, SplitParameters, ConcatParameters),
                        can_pass_fn=lambda G, node: node.no_model_code,
                        follow_multi=True)):
                continue
            nodes_to_remove.append(node)
        for node in nodes_to_remove:
            LOG.info(
                "remove redundant copy %s",
                node.name)
            has_modified_graph = True
            G.remove_and_reconnect(node, edge_class=NNEdge)
            if G.quantization:
                nid = NodeId(node)
                if nid in G.quantization:
                    del G.quantization[nid]

        if set_identity:
            self.set_identity(G)
        return has_modified_graph
