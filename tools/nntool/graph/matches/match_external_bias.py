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

from graph.types import  FilterParameters, MatrixAddParameters, ConstantInputParameters
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher, DontReplaceError

LOG = logging.getLogger("nntool." + __name__)


class MatchExternalBias(DefaultMatcher):
    NAME = 'fuse_external_bias'
    DESCRIPTION = 'Fuse bias addition after filter with filter bias'

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0', matcher=lambda node:\
                isinstance(node, FilterParameters)))
        sub.add_node(MatchNode('1', matcher=lambda node:\
                isinstance(node, MatrixAddParameters)))
        sub.add_node(MatchNode('2', matcher=lambda node:\
                isinstance(node, ConstantInputParameters)))
        sub.add_edge(Edge('0', '1', to_idx=0))
        sub.add_edge(Edge('2', '1', to_idx=1))

        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        filter_node = None
        constant_node = None
        for node in subgraph.nodes():
            if isinstance(node, FilterParameters):
                filter_node = node
            elif isinstance(node, ConstantInputParameters):
                constant_node = node
        flattened_constant = constant_node.value.flatten()
        # shape needs to match
        if flattened_constant.shape[0] == filter_node.filter.out_c:
            if filter_node.has_bias:
                assert filter_node.biases is not None, "can't absorb bias into filter. maybe weights are not loaded"
                filter_node.biases += flattened_constant
            else:
                filter_node.biases = flattened_constant
        else:
            raise DontReplaceError()
        return filter_node
