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

import numpy as np
from graph.dim import Dim
from graph.types import (ConstantInputParameters, MatrixSubParameters,
                         NNEdge)
from graph.types.conv2d import BatchNormalizationParameters
from graph.types.tensor_arithmetic import MatrixMulParameters
from utils.graph import GraphView

from ..matcher import Matcher, match_name, description, groups, run_qtune_on_match

LOG = logging.getLogger("nntool." + __name__)


@match_name('batchnorm_to_discrete_ops')
@description('Convert BatchNormParameters into a set of broadcasted operations')
@groups('scaled', 'symmetric')
class FuseBatchnorm(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):

        has_modified_graph = False
        for bn_node in G.nodes(node_classes=BatchNormalizationParameters):
            w_bn = bn_node.scale / \
                np.sqrt(bn_node.epsilon + bn_node.running_variance)
            b_bn = bn_node.bias - bn_node.running_mean * bn_node.scale / \
                np.sqrt(bn_node.running_variance + bn_node.epsilon)

            mul_params = MatrixMulParameters(
                G.unique_name(f"{bn_node.name}_mul"))
            add_params = MatrixSubParameters(
                G.unique_name(f"{bn_node.name}_add"))
            broadcasted_shape = [1 if i != bn_node.axis else dim for i, dim in enumerate(
                bn_node.in_dims[0].shape)]
            scale_node = ConstantInputParameters(G.unique_name(f"{bn_node.name}_scale"), value=w_bn.reshape(
                broadcasted_shape), dims=Dim.unnamed(broadcasted_shape))
            bias_node = ConstantInputParameters(G.unique_name(f"{bn_node.name}_bias"), value=b_bn.reshape(
                broadcasted_shape), dims=Dim.unnamed(broadcasted_shape))

            from_node = G.in_edges(bn_node)[0].from_node
            to_node = G.out_edges(bn_node)[0].to_node
            G.remove(bn_node)
            G.add_edge(NNEdge(from_node, mul_params))
            G.add_edge(NNEdge(scale_node, mul_params, to_idx=1))
            G.add_edge(NNEdge(mul_params, add_params))
            G.add_edge(NNEdge(bias_node, add_params, to_idx=1))
            G.add_edge(NNEdge(add_params, to_node))

            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
