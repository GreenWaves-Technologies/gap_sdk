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

from typing import Sequence

from generation.naming_convension import (DefaultNamingConvension,
                                          NamingConvension)

from ..dim import Dim, MoreThanOneInputError
from ..types import (ConcatParameters, EdgeParameters, InputParameters,
                     OutputParameters, Parameters, SingleInputAndOutput,
                     ReshapeParameters, ConstantInputParameters, SplitParameters)


LOG = logging.getLogger("nntool." + __name__)


def clone_dims(dims: Sequence[Dim], hints: Sequence[Dim]):
    cloned_dims = []
    for dim_idx, dim in enumerate(dims):
        if dim is None:
            cloned_dims.append(None)
            continue
        cloned_dim = dim.clone()
        if not cloned_dim.is_named and hints and hints[dim_idx]:
            cloned_dim.apply_naming_hints(hints[dim_idx])
        cloned_dims.append(cloned_dim)
    return cloned_dims


def set_input_size(node: Parameters, dims: Sequence[Dim]):
    node.in_dims = clone_dims(dims, node.in_dims_hint)
    return node.in_dims


def set_output_size(node: Parameters, dims: Sequence[Dim]):
    node.out_dims = clone_dims(dims, node.out_dims_hint)
    return node.out_dims


def connected_to_concat(G, node):
    if isinstance(node, ConcatParameters):
        return True
    if isinstance(node, ReshapeParameters):
        return connected_to_concat(G, G.out_edges(node.name)[0].to_node)
    return False


def set_out_edges_multi(G, node: Parameters, dims: Sequence[Dim], step_idx: int,
                        naming_convension: NamingConvension, edge_type: str = "in_out"):
    # clone the dims first so that the edge dims are the same objects as the node output dims
    dims = set_output_size(node, dims)
    for edge_idx, edge_group in enumerate(G.indexed_out_edges(node.name)):
        if not edge_group:
            continue
        ename = naming_convension.get_edge_name(node, step_idx, edge_type, edge_order=edge_idx)
        eparams = EdgeParameters(ename, dims[edge_idx], node, edge_idx, step_idx, edge_type)
        any_concat = False
        for edge in edge_group:
            edge.params = eparams
            if connected_to_concat(G, edge.to_node):
                any_concat = True
        eparams.is_alias = any_concat or isinstance(node, SplitParameters)
        LOG.debug("%s %s is alias %s", node.name, ename, any_concat)


def set_out_edges_one(G, node: Parameters, dim: Dim, step_idx: int,
                      naming_convension: NamingConvension, edge_type: str = "in_out"):
    ename = naming_convension.get_edge_name(node, step_idx, edge_type)
    eparams = EdgeParameters(ename, dim, node, 0, step_idx, edge_type)
    any_concat = False
    for edge in G.out_edges(node.name):
        assert edge.from_idx == 0, "Only for use with nodes that have one output"
        if connected_to_concat(G, edge.to_node):
            any_concat = True
        edge.params = eparams
    LOG.debug("%s %s is alias %s", node.name, ename, any_concat)
    eparams.is_alias = any_concat
    set_output_size(node, [dim])


def validate_one_in_edge(G, node: Parameters, expect_named: bool = True):
    edges = G.in_edges(node.name)
    if len(edges) != 1:
        raise MoreThanOneInputError()
    eparams = edges[0].params
    assert eparams is not None, "edge parameters not yet set"
    assert not expect_named or eparams.dims.has_keys(['c', 'h', 'w']), "dimensions not yet set"
    set_input_size(node, [eparams.dims])
    return eparams


def validate_multi_in_edge(G, node: Parameters, expect_named: bool = True):
    dims = []
    for edge in G.indexed_in_edges(node.name):
        if edge is None:
            dims.append(None)
            continue
        eparams = edge.params
        assert eparams is not None, "edge parameters not yet set"
        assert not expect_named or eparams.dims.has_keys(['c', 'h', 'w']), "dimensions not yet set"
        dims.append(eparams.dims)

    set_input_size(node, dims)
    return dims


def add_dimensions_concat(G, node: Parameters, step_idx: int,
                          naming_convension: NamingConvension, indexes):
    del indexes
    in_dims = validate_multi_in_edge(G, node, expect_named=False)
    out_dims = node.get_output_size(in_dims)
    set_out_edges_one(G, node, out_dims[0], step_idx, naming_convension)


def add_dimensions_constant(G, node: Parameters, step_idx: int,
                            naming_convension: NamingConvension, indexes):
    node.index = indexes['constant']
    indexes['constant'] += 1
    constant_dims = node.get_output_size(None)
    set_out_edges_one(G, node, constant_dims[0], step_idx,
                      naming_convension, edge_type="in")


def add_dimensions_input(G, node: Parameters, step_idx: int,
                         naming_convension: NamingConvension, indexes):
    node.index = indexes['input']
    indexes['input'] += 1
    input_dims = node.get_output_size(None)
    set_input_size(node, input_dims)
    set_out_edges_one(G, node, input_dims[0], step_idx,
                      naming_convension, edge_type="in")


def add_dimensions_output(G, node: Parameters, step_idx: int,
                          naming_convension: NamingConvension, indexes):
    node.index = indexes['output']
    indexes['output'] += 1
    eparams = validate_one_in_edge(G, node, expect_named=False)
    eparams.edge_type = "out"
    eparams.name = naming_convension.get_edge_name(node, step_idx, "out")
    # set the dimensions of the output node
    set_output_size(node, node.get_output_size([eparams.dims]))


def add_dimensions_unknown_single(G, node: Parameters, step_idx: int,
                                  naming_convension: NamingConvension, indexes):
    del indexes
    eparams = validate_one_in_edge(G, node, expect_named=False)
    out_dims = node.get_output_size([eparams.in_dims])
    set_out_edges_one(G, node, out_dims[0], step_idx, naming_convension)


def add_dimensions_unknown(G, node: Parameters, step_idx: int,
                           naming_convension: NamingConvension):
    in_dims = validate_multi_in_edge(G, node, expect_named=False)
    set_out_edges_multi(G, node, node.get_output_size(in_dims),
                        step_idx, naming_convension)


OP_ROUTINES = {
    InputParameters: add_dimensions_input,
    OutputParameters: add_dimensions_output,
    ConcatParameters: add_dimensions_concat,
    ConstantInputParameters: add_dimensions_constant,
    SingleInputAndOutput: add_dimensions_unknown_single
}


def add_dimensions(G, naming_convension: NamingConvension = None) -> list:
    """ Walks graph setting all edge names and dimensions
    """
    if naming_convension is None:
        naming_convension = DefaultNamingConvension(G)
    for edge in G.edges():
        edge.params = None

    steps = []
    indexes = {'input': 0, 'output': 0, 'constant': 0}
    for node in G.dfs():
        node.step_idx = len(steps)
        steps.append({'node': node})
        if node.__class__ in OP_ROUTINES:
            OP_ROUTINES[node.__class__](G, node, node.step_idx, naming_convension, indexes)
        else:
            add_dimensions_unknown(G, node, node.step_idx, naming_convension)
    return steps
