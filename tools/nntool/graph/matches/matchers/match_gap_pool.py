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

import numpy as np
from graph.types import (HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         NNEdge, PoolingParameters, ReluActivationParameters,
                         SigmoidActivationParameters)
from graph.types.fusions import ActivationFusion
from graph.types.global_pooling import GlobalPoolingParameters
from quantization.new_qrec import QRec
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, description, groups, match_name, run_after

LOG = logging.getLogger("nntool." + __name__)


VALID_ACTIVATIONS_SQ8 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters
)

VALID_ACTIVATIONS_SQ8_WO_POOL = VALID_ACTIVATIONS_SQ8

VALID_ACTIVATIONS_POW2 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters
    # SigmoidActivationParameters
)

VALID_ACTIVATIONS_POW2_WO_POOL = (
    ReluActivationParameters,
)


class FusionMatch():
    def __init__(self, valid_activations, valid_activations_wo_pool) -> None:
        self.pool = None
        self.active = None
        self.valid_activations = valid_activations
        self.valid_activations_wo_pool = valid_activations_wo_pool
        self.order = []

    def add_node(self, params):
        if isinstance(params, (PoolingParameters, GlobalPoolingParameters)):
            if self.pool:
                return None
            self.order.append(params)
            self.pool = params
            return self
        elif (self.pool and isinstance(params, self.valid_activations)) or isinstance(params, self.valid_activations_wo_pool):
            if self.active:
                return None
            self.order.append(params)
            self.active = params
            return self
        else:
            return None

    @property
    def fusion_type(self):
        return '_'.join(['pool' if isinstance(params, (PoolingParameters, GlobalPoolingParameters))
                         else 'active' for params in self.order])


@groups('*')
@match_name("fuse_gap_pool")
@description('Fuse pooling layers and activations to match GAP AutoTiler operations')
@run_after('fuse_gap_convs')
class MatchGapPool(Matcher):

    def get_node_list(self, G, params, valid_activations, valid_activations_wo_pool, result=None):
        if result is None:
            result = FusionMatch(valid_activations, valid_activations_wo_pool)
        if not result.add_node(params):
            return result
        out_edges = G.out_edges(params.name)
        if len(out_edges) > 1:
            return result
        return self.get_node_list(G, out_edges[0].to_node, valid_activations, valid_activations_wo_pool, result=result)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        group_identity = kwargs.get('group_identity')
        if group_identity == 'pow2_match_group':
            valid_activations = VALID_ACTIVATIONS_POW2
            valid_activations_wo_pool = VALID_ACTIVATIONS_POW2_WO_POOL
        else:
            valid_activations = VALID_ACTIVATIONS_SQ8
            valid_activations_wo_pool = VALID_ACTIVATIONS_SQ8_WO_POOL
        for pool_node in G.nodes(node_classes=(PoolingParameters, GlobalPoolingParameters)):
            node_list = self.get_node_list(
                G, pool_node, valid_activations, valid_activations_wo_pool)
            if node_list is None or len(node_list.order) < 2:
                continue
            LOG.info("fusing nodes %s", ",".join(
                (node.name for node in node_list.order)))
            has_modified_graph = True
            subgraph = GraphView()
            last_node = None
            for node in node_list.order:
                if last_node is not None:
                    subgraph.add_edge(
                        NNEdge(from_node=last_node, to_node=node))
                last_node = node
            input_mapping = [[(node_list.pool, 0)]]
            output_mapping = [(last_node, 0)]
            pnode = ActivationFusion(
                node_list.pool.name + '_fusion',
                fusion_type=node_list.fusion_type,
                subgraph=subgraph,
                input_mapping=input_mapping,
                output_mapping=output_mapping)
            if G.quantization:
                # TODO - stats
                qrecs = G.quantization.get_all(pnode.contained_nodes())
                if qrecs:
                    prec = QRec.copy_ktype(
                        qrecs[0], in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                    for node in pnode.contained_nodes():
                        G.quantization.move_to_fusion(node, pnode)
                        if isinstance(node, GlobalPoolingParameters):
                            # Global pooling fused with activations need to have only the activation scale
                            G.quantization[NodeId(pnode, node)].out_qs[0] = deepcopy(
                                G.quantization[NodeId(pnode, node)].in_qs[0])
                            G.quantization[NodeId(
                                pnode, node)].out_qs[0].dtype = np.int32
                    G.quantization[NodeId(pnode)] = prec
            in_edges = G.in_edges(node_list.pool.name)
            out_edges = G.out_edges(last_node.name)
            for node in node_list.order:
                G.remove(node)
            for edge in in_edges:
                G.add_edge(NNEdge(edge.from_node, pnode,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx))
            for edge in out_edges:
                G.add_edge(NNEdge(pnode, edge.to_node,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
