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

from graph.types import FcParameters, ActivationParameters, ConvFusionParameters
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher

LOG = logging.getLogger("nntool." + __name__)


class MatchGapLinear(DefaultMatcher):
    NAME = 'fuse_gap_linear'
    DESCRIPTION = 'Fuse linear layers and activations to match GAP AutoTiler operations'

    def valid_linear(self, node):
        del node
        # TODO - Add specific pool parameter checking here
        return True

    def valid_activation(self, node):
        del node
        # TODO - Add specific pool parameter checking here
        return True

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0',
                               matcher=lambda node:
                               isinstance(node, FcParameters) and
                               self.valid_linear(node)))
        sub.add_node(MatchNode('1', matcher=lambda node:
                               isinstance(node, ActivationParameters) and
                               self.valid_activation(node)))
        sub.add_edge(Edge('0', '1'))
        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        step = 0
        for node in subgraph.nodes():
            node.step_idx = step
            step = step + 1
            if isinstance(node, FcParameters):
                linear_name = node.name + "_fusion"
                break
        LOG.debug("fused nodes %s", ",".join(
            (node.name for node in subgraph.nodes())))
        # simple node order is necessary because nodes() will not necessarily
        # be in order
        return ConvFusionParameters(linear_name, "linear_active", subgraph)
