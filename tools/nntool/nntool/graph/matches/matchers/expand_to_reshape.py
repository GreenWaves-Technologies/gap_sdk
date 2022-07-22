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

from nntool.graph.manipulations.eliminate_transposes.transpose_helpers import strip_ones
import logging

from nntool.graph.dim import Dim
from nntool.graph.types import NNEdge, ReshapeNode, TransposeNode, ExpandNode
from nntool.utils.graph import GraphView

from ..matcher import Matcher, match_name, description, run_before, groups, needs_valid_dimension

LOG = logging.getLogger(__name__)

@match_name("expand_to_reshape")
@description("remove expands that are really just reshapes")
@run_before('*')
@groups('*')
@needs_valid_dimension(True)
class ExpandToReshape(Matcher):

    def _match(self, G: GraphView, **kwargs):
        modified_graph = False
        for node in G.nodes(node_classes=ExpandNode):
            in_shape = node.in_dims[0].shape
            out_shape = node.out_dims[0].shape
            if strip_ones(in_shape) != strip_ones(out_shape):
                continue
            LOG.info(f'replacing expand {node.name} with a reshape')
            reshape = ReshapeNode(G.unique_name(f'{node.name}_reshape'), old_shape=in_shape, shape=out_shape)
            G.replace_node(node, reshape)
            modified_graph = True



        return modified_graph
