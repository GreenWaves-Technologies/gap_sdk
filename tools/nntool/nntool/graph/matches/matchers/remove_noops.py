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

from nntool.graph.types import NNEdge
from nntool.utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before, run_adjust_on_match

LOG = logging.getLogger(__name__)



@match_name("remove_noops")
@description("Remove noop nodes and other nodes that are doing nothing such as Concats "
             "or Splits with one input and output or transposes with no transpose or "
             "reshapes that don't reshape, etc.")
@run_before('*')
@groups('symmetric', 'scaled')
@run_adjust_on_match
class RemoveNoOPs(Matcher):

    def _match(self, G: GraphView, **kwargs) -> bool:
        has_modified_graph = False
        for node in [node for node in G.nodes() if node.does_nothing]:
            has_modified_graph = True
            in_edge = G.in_edges(node.name)[0]
            G.remove_edge(in_edge)
            for out_edge in G.out_edges(node.name):
                G.remove_edge(out_edge)
                G.add_edge(NNEdge(in_edge.from_node, out_edge.to_node,
                                  from_idx=in_edge.from_idx, to_idx=out_edge.to_idx))
            LOG.info(f'removing {node.name} that does nothing')
            G.remove(node)



        return has_modified_graph
