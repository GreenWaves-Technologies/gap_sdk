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
from graph.types import (ConstantInputParameters, MatMulOpParameters, MatMulTransposedParameters,
                         NNEdge)
from graph.types.conv2d import BatchNormalizationParameters
from utils.graph import GraphView

from ..matcher import Matcher, match_name, description, groups, run_qtune_on_match

LOG = logging.getLogger("nntool." + __name__)


@match_name('fuse_batchnorm')
@description('Fuse batch normalization into MatMul')
@groups('scaled', 'symmetric')
@run_qtune_on_match
class FuseBatchnorm(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):

        has_modified_graph = False
        nodes = []
        for node in G.nodes(node_classes=BatchNormalizationParameters):
            in_node = G.indexed_in_edges(node)[0].from_node
            if isinstance(in_node, MatMulOpParameters):
                nodes.append((node, in_node))

        for bn_node, filt_node in nodes:
            filt_in_edges = G.indexed_in_edges(filt_node.name)
            weights_node = filt_in_edges[1].from_node
            biases_node = filt_in_edges[2].from_node if len(
                filt_in_edges) > 2 else None
            w_bn = bn_node.scale / np.sqrt(bn_node.epsilon + bn_node.running_variance)
            if not isinstance(weights_node, ConstantInputParameters):
                continue
            weights = weights_node.dqvalue
            if len(w_bn) > 1:
                if not isinstance(filt_node, MatMulTransposedParameters):
                    weights = np.swapaxes(weights.copy(), -2, -1)
                if weights.shape[-2] != len(w_bn):
                    LOG.info(f'{filt_node.name} - weights shape does not match batch norm')
                    continue
            if biases_node is None:
                biases = np.zeros((weights.shape[-1],))
                biases_node = ConstantInputParameters(
                    G.unique_name(f'{filt_node.name}_biases'), value=biases)
                G.add_edge(NNEdge(from_node=biases,
                                  to_node=filt_node, to_idx=2))
            elif not isinstance(biases_node, ConstantInputParameters):
                continue
            else:
                biases = biases_node.dqvalue
            # fold batch norm into conv weights and biases
            if len(w_bn) > 1:
                w_bn = np.diag(w_bn)
                weights = np.matmul(w_bn, weights)
            else:
                weights = weights * w_bn
            biases = bn_node.bias + ((biases - bn_node.running_mean) *
                                     bn_node.scale / np.sqrt(bn_node.running_variance + bn_node.epsilon))
            if len(w_bn) > 1 and not isinstance(filt_node, MatMulTransposedParameters):
                weights = np.swapaxes(weights, -2, -1)
            weights_node.value = weights
            biases_node.value = biases
            G.remove_and_reconnect(bn_node, edge_class=NNEdge)
            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
