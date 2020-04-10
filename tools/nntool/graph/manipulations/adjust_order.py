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

from ..types import (ConcatParameters, ConstantInputParameters,
                     Conv2DParameters, FcParameters, InputBaseParameters,
                     OutputParameters, ReshapeParameters,
                     UnconvertedOpParameters)
from .dimensions import add_dimensions
from .eliminate_transposes import eliminate_transposes

AT_CONVFILTER_ORD = ['out_c', 'in_c', 'h', 'w']
AT_FCFILTER_EXP_ORD = ['out_c', 'in_c', 'h', 'w']
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
            LOG.info("step %s: %s adjust %s %s %s => %s",
                     step_idx, node.name, direction, idx, dim, " x ".join(AT_ACTIVATION_ORD))
            dim.impose_order(AT_ACTIVATION_ORD)
            if hint and hint[idx]:
                hint[idx] = deepcopy(AT_ACTIVATION_ORD)
        elif len(dim) == 1:
            LOG.info("step %s: %s %s is one dimensional so no adjustment",
                     step_idx, node.name, direction)
        else:
            dim.move_last_to_first()


def adjust_order(G, reshape_weights=True):
    for step_idx, node, fusion_idx, _ in G.nodes_iterator():
        assert not fusion_idx, "order must be adjusted before fusing"
        if isinstance(node, InputBaseParameters):
            if node.fixed_order:
                node.transpose_out = node.last_first(node.dims)
                if node.out_dims_hint and node.out_dims_hint[0]:
                    node.out_dims_hint[0] = deepcopy(AT_ACTIVATION_ORD)
            else:
                if isinstance(node, ConstantInputParameters) and node.value is not None and reshape_weights:
                    node.value = maybe_transpose(node.dims, AT_ACTIVATION_ORD, node.value)
                adjust_dims(step_idx, node, node.out_dims, node.out_dims_hint, direction="output")
                node.dims = node.out_dims[0]
            continue
        elif isinstance(node, OutputParameters):
            if node.fixed_order:
                node.transpose_in = node.first_last(node.in_dims[0])
            else:
                adjust_dims(step_idx, node, node.in_dims, node.in_dims_hint, direction="input")
                node.dims = node.in_dims[0]
            continue
        elif isinstance(node, Conv2DParameters):
            LOG.info("step %s: %s adjust weights %s => %s",
                     step_idx, node.name, node.filter, " x ".join(AT_CONVFILTER_ORD))
            if node.weights is not None and reshape_weights:
                node.weights = maybe_transpose(node.filter, AT_CONVFILTER_ORD,
                                               node.weights)
            node.filter.impose_order(AT_CONVFILTER_ORD)
        elif isinstance(node, FcParameters):
            LOG.info("step %s: %s adjust weights %s => %s",
                     step_idx, node.name, node.filter, " x ".join(AT_FCFILTER_EXP_ORD))
            if node.weights is not None and reshape_weights:
                exp_weights = node.weights.reshape(node.filter.shape)
                node.weights = maybe_transpose(node.filter, AT_FCFILTER_EXP_ORD,
                                               exp_weights,
                                               reshape=(node.filter.out_c, node.filter.sz))

            node.filter.impose_order(AT_FCFILTER_EXP_ORD)
            node.in_dims[0].impose_order(AT_ACTIVATION_ORD)
            continue
        elif isinstance(node, ConcatParameters):
            if node.axis == len(node.in_dims[0].shape) - 1:
                node.axis = 0
            elif node.axis == 0:
                node.transpose_in = node.first_last(node.in_dims[0])
                node.transpose_out = node.last_first(node.out_dims[0])
            else:
                raise NotImplementedError("this needs to be implemented")
        elif isinstance(node, ReshapeParameters):
            in_dim = node.in_dims[0]
            out_dim = node.out_dims[0]
            if (in_dim.shape[-1] == out_dim.shape[-1] or node.does_nothing() or
                    (len(in_dim.shape) == 1 and out_dim.is_named and out_dim.c == 1)):
                node.shape.move_last_to_first()
                node.old_shape.move_last_to_first()
            else:
                node.transpose_in = node.first_last(node.in_dims[0])
                node.transpose_out = node.last_first(node.out_dims[0])
        elif isinstance(node, UnconvertedOpParameters):
            if node.indicated_outputs:
                for out in node.indicated_outputs:
                    out.move_last_to_first()

        adjust_dims(step_idx, node, node.in_dims, node.in_dims_hint)
        adjust_dims(step_idx, node, node.out_dims, node.out_dims_hint, direction="output")

    add_dimensions(G)
    eliminate_transposes(G)
    add_dimensions(G)
