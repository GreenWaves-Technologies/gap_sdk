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

from graph.types import InputParameters, ReshapeParameters
from graph.types.others import CopyParameters, SplitParameters
from utils.graph import GraphView

from ..matcher import Matcher, groups, match_name, description, modifies_dimensions

LOG = logging.getLogger("nntool." + __name__)


def search_up_for_input(G, node, going_up=None):
    if going_up is None or isinstance(node, ReshapeParameters):
        return search_up_for_input(G, G.in_edges(node.name)[0].from_node, going_up=True)
    if isinstance(node, InputParameters):
        return node
    return None

@match_name("copy_on_split_inputs")
@description("Insert copy on split inputs")
@modifies_dimensions(True)
@groups('*')
class CopyOnSplitInputs(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):

        candidates = [node for node in G.nodes(
            node_classes=SplitParameters) if search_up_for_input(G, node)]
        has_modified_graph = False
        for node in candidates:
            LOG.info(
                "Insert copy on split input %s", node.name)
            has_modified_graph = True
            cnode = CopyParameters(G.unique_name(f'{node.name}_copy'))
            G.insert_node_at_edge(cnode, G.in_edges(node.name)[0])
            if G.quantization:
                G.quantization.copy_qrec(node, 'in', 0, cnode)
        if set_identity:
            self.set_identity(G)
        return has_modified_graph
