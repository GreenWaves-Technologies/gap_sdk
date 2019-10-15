# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
from ..types import (Conv2DParameters, FcParameters, ConcatParameters,
                     ReshapeParameters, InputParameters, OutputParameters)

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

def adjust_order(G, reshape_weights=True):
    for step_idx, node, fusion_idx, _ in G.nodes_iterator():
        assert not fusion_idx, "order must be adjusted before fusing"
        if isinstance(node, Conv2DParameters):
            LOG.info("step %s: %s adjust weights %s => %s",\
                step_idx, node.name, node.filter, " x ".join(AT_CONVFILTER_ORD))
            if node.weights is not None and reshape_weights:
                node.weights = maybe_transpose(node.filter, AT_CONVFILTER_ORD,
                                               node.weights)
            node.filter.impose_order(AT_CONVFILTER_ORD)
        elif isinstance(node, FcParameters):
            LOG.info("step %s: %s adjust weights %s => %s",\
                    step_idx, node.name, node.filter, " x ".join(AT_FCFILTER_EXP_ORD))
            if node.weights is not None and reshape_weights:
                exp_weights = node.weights.reshape(node.filter.shape)
                node.weights = maybe_transpose(node.filter, AT_FCFILTER_EXP_ORD,
                                               exp_weights,
                                               reshape=(node.filter.out_c, node.filter.sz))

            node.filter.impose_order(AT_FCFILTER_EXP_ORD)
        elif isinstance(node, ConcatParameters):
            if node.axis != 'c':
                raise NotImplementedError("adjusting order of concat on non channel axis")
        elif isinstance(node, ReshapeParameters):
            raise NotImplementedError("this needs to be implemented")

        for in_idx, in_dim in enumerate(node.in_dims):
            LOG.info("step %s: %s adjust input %s %s => %s",\
                step_idx, node.name, in_idx, in_dim, " x ".join(AT_ACTIVATION_ORD))
            in_dim.impose_order(AT_ACTIVATION_ORD)
            if node.in_dims_hint and node.in_dims_hint[in_idx]:
                node.in_dims_hint[in_idx].impose_order(AT_ACTIVATION_ORD)
        for out_idx, out_dim in enumerate(node.out_dims):
            LOG.info("step %s: %s adjust output %s %s => %s",\
                step_idx, node.name, out_idx, out_dim, " x ".join(AT_ACTIVATION_ORD))
            out_dim.impose_order(AT_ACTIVATION_ORD)
            if node.out_dims_hint and node.out_dim_hint[out_idx]:
                node.out_dims_hint[out_idx].impose_order(AT_ACTIVATION_ORD)
        if isinstance(node, (InputParameters, OutputParameters)):
            node.dims = node.out_dims[0]
    