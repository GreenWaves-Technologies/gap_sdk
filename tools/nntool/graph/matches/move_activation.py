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

from utils.graph import GraphView, Edge

from ..types.conv2d import Conv2DParameters
from ..types.linear import FcParameters
from ..types.others import (ActivationParameters, ConcatParameters,
                            ReshapeParameters, TransposeParameters)
from ..types.pooling import PoolingParameters
from .matcher import Matcher

VALID_FUSIONS = (Conv2DParameters, FcParameters, PoolingParameters)
VALID_NODES_TO_PASS = (ReshapeParameters, TransposeParameters)

LOG = logging.getLogger("nntool." + __name__)


class LocationNotFoundError(Exception):
    pass


class MoveActivationsMatcher(Matcher):
    NAME = "move_activations"
    DESCRIPTION = "Tries to move activations so they are after layers that they can be fused with. \
        Should be run before match_gap_* fusions."

    def find_home_for_activation(self,
                                 G,
                                 activation,
                                 edge=None):
        if edge is None:
            in_edge = G.in_edges(activation.name)[0]
            yield from self.find_home_for_activation(G,
                                                     activation,
                                                     edge=in_edge)
        elif isinstance(edge.from_node, ConcatParameters):
            for in_edge in G.in_edges(edge.from_node.name):
                yield from self.find_home_for_activation(G,
                                                         activation,
                                                         edge=in_edge)
        elif isinstance(edge.from_node, VALID_NODES_TO_PASS):
            in_edge = G.in_edges(edge.from_node.name)[0]
            yield from self.find_home_for_activation(G,
                                                     activation,
                                                     edge=in_edge)
        elif isinstance(edge.from_node, VALID_FUSIONS):
            yield edge
        else:
            raise LocationNotFoundError()

    @staticmethod
    def move_activation(G, activation, edges):
        ain_edge = G.in_edges(activation.name)[0]
        aout_edge = G.out_edges(activation.name)[0]
        G.remove(activation)
        new_edge = Edge(from_node=ain_edge.from_node, to_node=aout_edge.to_node,
                        from_idx=ain_edge.from_idx, to_idx=aout_edge.to_idx)
        G.add_edge(new_edge)
        cnt = 0
        for edge in edges:
            LOG.info("Moving activation %s between %s and %s",
                     activation.name, edge.from_node.name, edge.to_node.name)
            if cnt > 0:
                new_activation = activation.clone("{}_{}".format(new_activation.name, cnt))
            else:
                new_activation = activation
            cnt += 1
            new_activation.in_dims = [edge.from_node.out_dims[edge.from_idx].clone()]
            new_activation.out_dims = [edge.to_node.in_dims[edge.to_idx].clone()]
            G.insert_node(new_activation, edge.from_node, edge.to_node,
                          from_idx=edge.from_idx, to_idx=edge.to_idx)

    def match(self, G: GraphView, set_identity: bool = True):
        activations = [node for node in G.nodes(
        ) if isinstance(node, ActivationParameters)]
        activations = filter(lambda n: not isinstance(
            G.in_edges(n.name)[0].from_node, VALID_FUSIONS), activations)
        can_be_moved = []
        for activation in activations:
            try:
                edges = list(self.find_home_for_activation(G, activation))
                LOG.info("Activation %s can be moved", activation.name)
                can_be_moved.append({'activation': activation, 'edges': edges})
            except LocationNotFoundError:
                LOG.info("Activation %s cannot be moved", activation.name)
        for move in can_be_moved:
            self.move_activation(G, move['activation'], move['edges'])

        if set_identity:
            self.set_identity(G)


# Find activation
# check node in front
# if it isn't conv, linear or pool
