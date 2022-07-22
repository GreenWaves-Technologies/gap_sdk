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
# from numba import njit, prange
from nntool.graph.types import Conv2DNode
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.multiplicative.mulbias import apply_multiplicative_bias
from nntool.quantization.new_qrec import AllFloatQRec, QRec
from nntool.utils.numpy_fast_conv import fast_numpy_conv

# pylint: disable=invalid-name

LOG = logging.getLogger(__name__)


@params_type(Conv2DNode)
@qrec_type('float')
class Conv2DFloat32(KernelBase):
    @classmethod
    def execute(cls, params: Conv2DNode,
                in_tensors,
                qrec: QRec,
                **kwargs):
        '''3D convolution by sub-matrix summing.
        '''
        details = kwargs.get('details')
        if qrec is None:
            qrec = AllFloatQRec()
        in_dim, w_dim, out_dim = params.in_dims[0], params.filter_dim, params.out_dims[0]
        prepared_in_tensors = qrec.prepare_inputs(
            params, in_tensors, ktype="float")
        in_tensor = prepared_in_tensors[0]
        weights = prepared_in_tensors[1]
        biases = prepared_in_tensors[2]

        if details is not None:
            details['min_acc'] = float("Infinity")
            details['max_acc'] = float("-Infinity")
            details['min_pre_mul_bias'] = float("Infinity")
            details['max_pre_mul_bias'] = float("-Infinity")
        in_rank = len(in_tensor.shape)
        if in_rank == 3:
            batch = None
            internal_in_order = ['h', 'w', 'c']
            internal_out_order = ['c', 'h', 'w']
        elif in_rank == 4:
            batch = in_tensor.shape[in_dim.get_order_idx('n')]
            internal_in_order = ['n', 'h', 'w', 'c']
            internal_out_order = ['n', 'c', 'h', 'w']
        else:
            raise NotImplementedError(
                f'{params.name} input has input rank of {in_rank} shape {in_tensor.shape} '
                'which is not supported by nntool kernels')

        in_tensor = in_tensor.transpose(
            in_dim.transpose_to_order(internal_in_order))

        in_tensor, pad_w, pad_h = cls.pad_input(params, in_tensor, batch)

        filt_w = w_dim.w
        filt_h = w_dim.h
        filt_dil_h = params.dilation.h
        filt_dil_w = params.dilation.w
        filt_str_h = params.stride.h
        filt_str_w = params.stride.w

        in_h = in_dim.h
        in_w = in_dim.w
        in_c = in_dim.c
        out_h = out_dim.h
        out_w = out_dim.w
        out_c = w_dim.out_c

        dilated_filter_w = (filt_dil_w - 1) * (filt_w - 1) + filt_w
        dilated_filter_h = (filt_dil_h - 1) * (filt_h - 1) + filt_h

        const_h = pad_h + in_h - dilated_filter_h + 1
        const_w = pad_w + in_w - dilated_filter_w + 1

        calc_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32

        if params.has_bias:
            # biases = qrec.prepare_biases(params, params.get_uncompressed_biases(),
            #                              params.get_uncompressed_weights(), ktype="float")
            biases = np.broadcast_to(biases.reshape(
                out_c, 1, 1), (out_c, out_h, out_w)).copy().astype(calc_dtype)
        else:
            biases = np.zeros((out_c, out_h, out_w),
                              dtype=calc_dtype)

        if details is not None:
            min_acc, max_acc = details['min_acc'], details['max_acc']
        else:
            min_acc, max_acc = qrec.out_qs[0].max, qrec.out_qs[0].min

        weights = weights.transpose(w_dim.transpose_to_order(
            ['h', 'w', 'in_c', 'out_c']))

        cur_batch = 0
        result = None
        while True:
            if batch is None:
                cur_in_tensor = in_tensor
            else:
                cur_in_tensor = in_tensor[cur_batch]
            cur_result, min_acc_tmp, max_acc_tmp = fast_numpy_conv(
                cur_in_tensor,
                weights,
                biases,
                in_c,
                out_h,
                out_w,
                out_c,
                dilated_filter_h,
                dilated_filter_w,
                filt_h,
                filt_w,
                filt_dil_h,
                filt_dil_w,
                filt_str_h,
                filt_str_w,
                const_h,
                const_w,
                params.groups,
                params.is_depthwise_conv(),
                calc_dtype
            )
            if details is not None:
                min_acc, max_acc = min(min_acc_tmp, max_acc_tmp), max(
                    max_acc_tmp, max_acc)
            if details is not None:
                details['min_acc'], details['max_acc'] = min_acc, max_acc
                details['min_pre_mul_bias'] = min(
                    np.min(cur_result), details['min_pre_mul_bias'])
                details['max_pre_mul_bias'] = max(
                    np.max(cur_result), details['max_pre_mul_bias'])

            cur_result = apply_multiplicative_bias(qrec,
                                                   params, cur_result, axis=0, ktype="float")
            if batch is None:
                result = cur_result
                break
            if result is None:
                result = np.expand_dims(cur_result, 0)
            else:
                result = np.concatenate(
                    [result, np.expand_dims(cur_result, 0)], axis=0)
            cur_batch += 1
            if cur_batch >= batch:
                break

        result = result.transpose(
            out_dim.transpose_from_order(internal_out_order))

        return qrec.get_outputs(params, [result], ktype="float")

    @classmethod
    def pad_input(cls, params, in_tensor, batch):
        if (params.padding.h + params.padding.w) > 0:
            padding = [[params.padding.t,
                        params.padding.b],
                       [params.padding.l,
                        params.padding.r]]
            if batch is not None:
                padding = [[0, 0]] + padding
            in_tensor = np.pad(
                in_tensor,
                padding + [[0, 0], ] * (np.ndim(in_tensor)-len(padding)),
                mode='constant',
                constant_values=0.0)
            pad_w = params.padding.w
            pad_h = params.padding.h
        else:
            pad_w = pad_h = 0
        return in_tensor, pad_w, pad_h
