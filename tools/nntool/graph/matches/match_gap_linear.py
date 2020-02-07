# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging

from graph.types import FcParameters, ActivationParameters, FusionParameters
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
        return FusionParameters(linear_name, "linear_active", subgraph)
