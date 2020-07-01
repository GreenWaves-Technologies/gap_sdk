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

from quantization.quantization_record_base import ScalableFilterQuantizationRecordBase
from quantization.float32.float32_quantization import Float32ScalableFilterQuantizationRecord

LOG = logging.getLogger("nntool." + __name__)


def linear(params,
           in_tensors,
           qrec: ScalableFilterQuantizationRecordBase,
           details=None):

    if qrec is None:
        qrec = Float32ScalableFilterQuantizationRecord()

    in_dims = params.in_dims[0]
    out_dims = params.out_dims[0]
    weights = qrec.prepare_weights(params, params.weights, ktype="float32")
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]

    if details is not None:
        details['min_acc'] = float("Infinity")
        details['max_acc'] = float("-Infinity")

    if params.has_bias:
        biases = qrec.prepare_biases(params, params.biases, params.weights, ktype="float32")
        acc_tensor = np.ones(out_dims.shape, dtype=np.float32) * biases
    else:
        acc_tensor = np.zeros(out_dims.shape,
                              dtype=np.float32)

    in_tensor = in_tensor.reshape((in_dims.size()))
    filt = params.filter.get_filter_dims()
    for out_c in range(out_dims.c):
        # Expand and normalize the accumulator

        w_slice = weights[filt.srange(out_c=out_c)].reshape((in_dims.size()))

        res = np.dot(in_tensor, w_slice)

        if details is not None:
            details['min_acc'] = min(np.sum(res[res < 0]), details['min_acc'])
            details['max_acc'] = min(np.sum(res[res > 0]), details['max_acc'])

        acc_tensor[out_c] += res

        if details is not None:
            details['min_acc'] = min(np.min(acc_tensor[out_c]), details['min_acc'])
            details['max_acc'] = max(np.max(acc_tensor[out_c]), details['max_acc'])

    acc_tensor = qrec.apply_multiplicative_bias(
        params, acc_tensor, 0, ktype="float32")

    return qrec.get_outputs(params, [acc_tensor], ktype="float32")
