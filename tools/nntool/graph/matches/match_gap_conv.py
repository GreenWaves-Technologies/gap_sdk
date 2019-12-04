# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

from graph.types import ConcatParameters, GroupParameters, Conv2DParameters, FusionParameters, PoolingParameters, ActivationParameters
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher, MatchGroup

LOG = logging.getLogger("nntool." + __name__)


class MatchGapConv(DefaultMatcher):

    def __init__(self, match_activation=True, match_pool=False, pool_after_activation=False):
        assert match_activation or match_pool, "not very interesting to just match conv"
        self.match_activation = match_activation
        self.match_pool = match_pool
        self.pool_after_activation = pool_after_activation
        if match_activation and match_pool:
            if pool_after_activation:
                self.fusion_type = "conv_active_pool"
            else:
                self.fusion_type = "conv_pool_active"
        elif match_activation:
            self.fusion_type = "conv_active"
        else:
            self.fusion_type = "conv_pool"

    def valid_convolution(self, node):
        # TODO - Add specific convolution parameter checking here
        return True

    def valid_pooling(self, node):
        # TODO - Add specific pool parameter checking here
        return True

    def valid_activation(self, node):
        # TODO - Add specific pool parameter checking here
        return True

    def add_pooling(self, i, sub):
        sub.add_node(MatchNode(str(i), 
                     matcher=lambda node:\
                         isinstance(node, PoolingParameters) and\
                         self.valid_pooling(node)))

    def add_activation(self, i, sub):
        sub.add_node(MatchNode(str(i), 
                     matcher=lambda node:\
                         isinstance(node, ActivationParameters) and\
                         self.valid_activation(node)))

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0', matcher=lambda node:\
                isinstance(node, Conv2DParameters) and\
                self.valid_activation(node)))
        if self.match_activation and self.match_pool:
            if self.pool_after_activation:
                self.add_activation('1', sub)
                self.add_pooling('2', sub)
            else:
                self.add_pooling('1', sub)
                self.add_activation('2', sub)
            sub.add_edge(Edge('0', '1'))
            sub.add_edge(Edge('1', '2'))
        elif self.match_activation:
            self.add_activation('1', sub)
            sub.add_edge(Edge('0', '1'))
        elif self.match_pool:
            self.add_pooling('1', sub)
            sub.add_edge(Edge('0', '1'))

        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        for node in subgraph.nodes():
            if isinstance(node, Conv2DParameters):
                conv_name = node.name + "_fusion"
                break
        LOG.debug("fused nodes %s", ",".join((node.name for node in subgraph.nodes())))
        # simple node order is necessary because nodes() will not necessarily
        # be in order
        return FusionParameters(conv_name, self.fusion_type, 
                                [node for node in subgraph.dfs()])

class MatchAllGapConv(MatchGroup):
    NAME = 'fuse_gap_convs'
    DESCRIPTION = 'Fuse convolutions, pools and activations to match GAP AutoTiler operations'
    def __init__(self):
        super().__init__(\
            MatchGapConv(match_activation=True, match_pool=True, pool_after_activation=True),\
            MatchGapConv(match_activation=True, match_pool=True, pool_after_activation=False),\
            MatchGapConv(match_activation=True, match_pool=False),\
            MatchGapConv(match_activation=False, match_pool=True)\
        )
