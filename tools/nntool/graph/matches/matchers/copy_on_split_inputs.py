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

from graph.matches.matchers.insert_copies import find_real_in_edge
import logging

from graph.types import InputParameters, ReshapeParameters, ConstantInputParameters
from graph.types.others import ConcatParameters, CopyParameters, SplitParameters
from utils.graph import GraphView

from ..matcher import Matcher, groups, match_name, description, modifies_dimensions, run_after

LOG = logging.getLogger("nntool." + __name__)


def search_up_for_input(G, node, going_up=None):
    if going_up is None or isinstance(node, ReshapeParameters):
        return search_up_for_input(G, G.in_edges(node.name)[0].from_node, going_up=True)
    if isinstance(node, (InputParameters, ConstantInputParameters)):
        return node
    return None

@match_name("copy_on_split_inputs")
@description("Insert copy on inputs that could be in a tensor stack")
@modifies_dimensions(True)
@groups('*')
class CopyOnSplitInputs(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):

        candidates = [node for node in G.nodes(node_classes=(SplitParameters, ConcatParameters))]
        need_a_copy_edges = []
        for node in candidates:
            for idx, edge in enumerate(G.indexed_in_edges(node.name)):
                real_from_node, _ = find_real_in_edge(G, edge)
                if isinstance(real_from_node, (InputParameters, ConstantInputParameters)):
                    need_a_copy_edges.append((edge, idx))
        has_modified_graph = False
        for edge in need_a_copy_edges:
            LOG.info(
                "Insert copy on split input %s", edge[0].to_node.name)
            has_modified_graph = True
            cnode = CopyParameters(G.unique_name(f'{edge[0].to_node.name}_copy'))
            G.insert_node_at_edge(cnode, edge[0])
            if G.quantization:
                G.quantization.copy_qrec(edge[0].to_node, 'in', 0, cnode)
        if set_identity:
            self.set_identity(G)
        return has_modified_graph
