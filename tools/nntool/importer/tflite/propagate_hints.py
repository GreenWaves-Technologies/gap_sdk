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

from graph.nngraph import NNGraph
from graph.dim import Dim
from graph.types import (ConstantInputParameters,
                         MatrixBroadcastedLinearOpParameters,
                         ReshapeParameters, GlobalPoolParameters,
                         InputParameters, ConcatParameters)
from utils.sparse_list import SparseList

LOG = logging.getLogger('nntool.' + __name__)

def propagate_downwards(G: NNGraph):
    for node in G.dfs():
        # First propagate the in dim hints to the out dim hints
        # Any node that does not want this to happen should set its out dim hints

        if node.in_dims_hint is not None:
            if isinstance(node, ReshapeParameters):
                if len(node.old_shape) == len(node.in_dims_hint[0]):
                    LOG.debug("set reshape %s in dims hint %s", node.name, node.in_dims_hint[0])
                    node.old_shape.apply_naming_hints(node.in_dims_hint[0])
            elif isinstance(node, GlobalPoolParameters):
                if node.keep_dims:
                    node.out_dims_hint = deepcopy(node.in_dims_hint)
            elif isinstance(node, MatrixBroadcastedLinearOpParameters):
                max_hint = None
                for hint in node.in_dims_hint:
                    if hint is not None and (max_hint is None or len(hint) > len(max_hint)):
                        max_hint = hint
                if max_hint is not None:
                    node.out_dims_hint = [max_hint]
            elif isinstance(node, ConcatParameters):
                # if any incoming edge of the concat doesn't have a hint
                # set it the same as the others
                any_in_hint = next((hint for hint in node.in_dims_hint if hint is not None), None)
                if any_in_hint:
                    LOG.debug("set concat %s in dims hint %s", node.name, any_in_hint)
                    for edge in G.in_edges(node.name):
                        if not node.in_dims_hint[edge.to_idx]:
                            node.in_dims_hint[edge.to_idx] = any_in_hint
            else:
                if node.out_dims_hint is None:
                    node.out_dims_hint = deepcopy(node.in_dims_hint)

        # if we have an out dim hint then propagate it to downstream nodes
        if node.out_dims_hint is not None:
            LOG.debug("propagate down hint from %s", node.name)
            for edge in G.out_edges(node.name):
                hint = node.out_dims_hint[edge.from_idx]
                if hint is None:
                    continue
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
                if len(node.shape) < len(node.out_dims_hint[0]):
                    node.shape = Dim.unnamed(
                        ([1] * (len(node.out_dims_hint[0]) - len(node.shape))) + node.shape.shape)
                node.shape.apply_naming_hints(node.out_dims_hint[0])
                if node.in_dims_hint is None:
                    node.in_dims_hint = SparseList([["%s" % i for i in range(len(node.old_shape))]])
            elif isinstance(node, MatrixBroadcastedLinearOpParameters):
                node.in_dims_hint = [node.out_dims_hint[0]] * 2
            elif isinstance(node, GlobalPoolParameters):
                if node.keep_dims:
                    node.in_dims_hint = deepcopy(node.out_dims_hint)
            elif isinstance(node, ConstantInputParameters) and not node.dims.is_named:
                node.dims.apply_naming_hints(node.out_dims_hint[0])
            else:
                if node.in_dims_hint is None:
                    node.in_dims_hint = deepcopy(node.out_dims_hint)

        # if we have an in dim hint then propagate it to upstream nodes
        if node.in_dims_hint is not None:
            for edge in G.in_edges(node.name):
                hint = node.in_dims_hint[edge.to_idx]
                if hint is None:
                    continue
                if edge.from_node.out_dims_hint is None:
                    edge.from_node.out_dims_hint = SparseList()
                if edge.from_node.out_dims_hint[edge.from_idx] is None:
                    edge.from_node.out_dims_hint[edge.from_idx] = hint
                    if isinstance(edge.from_node, InputParameters):
                        assert edge.from_idx == 0, "input node should only have one output"
                        dims_len = len(edge.from_node.dims)
                        hint_len = len(hint)
                        if dims_len < hint_len:
                            edge.from_node.dims = Dim.unnamed(
                                [1] * (hint_len - dims_len) + edge.from_node.dims.shape)


def propagate_hints(G):
    propagate_downwards(G)
    propagate_upwards(G)
