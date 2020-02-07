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

from graph.types import  Conv2DParameters, FusionParameters, PoolingParameters, ActivationParameters
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher, MatchGroup, DontReplaceError

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
        del node
        # TODO - Add specific convolution parameter checking here
        return True

    def valid_pooling(self, node):
        del node
        # TODO - Add specific pool parameter checking here
        return True

    def valid_activation(self, node):
        del node
        # TODO - Add specific pool parameter checking here
        return True

    def validate_match(self, subgraph: GraphView):
        nodes = list(subgraph.nodes())
        if self.match_pool and self.match_activation and len(nodes) == 3:
            if self.pool_after_activation:
                pool_node = nodes[2]
                # standard fusion puts relu after the pool operation
                if pool_node.pool_type == "average":
                    return False
            else:
                pool_node = nodes[1]
                act_node = nodes[2]
                if pool_node.pool_type == "average" and act_node.activation != "relu":
                    return False
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
                self.valid_convolution(node)))
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
        if not self.validate_match(subgraph):
            raise DontReplaceError()
        step = 0
        for node in subgraph.nodes():
            node.step_idx = step
            step = step + 1
            if isinstance(node, Conv2DParameters):
                conv_name = node.name + "_fusion"
                break
        LOG.debug("fused nodes %s", ",".join((node.name for node in subgraph.nodes())))
        # simple node order is necessary because nodes() will not necessarily
        # be in order
        return FusionParameters(conv_name, self.fusion_type, subgraph)

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
