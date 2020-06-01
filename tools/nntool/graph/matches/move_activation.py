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
from copy import deepcopy
from graph.types import (ActivationParameters, ConcatParameters,
                         Conv2DParameters, FcParameters, GlobalPoolParameters,
                         MatrixAddParameters, MatrixMulParameters,
                         PoolingParameters, ReshapeParameters,
                         TransposeParameters)
from utils.graph import Edge, GraphView
from utils.node_id import NodeId

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


class LocationNotFoundError(Exception):
    pass


class MoveActivationsMatcher(Matcher):

    ValidNodesToPass = None
    ValidFusions = None

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
        elif isinstance(edge.from_node, self.ValidNodesToPass):
            in_edge = G.in_edges(edge.from_node.name)[0]
            yield from self.find_home_for_activation(G,
                                                     activation,
                                                     edge=in_edge)
        elif isinstance(edge.from_node, self.ValidFusions):
            yield edge
        else:
            raise LocationNotFoundError()

    @staticmethod
    def move_activation(G, activation, edges):
        nid = NodeId(activation)
        qrec = G.quantization[nid] if G.quantization and nid in G.quantization else None
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
            if qrec:
                from_qrec = G.quantization[NodeId(edge.from_node)]
                new_qrec = deepcopy(qrec)
                new_qrec.in_qs[0] = deepcopy(from_qrec.out_qs[edge.from_idx])
                G.quantization[NodeId(new_activation)] = new_qrec
                G.quantization.propagate(
                    G, new_activation, new_edge.from_node, qtype=new_qrec.out_qs[0])

    def match(self, G: GraphView, set_identity: bool = True):
        activations = [node for node in G.nodes(
        ) if isinstance(node, ActivationParameters)]
        activations = filter(lambda n: not isinstance(
            G.in_edges(n.name)[0].from_node, self.ValidFusions), activations)
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


class MoveActivationsMatcherScale8(MoveActivationsMatcher):
    NAME = "move_activations_scale8"
    DESCRIPTION = "Tries to move activations so they are after layers that they can be fused with. \
        Should be run before match_gap_* fusions. Compatible with AutoTiler SQ8 kernels."

    ValidNodesToPass = (ReshapeParameters, TransposeParameters)
    ValidFusions = (Conv2DParameters, FcParameters, PoolingParameters, PoolingParameters,
                    GlobalPoolParameters, MatrixAddParameters, MatrixMulParameters)


class MoveActivationsMatcherPow2(MoveActivationsMatcher):
    NAME = "move_activations_pow2"
    DESCRIPTION = "Tries to move activations so they are after layers that they can be fused with. \
        Should be run before match_gap_* fusions. Compatible with AutoTiler POW2 kernels."

    ValidNodesToPass = (ReshapeParameters, TransposeParameters)
    ValidFusions = (Conv2DParameters, FcParameters, PoolingParameters)
