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

from graph.types import (FilterLikeParameters, PadParameters)
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher

LOG = logging.getLogger("nntool." + __name__)

class MatchFusePad(DefaultMatcher):
    NAME = 'fuse_pad'
    DESCRIPTION = 'Fuse pad operation to subsequent Convolution or Pool'

    @staticmethod
    def has_no_padding(node):
        return node.padding.size() == 0

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0', matcher=lambda node:\
                isinstance(node, PadParameters)))
        sub.add_node(MatchNode('1', matcher=lambda node:\
                isinstance(node, FilterLikeParameters) and\
                self.has_no_padding(node)))
        sub.add_edge(Edge('0', '1'))
        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        for node in subgraph.nodes():
            if isinstance(node, FilterLikeParameters):
                filter_like_node = node
            elif isinstance(node, PadParameters):
                pad_node = node

        LOG.debug("adding padding from: %s to filter: %s", pad_node.name, filter_like_node.name)

        filter_like_node.padding = pad_node.padding
        filter_like_node.pad_type = "zero"

        return filter_like_node
