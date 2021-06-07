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
from graph.types import FcParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.multiplicative.mulbias import apply_multiplicative_bias
from quantization.new_qrec import AllFloatQRec, QRec

LOG = logging.getLogger("nntool." + __name__)


@params_type(FcParameters)
@qrec_type('float')
class LinearFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        details = kwargs.get('details')
        if qrec is None:
            qrec = AllFloatQRec()

        in_dims, out_dims = tuple(dims[0] for dims in cls.calc_transposed_dims(params))

        prepared_in_tensors = qrec.prepare_inputs(
            params, in_tensors, ktype="float")

        in_tensor = prepared_in_tensors[0]
        weights = prepared_in_tensors[1]
        biases = prepared_in_tensors[2]

        if details is not None:
            details['min_acc'] = float("Infinity")
            details['max_acc'] = float("-Infinity")

        calc_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32

        if params.has_bias:
            acc_tensor = np.ones(out_dims.shape, dtype=calc_dtype) * biases
        else:
            acc_tensor = np.zeros(out_dims.shape,
                                  dtype=calc_dtype)
        if params.batch_size > 1:
            in_tensor = in_tensor.reshape(
                (params.batch_size, in_dims.size()//params.batch_size))
            # weights will already be transposed at import
            acc_tensor += np.dot(in_tensor, weights)
            details['min_acc'] = np.min(acc_tensor)
            details['max_acc'] = np.max(acc_tensor)
            acc_tensor = apply_multiplicative_bias(qrec,
                                                   params, acc_tensor, 1, ktype="float")
            if params.batch_minor:
                acc_tensor = acc_tensor.transpose(1, 0)
        else:
            in_tensor = in_tensor.reshape((in_dims.size()))
            filt = params.filter.get_filter_dims()

            for out_c in range(out_dims.c):
                # Expand and normalize the accumulator

                w_slice = weights[filt.srange(out_c=out_c)].reshape(
                    (in_dims.size()))

                res = np.dot(in_tensor, w_slice)

                if details is not None:
                    details['min_acc'] = min(
                        np.sum(res[res < 0]), details['min_acc'])
                    details['max_acc'] = min(
                        np.sum(res[res > 0]), details['max_acc'])

                acc_tensor[out_c] += res

                if details is not None:
                    details['min_acc'] = min(
                        np.min(acc_tensor[out_c]), details['min_acc'])
                    details['max_acc'] = max(
                        np.max(acc_tensor[out_c]), details['max_acc'])

            acc_tensor = apply_multiplicative_bias(qrec,
                                                   params, acc_tensor, 0, ktype="float")

        return qrec.get_outputs(params, [acc_tensor], ktype="float")
