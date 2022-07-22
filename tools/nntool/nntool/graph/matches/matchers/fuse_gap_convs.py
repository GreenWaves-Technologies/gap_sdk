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

from nntool.expressions.symbolic.iteration_space import IterationSpace
from nntool.graph.types import (ActivationNodeBase, Conv2DNode,
                         ConvFusionNode, ExpressionFusionNode,
                         FusionInputNode, FusionOutputNode,
                         HSigmoidNode,
                         HSwishNode, HTanHNode,
                         LeakyNode, PoolingNodeBase,
                         ReluNode, SigmoidNode,
                         TanHNode)
from nntool.graph.types.base import NNNodeRef
from nntool.graph.types.fusions import LinearFusionNode
from nntool.graph.types.linear import LinearNode
from nntool.utils.exception import NNToolInternelError
from nntool.utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name,
                       run_again_on_match, run_qtune_on_match)

LOG = logging.getLogger(__name__)

VALID_ACTIVATIONS_SQ8 = (
    ReluNode,
    LeakyNode,
    HSigmoidNode,
    HSwishNode,
    SigmoidNode,
    TanHNode,
    HTanHNode,
    ExpressionFusionNode
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
    ReluNode,
    LeakyNode,
    HSigmoidNode,
    HSwishNode,
    SigmoidNode
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
    def __init__(self, filter_class, valid_activations, valid_fusions, validators=None) -> None:
        self.filter_dim_class = filter_class
        self.fusion = None
        self.filter_dim = None
        self.pool = None
        self.active = None
        self.valid_activations = valid_activations
        self.valid_fusions = valid_fusions
        self.order = []
        self.nodes_in_fusion = 0
        self.validators = {} if validators is None else validators

    @classmethod
    def from_node(cls, G, node, valid_activations, valid_fusions, validators=None) -> 'NewFusionMatch':
        filter_class = Conv2DNode if isinstance(node, ConvFusionNode) else node.__class__
        matcher = cls(filter_class, valid_activations, valid_fusions, validators=validators)
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
        return '_'.join(['conv' if isinstance(params, Conv2DNode)
                         else 'linear' if isinstance(params, LinearNode)
                         else 'active' if isinstance(params, (ActivationNodeBase, ExpressionFusionNode))
                         else params.pool_type if pool_type else 'pool' for params in contents])

    def can_add(self, node):
        fusion_type = self.calc_fusion_type(
            self.order + [node], pool_type=True)
        return any(valid_fusion.startswith(fusion_type) for valid_fusion in self.valid_fusions)

    def add_node(self, params, in_fusion=False):
        if in_fusion:
            self.nodes_in_fusion += 1
        if params.__class__ in self.validators and not self.validators[params.__class__](params):
            raise MergeStopError()  # @IgnoreException
        if isinstance(params, ConvFusionNode):
            if self.fusion:
                raise MergeStopError()  # @IgnoreException
            self.fusion = params
            try:
                for cnode in params.contained_nodes():
                    self.add_node(cnode, in_fusion=True)
            except MergeStopError as ex:
                raise MergeAbortError() from ex  # @IgnoreException
        elif isinstance(params, self.filter_dim_class):
            if self.filter_dim or not self.can_add(params):
                raise MergeStopError()  # @IgnoreException
            self.order.append(params)
            self.filter_dim = params
        elif isinstance(params, self.valid_activations):
            if self.active or not self.can_add(params):
                raise MergeStopError()  # @IgnoreException
            self.order.append(params)
            self.active = params
        elif isinstance(params, PoolingNodeBase):
            if self.pool or not self.can_add(params):
                raise MergeStopError()  # @IgnoreException
            self.order.append(params)
            self.pool = params
        else:
            raise MergeStopError()  # @IgnoreException

    @property
    def can_fuse(self):
        return (self.calc_fusion_type(self.order, pool_type=True) in self.valid_fusions
                and len(self.order) > self.nodes_in_fusion)

    def fuse(self, G: GraphView):
        fusion_outputs = G.out_edges(self.order[-1])
        if self.fusion is None:
            fuse_node_name = G.unique_name(self.filter_dim.name + '_fusion')
            subg = GraphView()
            inputs = [NNNodeRef(subg, FusionInputNode(f'{fuse_node_name}_in_{idx}',
                                                            idx=idx, dims=self.filter_dim.in_dims[0].shape), 0) for idx in range(3)]
            in_edges = G.indexed_in_edges(self.filter_dim)
        else:
            fuse_node_name = self.fusion.name
            subg = self.fusion.subgraph
            subg_output = subg.outputs()
            assert len(subg_output) == 1
            inputs = [NNNodeRef(subg, subg.in_edges(
                subg_output[0])[0].from_node, 0)]
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
        FusionOutputNode(
            f'{fuse_node_name}_out_0',
            dims=node.out_dims[0].shape)(*inputs)
        if not self.fusion:
            if isinstance(self.filter_dim, Conv2DNode):
                fusion = ConvFusionNode(
                    fuse_node_name,
                    fusion_type=self.fusion_type,
                    subgraph=subg,
                    in_dims_hint=self.filter_dim.in_dims_hint,
                    out_dims_hint=self.filter_dim.out_dims_hint,
                    in_dims=deepcopy(self.filter_dim.in_dims),
                    out_dims=deepcopy(self.order[-1].out_dims),
                    inout_set=True)
            elif isinstance(self.filter_dim, LinearNode):
                fusion = LinearFusionNode(
                    fuse_node_name,
                    in_dims_hint=self.filter_dim.in_dims_hint,
                    out_dims_hint=self.filter_dim.out_dims_hint,
                    fusion_type=self.fusion_type,
                    in_dims=deepcopy(self.filter_dim.in_dims),
                    out_dims=deepcopy(self.order[-1].out_dims),
                    subgraph=subg,
                    inout_set=True)
            else:
                raise NNToolInternelError(f'{self.filter_dim.name} is an unexpected type {self.filter_dim.__class__.__name__}')
            for edge in in_edges:
                G.add_edge(edge.clone(to_node=fusion))
        else:  # in the fusion case the outputs will already be removed since the node after was removed
            fusion = self.fusion
        for edge in fusion_outputs:
            G.add_edge(edge.clone(from_node=fusion))
        if G.quantization:
            for node in self.order[self.nodes_in_fusion:]:
                G.quantization.move_to_fusion(node, fusion)

    @property
    def fusion_type(self):
        return self.calc_fusion_type(self.order)


def fuse_expression_activation(params: ExpressionFusionNode):
    if not (len(params.input_symbols) == 1 and len(params.output_symbols) == 1 and
            params.in_dims[0].shape == params.out_dims[0].shape):
        return False
    itspace = IterationSpace(params.func_col)
    if len(itspace.spaces) != 1:
        return False
    return True


@groups('*')
@match_name("fuse_gap_convs")
@run_qtune_on_match
@run_again_on_match('common_fused_expressions')
@description(
    'Fuse convolutions, pools and activations to match GAP AutoTiler operations. Pooling and activation nodes'
    ' are also fused into existing convolution fusions.')
class MatchAllGapConv(Matcher):
    def _match(self, G: GraphView, **kwargs):
        has_modified_graph = False
        group_identity = kwargs.get('group_identity')
        if group_identity == 'pow2_match_group':
            valid_activations = VALID_ACTIVATIONS_POW2
            valid_fusions = VALID_FUSIONS_POW2
        else:
            valid_activations = VALID_ACTIVATIONS_SQ8
            valid_fusions = VALID_FUSIONS_SQ8

        for conv_node in G.nodes(node_classes=(Conv2DNode, ConvFusionNode)):
            matcher = NewFusionMatch.from_node(G, conv_node, valid_activations, valid_fusions,
                                               validators={ExpressionFusionNode: fuse_expression_activation})
            if not matcher or not matcher.can_fuse:
                continue
            has_modified_graph = True
            matcher.fuse(G)



        return has_modified_graph
