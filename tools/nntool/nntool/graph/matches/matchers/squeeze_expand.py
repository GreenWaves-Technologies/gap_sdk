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

from nntool.graph.types.linear import LinearNode
import logging

from nntool.graph.dim import Dim
from nntool.graph.matches.find_sequence import (AnyOf, NodeMatcher, OneOrMoreOf,
                                         Sequence)
from nntool.graph.types import (ActivationNodeBase, ConstantInputNode,
                         MatrixBroadcastedLinearOpNodeBase, NNEdge,
                         ReshapeNode)
from nntool.utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before, run_after

LOG = logging.getLogger(__name__)


class CustomMatcher(NodeMatcher):
    def _matches(self, G, elem):
        if (isinstance(elem, MatrixBroadcastedLinearOpNodeBase) and
            any(isinstance(edge.from_node, ConstantInputNode)
                for edge in G.in_edges(elem.name))):
            return [elem], True, True
        return None, None, None


def propagate_shape(G, shape, in_pattern, nodes):
    for node in nodes:
        if isinstance(node, MatrixBroadcastedLinearOpNodeBase):
            max_len = max(len(in_dim.shape) for in_dim in node.in_dims)
            for const_node in [edge.from_node for edge in G.in_edges(node.name)
                               if isinstance(edge.from_node, ConstantInputNode)]:
                shape = ([1] * (max_len - len(const_node.dims.shape))
                         ) + const_node.dims.shape
                new_shape = []
                for pat in in_pattern:
                    if pat is None:
                        new_shape.append(shape.pop(0))
                    elif pat:
                        assert shape.pop(0) == 1
                    else:
                        new_shape.append(1)
                new_dim = Dim.unnamed(new_shape)
                LOG.info('reshaping const node %s %s -> %s',
                         const_node.name, const_node.dims, new_dim)
                const_node.dims = new_dim
                const_node.value = const_node.value.reshape(new_shape)


@match_name('squeeze_expand')
@description('Remove reshapes that remove axes and then add them back with Activations and piecewise operations inbetween')
@groups('*')
@run_before('fuse_external_bias', 'remove_reshapes_before_linear')
@run_after('remove_reshapes')
class SqueezeExpandMatcher(Matcher):

    def _match(self, G: GraphView, **kwargs):
        has_modified_graph = False

        seq = Sequence(ReshapeNode, OneOrMoreOf(
            AnyOf(ActivationNodeBase, CustomMatcher())), AnyOf(ReshapeNode, LinearNode))
        for nodes in seq.find(G):
            in_pattern = nodes[0].exp_red_pattern()
            if not in_pattern:
                continue
            has_modified_graph = True
            remove_both = isinstance(nodes[-1], ReshapeNode) and nodes[0].old_shape.shape == nodes[-1].shape.shape
            propagate_shape(G, nodes[0].shape, in_pattern, nodes[1:-1:])
            LOG.info('removing unnecessary reshape %s', nodes[0].name)
            G.remove_and_reconnect(nodes[0], edge_class=NNEdge)
            self.remove_quantization(G, nodes[0])
            if remove_both:
                LOG.info('removing unnecessary reshape %s', nodes[-1].name)
                G.remove_and_reconnect(nodes[-1], edge_class=NNEdge)
                self.remove_quantization(G, nodes[-1])
            elif isinstance(nodes[-1], ReshapeNode):
                nodes[-1].old_shape = nodes[0].shape

        return has_modified_graph
