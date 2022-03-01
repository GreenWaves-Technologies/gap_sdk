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
from functools import reduce
from typing import OrderedDict

import numpy as np
from graph.dim import Dim
from graph.manipulations.eliminate_transposes.transpose_helpers import \
    reverse_transpose
from graph.types import (NNEdge, OutputParameters, ReshapeParameters,
                         SplitParameters, StridedSliceParameters,
                         TransposeParameters)
from quantization.quantizer.new_quantizer import NewQuantizer
from utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before
from .remove_unnecessary_quantize_operators import \
    RemoveUnnecessaryQuantizeOperators

LOG = logging.getLogger("nntool." + __name__)


def sort_recursive(tree):
    if 'node' in tree:
        return tree
    return OrderedDict(sorted([(k, sort_recursive(v)) for k, v in tree.items()], key=lambda x: x[0]))


def build_slice_tree(slices, slice_nodes):
    # Build a sorted tree of the slices.
    tree = {}
    for idx, slice_set in enumerate(zip(*slices)):
        leaf = tree
        for sub_slice in slice_set:
            leaf = leaf.setdefault(sub_slice, {})
        if slice_nodes is not None:
            leaf['node'] = slice_nodes[idx]

    return sort_recursive(tree)


def check_tree_complete(tree, lengths, each_complete=False):
    # Check that each subtree is a complete set of slices of all the iteration space below it.
    # if it isn't there are gaps and we cannot slice properly
    # if any space has a length greater than one the individual spaces below it must be complete
    if not tree or 'node' in tree:
        return True
    slices = list(tree.keys())
    if each_complete:
        if set(sl[1]-sl[0] for sl in slices) != {lengths[0]}:
            return False
    else:
        # length of all the slices should equal the level interation space
        length = reduce(lambda s, x: s+x[1]-x[0], slices, 0)
        if length != lengths[0]:
            return False
    # slices must not overlap
    if not all(sl[1] == slices[i+1][0] for i, sl in enumerate(slices[:-1:])):
        return False
    return all(
        check_tree_complete(
            tree[k],
            lengths[1::],
            each_complete=each_complete or (k[1]-k[0]) > 1)
        for k in tree)


def build_slices_from_tree(tree, lengths, start=0, shape=None, multiple=1):
    if shape is None:
        shape = []
    slices = []
    ilen = np.prod(lengths[1::]) if lengths else 1
    for sl, subtree in tree.items():
        if subtree and 'node' not in subtree:
            sub_shape = shape.copy()
            sub_shape.append((sl[1]-sl[0]))
            slices.extend(build_slices_from_tree(
                subtree, lengths[1::], start=start+ilen*sl[0], multiple=(sl[1]-sl[0])*multiple, shape=sub_shape))
        else:
            slice_desc = (start+sl[0], start+sl[0]+(sl[1]-sl[0])*multiple, 1)
            shape_desc = tuple(shape+[sl[1]-sl[0]])
            if 'node' in subtree:
                slices.append((slice_desc, shape_desc, subtree['node']))
            else:
                slices.append((slice_desc, shape_desc))
    return slices


def combine_slices(lengths, slices, slice_nodes=None):
    sl_tree = build_slice_tree(slices, slice_nodes)
    if not check_tree_complete(sl_tree, lengths):
        return None
    return build_slices_from_tree(sl_tree, lengths)


def slices_to_sizes(slices_and_shapes, shape_rest):
    sizes = [sands[0][1]-sands[0][0] for sands in slices_and_shapes]
    shapes = [tuple(list(sands[1]) + list(shape_rest))
              for sands in slices_and_shapes]
    nodes = [sands[2] for sands in slices_and_shapes]
    return sizes, shapes, nodes


@ match_name("slice_to_split")
@ description("collects slices from a single node and converts to a single split")
@ run_before('remove_noops', 'insert_copies')
@ groups('*')
class SliceToSplitMatch(Matcher):
    @ staticmethod
    def slice_to_split(G, slice_nodes, slices):
        slice_node = slice_nodes[0]
        in_dims = slice_node.in_dims[0].shape
        slices = [sl[0] for sl in slices]
        if any(sl[2] != 1 for sl in slices):
            return
        szes = tuple([sl[1] - sl[0] for sl in slices])
        # find sliced axes that differ
        diff_axis = tuple(idx for idx, (d1, d2) in enumerate(
            zip(szes, in_dims)) if d1 != d2)
        if len(diff_axis) != 1:
            return
        # good to convert to a split
        axis = diff_axis[0]
        axis_slice = slices[axis]
        axis_dim = in_dims[axis]
        outs = []
        splits = []
        if axis_slice[0] > 0:
            splits.append(axis_slice[0])
            oparams = OutputParameters(G.unique_name('unused'))
            oparams.at_options.allocate = 1
            outs.append(
                ((oparams, 0),))
        splits.append(axis_slice[1] - axis_slice[0])
        outs.append([(edge.to_node, edge.to_idx)
                     for edge in G.out_edges(slice_node.name)])
        if axis_slice[1] < axis_dim:
            splits.append(axis_dim - axis_slice[1])
            oparams = OutputParameters(G.unique_name('unused'))
            oparams.at_options.allocate = 1
            outs.append(
                ((oparams, 0),))
        in_edge = G.in_edges(slice_node.name)[0]
        G.remove(slice_node)
        act_slices, out_shapes, axis = SplitParameters.get_splits(
            in_dims, axis, splits=splits)
        LOG.info('replacing strided slice %s with split with %s redundant outputs',
                 slice_node.name, len(outs) - 1)
        if axis != 0:
            LOG.warning('adjust needs to be rerun')
        split_params = SplitParameters(slice_node.name, act_slices=act_slices,
                                       out_shapes=out_shapes, axis=axis)
        G.add_edge(NNEdge(from_node=in_edge.from_node,
                          from_idx=in_edge.from_idx, to_node=split_params))
        for out_idx, out_cons in enumerate(outs):
            for out_con in out_cons:
                G.add_edge(NNEdge(from_node=split_params, from_idx=out_idx,
                                  to_node=out_con[0], to_idx=out_con[1]))
        if G.quantization:
            G.add_dimensions()
            quantizer = NewQuantizer.from_quantized_graph(G)
            quantizer.quantize()
            RemoveUnnecessaryQuantizeOperators().match(G)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        slices_by_origin = {}
        for slice_node in [node for node in G.nodes() if isinstance(node, StridedSliceParameters)]:
            in_edge = G.in_edges(slice_node.name)[0]
            group = slices_by_origin.setdefault(
                (in_edge.from_node, in_edge.from_idx), [])
            group.append(slice_node)
        for in_edge, slice_nodes in slices_by_origin.items():
            slices = list(zip(*[node.act_slice for node in slice_nodes]))
            if len(slice_nodes) == 1:
                self.slice_to_split(G, slice_nodes, slices)
                continue

            # strides must be one
            if any(sl[2] != 1 for sl_axis in slices for sl in sl_axis):
                continue

            diff_axes = list([idx for idx, elems in enumerate(
                slices) if not all(elems[0] == elem for elem in elems[1::])])
            not_diff_axes = [idx for idx in range(
                len(slices)) if idx not in diff_axes]
            diff_slices = [sl for idx, sl in enumerate(
                slices) if idx in diff_axes]
            axis_lengths = in_edge[0].out_dims[in_edge[1]].shape
            if not_diff_axes and min(not_diff_axes) < max(diff_axes):
                transpose_from = tuple(range(len(slices)))
                transpose_to = tuple(diff_axes + not_diff_axes)
                axis_lengths = [axis_lengths[idx] for idx in transpose_to]
            else:
                transpose_from = transpose_to = None
            diff_axis_lengths = axis_lengths[0:len(diff_axes):]

            diff_slices = combine_slices(
                diff_axis_lengths, diff_slices, slice_nodes)
            if diff_slices is None:
                continue

            if len(diff_axes) > 1:
                reshape_from = axis_lengths
                reshape_to = [np.prod(diff_axis_lengths)] + \
                    axis_lengths[len(diff_axes)::]
            else:
                reshape_from = None
                reshape_to = slice_nodes[0].in_dims[0].shape
                if transpose_from:
                    reshape_to = [reshape_to[idx] for idx in transpose_to]

            sizes, shapes, sorted_nodes = slices_to_sizes(
                diff_slices, axis_lengths[len(diff_axes)::])

            name_prefix = sorted_nodes[0].name

            in_edge = G.in_edges(sorted_nodes[0].name)[0]
            in_node = in_edge.from_node
            in_idx = in_edge.from_idx

            if transpose_from:
                params = TransposeParameters(
                    G.unique_name(name_prefix + '_tin'), transpose=transpose_to)
                G.add_edge(NNEdge(from_node=in_node,
                                  to_node=params, from_idx=in_idx))
                in_node = params
                in_idx = 0

            if reshape_from:
                params = ReshapeParameters(
                    G.unique_name(name_prefix + '_reshape'), old_shape=Dim.unnamed(reshape_from), shape=Dim.unnamed(reshape_to))
                G.add_edge(NNEdge(from_node=in_node,
                                  to_node=params, from_idx=in_idx))
                in_node = params
                in_idx = 0

            act_slices, out_shapes, axis = SplitParameters.get_splits(
                reshape_to, 0, splits=sizes)
            split_node = SplitParameters(
                G.unique_name(name_prefix + '_split'), act_slices=act_slices, out_shapes=out_shapes, axis=axis)

            G.add_edge(NNEdge(from_node=in_node, from_idx=in_idx,
                              to_node=split_node))

            sub_names = []
            for idx, node in enumerate(sorted_nodes):
                sub_names.append(node.name)
                out_edges = G.out_edges(node.name)
                G.remove(node)
                for out_edge in out_edges:
                    params = split_node
                    out_idx = idx
                    if reshape_from:
                        from_node = params
                        params = ReshapeParameters(
                            G.unique_name(name_prefix + f'_reshape{idx}'), shape=Dim.unnamed(shapes[idx]))
                        G.add_edge(NNEdge(from_node=from_node,
                                          to_node=params, from_idx=out_idx))
                        out_idx = 0
                    if transpose_from:
                        from_node = params
                        params = TransposeParameters(
                            G.unique_name(name_prefix + f'_tout{idx}'), transpose=reverse_transpose(transpose_to))
                        G.add_edge(NNEdge(from_node=from_node,
                                          to_node=params, from_idx=out_idx))
                        out_idx = 0

                    G.add_edge(NNEdge(from_node=params, to_node=out_edge.to_node,
                                      from_idx=out_idx, to_idx=out_edge.to_idx))
            if G.quantization:
                G.add_dimensions()
                quantizer = NewQuantizer.from_quantized_graph(G)
                quantizer.quantize()
                RemoveUnnecessaryQuantizeOperators().match(G)

            LOG.info(
                f'replaced slice nodes {",".join(sub_names)} with split node {split_node.name}')

            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
