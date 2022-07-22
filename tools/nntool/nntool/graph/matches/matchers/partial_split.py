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

from nntool.graph.types import NNEdge, SplitNode
from nntool.utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name, run_before)

LOG = logging.getLogger(__name__)


@groups('*')
@match_name("partial_split")
@run_before('insert_copies')
@description("add fake output to partial split")
class PartialSplitMatch(Matcher):
    def _match(self, G: GraphView, **kwargs) -> bool:
        has_modified_graph = False
        for node in G.nodes(node_classes=SplitNode):
            out_edges = G.indexed_out_edges(node)
            if not any(not edges for edges in out_edges):
                continue
            has_modified_graph = True
            for idx, edges in enumerate(out_edges):
                if edges:
                    continue
                fake_output = G.add_output(name=G.unique_name('unused'), not_used=True)
                G.add_edge(NNEdge(
                    from_node=node,
                    from_idx=idx,
                    to_node=fake_output))

        return has_modified_graph
