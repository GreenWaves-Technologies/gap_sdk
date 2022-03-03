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

from graph.types import (ActivationParameters, Conv2DParameters,
                         ConvFusionParameters, HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         NNEdge, PoolingParameters, ReluActivationParameters,
                         SigmoidActivationParameters)
from graph.types.activations import (HTanHActivationParameters,
                                     TanHActivationParameters)
from graph.types.base import NNNodeRef
from graph.types.fusions import FusionInputParameters, FusionOutputParameters
from utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name, run_qtune_on_match)

LOG = logging.getLogger("nntool." + __name__)

VALID_ACTIVATIONS_SQ8 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters,
    TanHActivationParameters,
    HTanHActivationParameters
)

VALID_FUSIONS_SQ8 = (
    'conv_active',
    'conv_max_active',
    'conv_average_active',
    'conv_active_max',
    'conv_max',
    'conv_average',
)

VALID_ACTIVATIONS_POW2 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters
)

VALID_FUSIONS_POW2 = (
    'conv_active',
    'conv_max_active',
    'conv_average_active',
    'conv_active_max',
    'conv_max',
    'conv_average',
)


class MergeStopError(Exception):
    pass


class MergeAbortError(Exception):
    pass
class NewFusionMatch():
    def __init__(self, valid_activations, valid_fusions) -> None:
        self.fusion = None
        self.conv = None
        self.pool = None
        self.active = None
        self.valid_activations = valid_activations
        self.valid_fusions = valid_fusions
        self.order = []
        self.nodes_in_fusion = 0

    @classmethod
    def from_node(cls, G, node, valid_activations, valid_fusions) -> 'NewFusionMatch':
        matcher = cls(valid_activations, valid_fusions)
        try:
            matcher.add_node(node)
            while node:
                edges = G.out_edges(node)
                if len(edges) > 1:
                    break
                node = edges[0].to_node
                matcher.add_node(node)
        except MergeStopError:
            pass
        except MergeAbortError:
            return None
        return matcher

    @staticmethod
    def calc_fusion_type(contents, pool_type=False):
        return '_'.join(['conv' if isinstance(params, Conv2DParameters)
                         else 'active' if isinstance(params, ActivationParameters)
                         else params.pool_type if pool_type else 'pool' for params in contents])

    def can_add(self, node):
        fusion_type = self.calc_fusion_type(
            self.order + [node], pool_type=True)
        return any(valid_fusion.startswith(fusion_type) for valid_fusion in self.valid_fusions)

    def add_node(self, params, in_fusion=False):
        if in_fusion:
            self.nodes_in_fusion += 1
        if isinstance(params, ConvFusionParameters):
            if self.fusion:
                raise MergeStopError() # @IgnoreException
            self.fusion = params
            try:
                for cnode in params.contained_nodes():
                    self.add_node(cnode, in_fusion=True)
            except MergeStopError:
                raise MergeAbortError() # @IgnoreException
        elif isinstance(params, Conv2DParameters):
            if self.conv or not self.can_add(params):
                raise MergeStopError() # @IgnoreException
            self.order.append(params)
            self.conv = params
        elif isinstance(params, self.valid_activations):
            if self.active or not self.can_add(params):
                raise MergeStopError() # @IgnoreException
            self.order.append(params)
            self.active = params
        elif isinstance(params, PoolingParameters):
            if self.pool or not self.can_add(params):
                raise MergeStopError() # @IgnoreException
            self.order.append(params)
            self.pool = params
        else:
            raise MergeStopError() # @IgnoreException

    @property
    def can_fuse(self):
        return (self.calc_fusion_type(self.order, pool_type=True) in self.valid_fusions
                and len(self.order) > self.nodes_in_fusion)

    def fuse(self, G: GraphView):
        fusion_outputs = G.out_edges(self.order[-1])
        if self.fusion is None:
            fuse_node_name = G.unique_name(self.conv.name + '_fusion')
            subg = GraphView()
            inputs = [NNNodeRef(subg, FusionInputParameters(f'{fuse_node_name}_in_{idx}',
                idx=idx, dims=self.conv.in_dims[0].shape), 0) for idx in range(3)]
            in_edges = G.indexed_in_edges(self.conv)
        else:
            fuse_node_name = self.fusion.name
            subg = self.fusion.subgraph
            subg_output = subg.outputs()
            assert len(subg_output) == 1
            inputs = [NNNodeRef(subg, subg.in_edges(subg_output[0])[0].from_node, 0)]
            subg.remove_all(subg_output)
            in_edges = None
        nodes_to_fuse = self.order[self.nodes_in_fusion:]
        LOG.info(f'fusing nodes {",".join(node.name for node in nodes_to_fuse)}'
                 f' into {fuse_node_name}')
        node = None
        while nodes_to_fuse:
            node = nodes_to_fuse.pop(0)
            G.remove(node)
            inputs = [node(*inputs)]
        FusionOutputParameters(
            f'{fuse_node_name}_out_0',
            dims=node.out_dims[0].shape)(*inputs)
        if not self.fusion:
            fusion = ConvFusionParameters(
                fuse_node_name,
                fusion_type=self.fusion_type,
                subgraph=subg,
                in_dims_hint=self.conv.in_dims_hint,
                out_dims_hint=self.conv.out_dims_hint,
                in_dims=deepcopy(self.conv.in_dims),
                out_dims=deepcopy(self.order[-1].out_dims),
                inout_set=True)
            for edge in in_edges:
                G.add_edge(edge.clone(to_node=fusion))
        else: # in the fusion case the outputs will already be removed since the node after was removed
            fusion = self.fusion
        for edge in fusion_outputs:
            G.add_edge(edge.clone(from_node=fusion))
        if G.quantization:
            for node in self.order[self.nodes_in_fusion:]:
                G.quantization.move_to_fusion(node, fusion)


    @property
    def fusion_type(self):
        return self.calc_fusion_type(self.order)


@groups('*')
@match_name("fuse_gap_convs")
@run_qtune_on_match
@description(
    'Fuse convolutions, pools and activations to match GAP AutoTiler operations. Pooling and activation nodes'
    ' are also fused into existing convolution fusions.')
class MatchAllGapConv(Matcher):
    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        group_identity = kwargs.get('group_identity')
        if group_identity == 'pow2_match_group':
            valid_activations = VALID_ACTIVATIONS_POW2
            valid_fusions = VALID_FUSIONS_POW2
        else:
            valid_activations = VALID_ACTIVATIONS_SQ8
            valid_fusions = VALID_FUSIONS_SQ8

        for conv_node in G.nodes(node_classes=(Conv2DParameters, ConvFusionParameters)):
            matcher = NewFusionMatch.from_node(G, conv_node, valid_activations, valid_fusions)
            if not matcher or not matcher.can_fuse:
                continue
            has_modified_graph = True
            matcher.fuse(G)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
