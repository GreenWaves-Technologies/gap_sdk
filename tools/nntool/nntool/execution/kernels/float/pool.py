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
from functools import reduce

import numpy as np
from nntool.graph.types import (AveragePoolNode, GlobalAveragePoolNode,
                         GlobalMaxPoolNode, GlobalMinPoolNode,
                         GlobalSumPoolNode, MaxPoolNode)
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import AllFloatQRec, QRec

LOG = logging.getLogger(__name__)

# pylint: disable=too-many-arguments, too-many-locals

@params_type(AveragePoolNode)
@qrec_type('float')
class AveragePoolingFloat(KernelBase):

    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        if qrec is None:
            qrec = AllFloatQRec()

        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dims, out_dims = params.in_dims[0], params.out_dims[0]
        filter_sz = params.filter_dim.h * params.filter_dim.w

        calc_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32

        pool_factor = np.array(1.0/filter_sz, dtype=calc_dtype)

        out_tensor = np.zeros(out_dims.shape, dtype=calc_dtype)

        if params.padding.h + params.padding.w > 0:
            in_tensor = np.pad(in_tensor,
                               params.padding.numpy_pad_shape(in_dims),
                               mode='constant',
                               constant_values=0.0)
            pad_w = params.padding.w
            pad_h = params.padding.h
        else:
            pad_w = pad_h = 0

        out_h = 0
        for h_idx in range(0, in_dims.h - params.filter_dim.h + pad_h + 1,
                           params.stride.h):
            out_w = 0
            for w_idx in range(0, in_dims.w - params.filter_dim.w + pad_w + 1,
                               params.stride.w):
                # accumulate - potentially with different Q
                out_slice_args = out_dims.srange(h=out_h, w=out_w)
                in_slice_args = in_dims.srange(c=[0, out_dims.c, 1],
                                               h=[h_idx, h_idx +
                                                   params.filter_dim.h, 1],
                                               w=[w_idx, w_idx + params.filter_dim.w, 1])

                res_shape = out_tensor[out_slice_args].shape
                sum_filter = np.sum(in_tensor[in_slice_args], dtype=calc_dtype,
                                    axis=(out_dims.keys.index('h'), out_dims.keys.index('w'))).reshape(res_shape)
                sum_filter = np.multiply(sum_filter, pool_factor)
                out_tensor[out_slice_args] = sum_filter
                out_w += 1
            out_h += 1

        return qrec.get_outputs(params, [out_tensor], ktype="float")


@params_type(MaxPoolNode)
@qrec_type('float')
class MaxPoolingFloat(KernelBase):

    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()

        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dims, out_dims = params.in_dims[0], params.out_dims[0]

        calc_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        out_tensor = np.zeros(out_dims.shape, dtype=calc_dtype)

        if params.padding.h + params.padding.w > 0:
            in_tensor = np.pad(in_tensor,
                               params.padding.numpy_pad_shape(in_dims),
                               mode='constant',
                               constant_values=0.0)
            pad_w = params.padding.w
            pad_h = params.padding.h
        else:
            pad_w = pad_h = 0

        out_h = 0
        for h_idx in range(0, in_dims.h - params.filter_dim.h + pad_h + 1,
                           params.stride.h):
            out_w = 0
            for w_idx in range(0, in_dims.w - params.filter_dim.w + pad_w + 1,
                               params.stride.w):
                # accumulate - potentially with different Q
                out_slice_args = out_dims.srange(h=out_h, w=out_w)
                in_slice_args = in_dims.srange(c=[0, out_dims.c, 1],
                                               h=[h_idx, h_idx +
                                                   params.filter_dim.h, 1],
                                               w=[w_idx, w_idx + params.filter_dim.w, 1])

                res_shape = out_tensor[out_slice_args].shape
                out_tensor[out_slice_args] = np.max(in_tensor[in_slice_args].view(np.ndarray),
                                                    axis=(out_dims.keys.index('h'), out_dims.keys.index('w'))).reshape(res_shape)
                out_w += 1
            out_h += 1

        return qrec.get_outputs(params, [out_tensor], ktype="float")


@params_type(GlobalAveragePoolNode)
@qrec_type('float')
class GlobalAveragePoolFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]

        calc_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        sum_by_chan = np.sum(in_tensor, dtype=calc_dtype, axis=tuple(
            params.axis), keepdims=params.keep_dims)
        sz = reduce(lambda x, y: x * y, [i for idx,
                                         i in enumerate(in_tensor.shape) if idx in params.axis])

        return qrec.get_outputs(params,
                                [(sum_by_chan / sz).reshape(params.out_dims[0].shape).astype(qrec.out_qs[0].dtype)],
                                ktype="float")

@params_type(GlobalMaxPoolNode)
@qrec_type('float')
class GlobalMaxPoolFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]

        return qrec.get_outputs(params, [np.max(in_tensor,
                                                axis=tuple(params.axis),
                                                keepdims=params.keep_dims)], ktype="float")

@params_type(GlobalMinPoolNode)
@qrec_type('float')
class GlobalMinPoolFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]

        return qrec.get_outputs(params, [np.min(in_tensor,
                                                axis=tuple(params.axis),
                                                keepdims=params.keep_dims)], ktype="float")

@params_type(GlobalSumPoolNode)
@qrec_type('float')
class GlobalSumPoolFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]

        return qrec.get_outputs(params, [np.sum(in_tensor,
                                                axis=tuple(params.axis),
                                                keepdims=params.keep_dims)], ktype="float")
