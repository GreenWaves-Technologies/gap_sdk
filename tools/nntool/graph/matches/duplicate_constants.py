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

from graph.dim import Dim
from graph.matches.matcher import Matcher
from graph.types import ConstantInputParameters
from graph.types.base import NNEdge
from utils.graph import GraphView

LOG = logging.getLogger("nntool." + __name__)


class MatchDuplicateConstants(Matcher):
    NAME = "match_duplicate_constants"
    DESCRIPTION = """Find constants that are linked to more than one node and duplicate them"""

    def match(self, G: GraphView, set_identity: bool = True):
        has_modified = False
        for node in G.nodes(node_classes=ConstantInputParameters):
            out_edges = G.out_edges(node.name)
            if len(out_edges) <= 1:
                continue
            has_modified = True
            LOG.info('node %s has more than one out edge and will be duplicated', node.name)
            idx = 1
            for out_edge in out_edges[1::]:
                new_constant = ConstantInputParameters(f'{node.name}_{idx}', dims=Dim.unnamed(node.dims.shape), value=node.value.copy())
                G.remove_edge(out_edge)
                G.add_edge(NNEdge(from_node=new_constant, to_node=out_edge.to_node, to_idx=out_edge.to_idx))
                idx += 1

        if set_identity:
            self.set_identity(G)

        return has_modified
