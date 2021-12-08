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

from graph.types.activations import TanHActivationParameters
import logging
from copy import deepcopy

from graph.types import (ActivationParameters, Conv2DParameters,
                         ConvFusionParameters, HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         NNEdge, PoolingParameters, ReluActivationParameters,
                         SigmoidActivationParameters)
from quantization.new_qrec import QRec
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, description, groups, match_name

LOG = logging.getLogger("nntool." + __name__)

VALID_ACTIVATIONS_SQ8 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters,
    TanHActivationParameters
)

VALID_ACTIVATIONS_POW2 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters
)


class FusionMatch():
    def __init__(self, valid_activations) -> None:
        self.conv = None
        self.pool = None
        self.active = None
        self.tensor_order = None
        self.valid_activations = valid_activations
        self.order = []

    def add_node(self, params):
        if isinstance(params, Conv2DParameters):
            if self.conv:
                return None
            self.tensor_order = params.ker_out_order[0]
            self.order.append(params)
            self.conv = params
            return self
        elif isinstance(params, self.valid_activations):
            if self.active:
                return None
            self.order.append(params)
            self.active = params
            return self
        elif isinstance(params, PoolingParameters):
            if self.pool:
                return None
            if self.tensor_order != params.ker_in_order[0]:
                return None
            self.order.append(params)
            self.pool = params
            return self
        else:
            return None

    @property
    def fusion_type(self):
        return '_'.join(['conv' if isinstance(params, Conv2DParameters)
                         else 'active' if isinstance(params, ActivationParameters)
                         else 'pool' for params in self.order])


@groups('*')
@match_name("fuse_gap_convs")
@description('Fuse convolutions, pools and activations to match GAP AutoTiler operations')
class MatchAllGapConv(Matcher):

    def get_node_list(self, G, params, valid_activations, result=None):
        if result is None:
            result = FusionMatch(valid_activations)
        if not result.add_node(params):
            return result
        out_edges = G.out_edges(params.name)
        if len(out_edges) > 1:
            return result
        return self.get_node_list(G, out_edges[0].to_node, valid_activations, result=result)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        group_identity = kwargs.get('group_identity')
        if group_identity == 'pow2_match_group':
            valid_activations = VALID_ACTIVATIONS_POW2
        else:
            valid_activations = VALID_ACTIVATIONS_SQ8
        for conv_node in [params for params in G.nodes() if isinstance(params, Conv2DParameters)]:
            node_list = self.get_node_list(G, conv_node, valid_activations)
            if node_list is None or len(node_list.order) < 2:
                continue
            if node_list.fusion_type == 'conv_active_pool':
                if node_list.pool.pool_type == "average":
                    node_list.order = node_list.order[:2:]
                    node_list.pool = None
            elif node_list.fusion_type == 'conv_pool_active':
                # NOTE: This is only for old POW2 kernels - SQ8 can handle this
                if node_list.pool.pool_type == "average" and node_list.active.activation != "relu":
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
            input_mapping = [[(node_list.conv, idx)] for idx in range(3)]
            output_mapping = [(last_node, 0)]
            pnode = ConvFusionParameters(
                node_list.conv.name + '_fusion',
                fusion_type=node_list.fusion_type,
                subgraph=subgraph,
                in_dims_hint=node_list.conv.in_dims_hint,
                out_dims_hint=node_list.conv.out_dims_hint,
                in_dims=deepcopy(node_list.conv.in_dims),
                out_dims=deepcopy(node_list.order[-1].out_dims),
                input_mapping=input_mapping,
                output_mapping=output_mapping)
            if G.quantization:
                qrecs = G.quantization.get_all(pnode.contained_nodes())
                if qrecs:
                    # TODO - stats
                    prec = QRec.copy_ktype(
                        qrecs[0], in_qs=deepcopy(qrecs[0].in_qs), out_qs=deepcopy(qrecs[-1].out_qs))
                    for node in pnode.contained_nodes():
                        G.quantization.move_to_fusion(node, pnode)
                    G.quantization[NodeId(pnode)] = prec
            in_edges = G.in_edges(node_list.conv.name)
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
