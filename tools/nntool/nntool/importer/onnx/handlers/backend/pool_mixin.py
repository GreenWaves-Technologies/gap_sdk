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

import numpy as np
from nntool.graph.dim import Dim, PoolFilterDim, StrideDim
from nntool.graph.types import GlobalPoolingNodeBase, PoolingNodeBase
from nntool.graph.types.base import NNEdge
from nntool.graph.types.misc import NoOPNode
from nntool.graph.types.tensor_shape import ReshapeNode
from nntool.importer.onnx.common import logger
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.common.check_batchdim import check_batchdim

from .pad_mixin import PadMixin


class PoolMixin(PadMixin):

    @classmethod
    def pool(cls, node, pool_type=None, copy_qtype=False, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x = check_batchdim(G, x, valid_name)
        x_shape = x[2].shape

        if x_shape[0] is not None:
            real_in_shape = tuple(x_shape.copy())
            if x_shape[0] > 1:
                # support for multi batch is very limited
                batch = x_shape[0]
                logger.warning(
                    f"{valid_name} has a non 1 batch dimension of {batch} -"
                    " this is not supported by nntool or autotiler kernels")
            else:
                # if the batch is specified but is 1 then the input will be reshaped
                # and the output will have the batch dim set as unknown.
                batch = None
        else:
            real_in_shape = tuple(x_shape[1:])
            batch = None

        kernel_shape = node.attrs["kernel_shape"]
        kernel_rank = len(kernel_shape)
        if kernel_rank > 2:
            raise NotImplementedError(f'{pool_type} pool {valid_name} is a {kernel_rank}D pool '
                                      'which is not supported by NNTOOL')
        if len(real_in_shape) > 2:
            x_feature_shape = x_shape[2::]
            in_c = x_shape[1]
        else:
            x_feature_shape = x_shape[1::]
            in_c = 1

        input_rank = len(x_feature_shape)
        if input_rank != kernel_rank:
            raise ValueError(f'error in ONNX graph. {pool_type} pool {valid_name} '
                             f'has a different input spatial rank {input_rank} to kernel rank {kernel_rank}')
        spatial_size = kernel_rank

        strides = node.attrs.get("strides", [1] * spatial_size)
        stride_is_one = all(stride == 1 for stride in strides)
        dilations = node.attrs.get("dilations", [1] * spatial_size)
        if any(dilation > 1 for dilation in dilations):
            raise ValueError(valid_name + " with dilation not supported")
        # ceil_mode = bool(node.attrs.get("ceil_mode", 0))
        pad_dim = cls.calc_pad_dim(node, 2)

        if spatial_size == 1:
            strides = [1] + strides
            dilations = [1] + dilations
            kernel_shape = [1] + kernel_shape
            h = 1
            w = x_shape[2]
            x_feature_shape = [1] + x_feature_shape
        else:
            h = x_shape[-2]
            w = x_shape[-1]

        stride_is_one = all(stride == 1 for stride in strides)
        if any(dilation > 1 for dilation in dilations):
            raise ValueError(valid_name + " with dilation not supported")
        # ceil_mode = bool(node.attrs.get("ceil_mode", 0))
        pad_dim = cls.calc_pad_dim(node, spatial_size)
        if pad_dim.is_same:
            pad_dim.calculate_same(
                Dim.named_ordered(h=h, w=w),
                PoolFilterDim(kernel_shape[0], kernel_shape[1]),
                StrideDim(strides[0], strides[1]))
        # Note: This needs to check dilation if it is added
        filter_matches_input = (all(k_dim >= (x_dim + pad) for k_dim, x_dim, pad
                                    in zip(kernel_shape, x_feature_shape, [pad_dim.h, pad_dim.w])))

        if filter_matches_input and stride_is_one:
            if all(k == 1 for k in kernel_shape):
                params = NoOPNode(valid_name, desc="GlobalPool of input shape 1x1")
            else:
                params = GlobalPoolingNodeBase(valid_name,
                                                pool_type=pool_type,
                                                axis=[1, 2],
                                                keep_dims=True,
                                                in_dims_hint=[['c', 'h', 'w']],
                                                out_dims_hint=[['c', 'h', 'w']])
        else:
            params = PoolingNodeBase(valid_name,
                                       filter_dim=PoolFilterDim(kernel_shape[0],
                                                          kernel_shape[1]),
                                       stride=StrideDim(strides[0],
                                                        strides[1]),
                                       padding=pad_dim,
                                       pool_type=pool_type,
                                       in_dims_hint=[['c', 'h', 'w']],
                                       out_dims_hint=[['c', 'h', 'w']])

        pool_in_shape = (in_c, h, w)
        if pool_in_shape != real_in_shape:
            # if batch is present add it back
            if batch is not None:
                pool_in_shape = (batch,) + pool_in_shape
            if np.prod(real_in_shape) != np.prod(pool_in_shape):
                raise ValueError(
                    f'shape inference issue {valid_name} filter indicates {pool_in_shape} but has an input of {real_in_shape}')
            r1_params = ReshapeNode(f'{valid_name}_reshape_in',
                                    old_shape=Dim.unnamed(real_in_shape),
                                    shape=Dim.unnamed(pool_in_shape))
            G.add_edge(
                NNEdge(from_node=x[0], to_node=r1_params, from_idx=x[1], to_idx=0))
            G.add_edge(NNEdge(from_node=r1_params,
                              to_node=params, from_idx=0, to_idx=0))
        else:
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        in_dim = Dim.named_ordered(c=in_c, h=h, w=w)
        out_dims = params.get_output_size([in_dim])
        # check if output needs a reshape
        if spatial_size == 1:
            if batch is not None:
                oned_out_shape = [batch, out_dims[0].c, out_dims[0].w]
                pout_dims = ProvisionalDim(oned_out_shape)
            else:
                oned_out_shape = [out_dims[0].c, out_dims[0].w]
                pout_dims = ProvisionalDim([None] + oned_out_shape)

            r2_params = ReshapeNode(f'{valid_name}_reshape_out',
                                    old_shape=out_dims[0],
                                    shape=Dim.unnamed(oned_out_shape))
            G.add_edge(NNEdge(from_node=params,
                              to_node=r2_params, from_idx=0, to_idx=0))
            params = r2_params
        else:
            pout_dims = ProvisionalDim([batch] + out_dims[0].shape)

        all_nodes[node.output[0]] = (
            params, 0, pout_dims, x[3] if copy_qtype else None)
        return params
