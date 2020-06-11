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

from quantization.quantization_record_base import QuantizationRecordBase
from quantization.multiplicative.mult_quantization import MultQuantizationRecordBase
from utils.exp_17_15 import exp_fp_17_15


def softmax_func(v):
    max_v = np.max(v)
    v = v - max_v
    return np.exp(v)/np.sum(np.exp(v))


def softmax(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    if isinstance(qrec, MultQuantizationRecordBase):
        return softmax_sq8(params, in_tensors, qrec, details=details)

    np.seterr(over='raise')
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    # TODO - Implement properly quantized version
    in_tensor = qrec.in_qs[0].dequantize(in_tensor)
    return qrec.get_outputs(params, [qrec.out_qs[0].quantize(softmax_func(in_tensor))], ktype="symmetric")


# void KerParSoftMax_SQ8(KerSoftMax_SQ8_T *Arg)
def softmax_sq8(params,
                in_tensors,
                qrec: QuantizationRecordBase,
                details=None):
    del details
    in_tensor = in_tensors[0].flatten()
    max_val = np.max(in_tensor)
    norm = 15 + np.ceil(np.log2(qrec.in_qs[0].scale)).astype(np.int32)
    exp = exp_fp_17_15((in_tensor.astype(np.int32) - max_val) << (norm))
    sum_exp = np.sum(exp)
    inv_sum = (np.array([(1 << 15)-1], dtype=np.uint32) << 15)//sum_exp
    res = np.abs((exp * inv_sum + (1 << 14)) >> 15)
    iinfo = np.iinfo(np.int16)
    res = np.clip(res, iinfo.min, iinfo.max).astype(np.int16).reshape(params.out_dims[0].shape)
    return qrec.get_outputs(params, [res], ktype="symmetric")
