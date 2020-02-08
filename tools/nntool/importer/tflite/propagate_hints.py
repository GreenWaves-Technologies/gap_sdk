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

from copy import deepcopy

from graph.nngraph import NNGraph
from graph.types import ReshapeParameters, ConcatParameters

from utils.sparse_list import SparseList

def propagate_downwards(G: NNGraph):
    for node in G.dfs():
        # First propagate the in dim hints to the out dim hints
        # Any node that does not want this to happen should set its out dim hints

        if node.in_dims_hint is not None:
            if isinstance(node, ReshapeParameters):
                assert len(node.old_shape) == len(node.in_dims_hint[0]), "reshape doesn't match input"
                node.old_shape.apply_naming_hints(node.in_dims_hint[0])
            else:
                if node.out_dims_hint is None:
                    node.out_dims_hint = deepcopy(node.in_dims_hint)

        # if we have an out dim hint then propagate it to downstream nodes
        if node.out_dims_hint is not None:
            for edge in G.out_edges(node.name):
                
                hint = node.out_dims_hint[edge.from_idx]
                if edge.to_node.in_dims_hint is None:
                    edge.to_node.in_dims_hint = SparseList()
                if edge.to_node.in_dims_hint[edge.to_idx] is None:
                    edge.to_node.in_dims_hint[edge.to_idx] = hint

def propagate_upwards(G: NNGraph):
    for node in G.dfs(reverse=True):
        # First propagate the out dim hints to the in dim hints
        # Any node that does not want this to happen should set its in dim hints

        if node.out_dims_hint is not None:
            if isinstance(node, ReshapeParameters):
                assert len(node.shape) == len(node.out_dims_hint[0])
                node.shape.apply_naming_hints(node.out_dims_hint[0])
                if node.in_dims_hint is None:
                    node.in_dims_hint = SparseList([["%s" % i for i in range(len(node.old_shape))]])
            else:
                if node.in_dims_hint is None:
                    node.in_dims_hint = deepcopy(node.out_dims_hint)

        # if we have an in dim hint then propagate it to upstream nodes
        if node.in_dims_hint is not None:
            for edge in G.in_edges(node.name):
                hint = node.in_dims_hint[edge.to_idx]
                if edge.from_node.out_dims_hint is None:
                    edge.from_node.out_dims_hint = SparseList()
                if edge.from_node.out_dims_hint[edge.from_idx] is None:
                    edge.from_node.out_dims_hint[edge.from_idx] = hint

def propagate_hints(G):
    propagate_downwards(G)
    propagate_upwards(G)
