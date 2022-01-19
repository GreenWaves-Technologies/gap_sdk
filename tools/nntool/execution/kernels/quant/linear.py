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
from graph.types.linear import FcParameters
from execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.multiplicative.mulbias import (apply_multiplicative_bias,
                                                 apply_zero_offset_bias)
from quantization.new_qrec import QRec

LOG = logging.getLogger("nntool." + __name__)


@params_type(FcParameters)
@qrec_type('symmetric', 'scaled', 'scaled_ne16')
class LinearSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        details = kwargs.get('details')

        in_dims, out_dims = params.in_dims[0], params.out_dims[0]
        prepared_in_tensors = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")
        prepared_in_tensors = apply_zero_offset_bias(
            qrec, params, prepared_in_tensors, ktype="symmetric")
        in_tensor = prepared_in_tensors[0]
        # expand the weights to apply the zero offset
        weights = prepared_in_tensors[1].astype(np.int32) - qrec.in_qs[1].zero_point.astype(np.int32)
        biases = prepared_in_tensors[2]

        if details is not None:
            details['min_acc'] = float("Infinity")
            details['max_acc'] = float("-Infinity")

        acc_q = qrec.cache.get('acc_q') or qrec.in_qs[2]
        calc_q = qrec.cache.get('calc_q') or qrec.in_qs[2]
        if params.has_bias:
            # move biases to accumulator dtype and Q
            acc_tensor = biases.copy().astype(acc_q.dtype)
            if acc_q != qrec.in_qs[2]:
                acc_tensor = acc_q.expand_from(acc_tensor, qrec.in_qs[2])
        else:
            acc_tensor = np.zeros(out_dims.shape,
                                  dtype=acc_q.dtype)

        if params.batch_size > 1:
            in_tensor = in_tensor.reshape(
                (params.batch_size, in_dims.size()//params.batch_size)).astype(calc_q.dtype)
            acc_tensor = calc_q.expand_from(acc_tensor, acc_q)
            # weights will already be transposed at import
            acc_tensor = np.dot(in_tensor, weights.astype(calc_q.dtype)) + acc_tensor
            details['min_acc'] = np.min(acc_tensor)
            details['max_acc'] = np.max(acc_tensor)
            acc_tensor = acc_q.reduce_from(acc_tensor, calc_q)
            acc_tensor = apply_multiplicative_bias(qrec,
                                                   params, acc_tensor, 1, ktype="symmetric")
            if params.batch_minor:
                acc_tensor = acc_tensor.transpose(1, 0)
        else:
            # force the bit dimension of the input tensor to the bit width of the calc
            # so that the dot product occurs in this precision
            in_tensor = in_tensor.astype(calc_q.dtype)

            in_tensor = in_tensor.reshape((in_dims.size()))
            filt = params.filter.get_filter_dims()
            for out_c in range(out_dims.c):
                # Expand and normalize the accumulator
                if calc_q != acc_q:
                    acc_tensor = calc_q.expand_from(acc_tensor, acc_q)

                w_slice = weights[filt.srange(out_c=out_c)].reshape(
                    (in_dims.size()))

                res = np.dot(in_tensor, w_slice)

                if details is not None:
                    details['min_acc'] = min(
                        np.sum(res[res < 0]), details['min_acc'])
                    details['max_acc'] = min(
                        np.sum(res[res > 0]), details['max_acc'])

                acc_tensor[out_c] += res

                if calc_q != acc_q:
                    acc_tensor = acc_q.reduce_from(acc_tensor, calc_q)

                if details is not None:
                    details['min_acc'] = min(
                        np.min(acc_tensor[out_c]), details['min_acc'])
                    details['max_acc'] = max(
                        np.max(acc_tensor[out_c]), details['max_acc'])

            # details['acc_before'] = acc_tensor.copy()
            acc_tensor = apply_multiplicative_bias(qrec,
                                                params, acc_tensor, 0, ktype="symmetric")
            # details['acc_after'] = acc_tensor.copy()

        out_q = qrec.out_qs[0]

        if qrec and out_q != acc_q:
            acc_tensor = out_q.reduce_from(acc_tensor, acc_q, allow_zero_adjust=True)

        return qrec.get_outputs(params, [acc_tensor], ktype="symmetric")
