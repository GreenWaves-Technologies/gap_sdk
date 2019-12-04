# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

from graph.types import (FilterLikeParameters, PadParameters)
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher

LOG = logging.getLogger("nntool." + __name__)

class MatchFusePad(DefaultMatcher):
    NAME='fuse_pad'
    DESCRIPTION='Fuse pad operation to subsequent Convolution or Pool'
    def has_no_padding(self, node):
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
