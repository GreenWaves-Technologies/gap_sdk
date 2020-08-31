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

from ..types import (ConcatParameters, ConstantInputParameters,
                     Conv2DParameters, FcParameters, ImageFormatParameters,
                     InputBaseParameters, OutputParameters, ReshapeParameters,
                     GlobalPoolParameters, RNNBaseParameters, SSDDetectorParameters,
                     StridedSliceParameters, UnconvertedOpParameters,
                     SplitParameters)
from .dimensions import add_dimensions
from .eliminate_transposes import eliminate_transposes

AT_CONVFILTER_ORD = ['out_c', 'in_c', 'h', 'w']
AT_FCFILTER_EXP_ORD = ['out_c', 'in_c']
AT_FCFILTER_ORD = ['out_c', 'sz']
AT_ACTIVATION_ORD = ['c', 'h', 'w']

LOG = logging.getLogger("nntool." + __name__)


def maybe_transpose(cur, desired_order, tensor, reshape=None):
    if cur.order != desired_order:
        tensor = tensor.transpose(cur.transpose_to_order(desired_order))
    if reshape is not None:
        tensor = tensor.reshape(reshape)
    return tensor


def adjust_dims(step_idx, node, dims, hint, direction="input"):
    for idx, dim in enumerate(dims):
        if dim.just_has_keys(AT_ACTIVATION_ORD):
            LOG.debug("step %s: %s adjust %s %s %s => %s",
                      step_idx, node.name, direction, idx, dim, " x ".join(AT_ACTIVATION_ORD))
            dim.impose_order(AT_ACTIVATION_ORD)
            if hint and hint[idx]:
                hint[idx] = deepcopy(AT_ACTIVATION_ORD)
        elif len(dim) == 1:
            LOG.debug("step %s: %s %s is one dimensional so no adjustment",
                      step_idx, node.name, direction)
        else:
            dim.move_last_to_first()


def adjust_order(G, reshape_weights=True, postprocess=True):
    for step_idx, node, fusion_idx, _ in G.nodes_iterator():
        assert not fusion_idx, "order must be adjusted before fusing"
        if isinstance(node, InputBaseParameters):
            if node.fixed_order:
                # Check if followed by an ImageFormat node in which case reordering will
                # happen there and there should be no transform on the input
                if not isinstance(G.out_edges(node.name)[0].to_node, ImageFormatParameters):
                    node.transpose_out = [node.last_first(node.dims)]
                    if node.out_dims_hint and node.out_dims_hint[0]:
                        if len(node.out_dims_hint[0]) == 3:
                            node.out_dims_hint[0] = deepcopy(AT_ACTIVATION_ORD)
                        else:
                            node.out_dims_hint[0] = node.out_dims_hint[0][-1::] + \
                                node.out_dims_hint[0][:-1:]

            else:
                if isinstance(node, ConstantInputParameters):
                    if node.adjust_transpose:
                        if node.value is not None and reshape_weights:
                            node.value = np.transpose(node.value, node.adjust_transpose)
                        node.dims.transpose(node.adjust_transpose)
                        continue
                    elif node.adjust_transpose != False:
                        if node.value is not None and reshape_weights:
                            node.value = maybe_transpose(node.dims, AT_ACTIVATION_ORD, node.value)
                    else:
                        continue
                adjust_dims(step_idx, node, node.out_dims, node.out_dims_hint, direction="output")
                node.dims = node.out_dims[0]
            continue
        elif isinstance(node, OutputParameters):
            if node.fixed_order:
                if len(node.in_dims[0].shape) > 1:
                    node.transpose_in = [node.first_last(node.in_dims[0])]
            else:
                adjust_dims(step_idx, node, node.in_dims, node.in_dims_hint, direction="input")
                node.dims = node.in_dims[0]
            continue
        elif isinstance(node, Conv2DParameters):
            LOG.debug("step %s: %s adjust weights %s => %s",
                      step_idx, node.name, node.filter, " x ".join(AT_CONVFILTER_ORD))
            if node.weights is not None and reshape_weights:
                node.weights = maybe_transpose(node.filter, AT_CONVFILTER_ORD,
                                               node.weights)
            node.filter.impose_order(AT_CONVFILTER_ORD)
        elif isinstance(node, FcParameters):
            LOG.debug("step %s: %s adjust weights %s => %s",
                      step_idx, node.name, node.filter, " x ".join(AT_FCFILTER_EXP_ORD))
            desired_order = AT_FCFILTER_EXP_ORD + [k for k in node.filter.keys
                                                   if k not in AT_FCFILTER_EXP_ORD]
            if node.weights is not None and reshape_weights:
                exp_weights = node.weights.reshape(node.filter.shape)
                node.weights = maybe_transpose(node.filter,
                                               desired_order,
                                               exp_weights,
                                               reshape=(node.filter.out_c, node.filter.sz))

            node.filter.impose_order(desired_order)
            # node.in_dims[0].impose_order(AT_ACTIVATION_ORD)
            continue
        elif isinstance(node, StridedSliceParameters):
            # reorder the slice parameters to bring the channel dim first
            node.act_slice = [node.act_slice[-1]] + node.act_slice[:-1:]
            node.out_shape = [node.out_shape[-1]] + list(node.out_shape)[:-1:]
        elif isinstance(node, SplitParameters):
            if node.axis != 0:
                # reorganize all the splits to be axis = 0
                transpose_to_shape = [node.axis] + \
                    [i for i in range(len(node.in_dims[0])) if i != node.axis]
                node.transpose_params(transpose_to_shape)

            if node.axis != len(node.in_dims[0]) - 1:
                # real axis will be one more since last axis will move to first
                node.axis += 1
                trans_length = len(node.in_dims[0])
                # move split axis first
                node.transpose_in = [[node.axis] + [i for i in range(trans_length)
                                                    if i != node.axis]]
                # move split axis back into original position
                node.transpose_out = [node.transpose_in[0].copy() if len(
                    dim) > 1 else None for dim in node.out_dims]
            # axis is 0 in all cases
            node.axis = 0
        elif isinstance(node, GlobalPoolParameters):
            # make sure that node.axis axes are at the RHS of the tensor
            in_dim_len = len(node.in_dims[0].shape)
            # move them to the position they will be at after the adjust
            axis = [0 if i == in_dim_len - 1 else i + 1 for i in node.axis]
            # how many dimensions of the tensor do we keep after reduction
            dim_keep = in_dim_len - len(axis)
            # check that none of the reduction dimensions are in the first dim_keep
            # dimensions
            if not all(red_axis not in range(dim_keep) for red_axis in axis):
                # we need a transpose [all not in axis] + axis
                transpose = [i for i in range(in_dim_len) if i not in axis] + axis
                node.transpose_in = [transpose]
                # if we keep dimensions then we need to transpose on output
                # this will be reduced to a reshape by eliminate transposes so no code
                # will be generated. But dimension calculation will fail without it.
                if node.keep_dims:
                    node.transpose_out = [transpose]
                # move axis onto new home
                axis = [transpose.index(i) for i in axis]
            node.axis = axis
        elif isinstance(node, ConcatParameters):
            # if axis is last it will become first so this concat is valid without change
            if node.axis != len(node.out_dims[0]) - 1:
                # real axis will be one more since last axis will move to first
                node.axis += 1
                trans_length = len(node.out_dims[0])
                # move concat axis first
                node.transpose_in = [[node.axis] + [i for i in range(trans_length)
                                                    if i != node.axis] for _ in range(len(node.in_dims))]
                # move concat axis back into original position
                node.transpose_out = [node.transpose_in[0].copy()]
            # axis is 0 in all cases
            node.axis = 0
        elif isinstance(node, RNNBaseParameters):
            node.transpose_in = [[1, 0]]
            node.transpose_out = [[1, 0]]
            adjust_dims(step_idx, node, [node.in_dims[0]], node.in_dims_hint)
            adjust_dims(step_idx, node, [node.out_dims[0]], node.out_dims_hint, direction="output")
            continue
        elif isinstance(node, SSDDetectorParameters):
            node.transpose_in = [[1, 0], [1, 0], None]
            adjust_dims(step_idx, node, node.in_dims, node.in_dims_hint)
            continue
        elif isinstance(node, ReshapeParameters):
            in_dim = node.in_dims[0]
            out_dim = node.out_dims[0]
            if in_dim.layout_shape != out_dim.layout_shape:
                # These two tests look at whether the last dimension is 1
                # in which case moving it first does not change the reshape
                # or whether the layout_shape (shape with all 1 dimensions removed)
                # has a single dimension in which case the reshape will also not
                # change with the axis move
                if in_dim.shape[-1] != 1 and len(in_dim.layout_shape) > 1:
                    node.transpose_in = [node.first_last(in_dim)]
                if out_dim.shape[-1] != 1 and len(out_dim.layout_shape) > 1:
                    node.transpose_out = [node.last_first(out_dim)]
            else:
                node.old_shape.move_last_to_first()
                node.shape.move_last_to_first()
        elif isinstance(node, UnconvertedOpParameters):
            if node.indicated_outputs:
                for out in node.indicated_outputs:
                    out.move_last_to_first()

        adjust_dims(step_idx, node, node.in_dims, node.in_dims_hint)
        adjust_dims(step_idx, node, node.out_dims, node.out_dims_hint, direction="output")

    add_dimensions(G)
    if postprocess:
        eliminate_transposes(G)
        add_dimensions(G)
