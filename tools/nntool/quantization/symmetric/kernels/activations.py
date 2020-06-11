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

from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.qtype import QType
from quantization.quantization_record_base import QuantizationRecordBase
from utils.at_norm import at_norm

FORCE_RELU = False

def leaky(params,
          in_tensors,
          qrec: QuantizationRecordBase,
          details=None):
    raise NotImplementedError()


def sigmoid(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    del details
    if isinstance(qrec, MultQuantizationRecord):
        raise NotImplementedError()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    dqinput = qrec.in_qs[0].dequantize(in_tensor)
    return qrec.get_outputs(params, [qrec.out_qs[0].quantize(1/(1 + np.exp(-dqinput)))], ktype="symmetric")


def relu_mult(params,
              in_tensors,
              qrec: QuantizationRecordBase,
              details=None):
    del details

    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    qrec.set_scale()
    relu_lb = qrec.in_qs[0].quantize(params.lower_bound)
    in_tensor = np.maximum(in_tensor, relu_lb)
    if params.upper_bound is not None and not FORCE_RELU:
        relu_ub = qrec.in_qs[0].quantize(params.upper_bound)
        in_tensor = np.minimum(in_tensor, relu_ub)
    in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor)
    if qrec.out_qs[0] != qrec.in_qs[0]:
        return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, qrec.in_qs[0])], ktype="symmetric")
    return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


def relu(params,
         in_tensors,
         qrec: QuantizationRecordBase,
         details=None):

    if isinstance(qrec, MultQuantizationRecord):
        return relu_mult(params, in_tensors, qrec, details=details)

    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]

    relu_lb = qrec.in_qs[0].quantize(params.lower_bound)
    in_tensor = np.maximum(in_tensor, relu_lb)
    if params.upper_bound is not None:
        relu_ub = qrec.in_qs[0].quantize(params.upper_bound)
        in_tensor = np.minimum(in_tensor, relu_ub)

    if qrec.out_qs[0] != qrec.in_qs[0]:
        return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, qrec.in_qs[0])], ktype="symmetric")
    return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


def hsigmoid_mult_gen_factors(params, qrec):
    in_q = qrec.in_qs[0]
    fac_1 = in_q.quantize(np.array([params.offset]))
    qrec.set_scale(extra_scale=1/6)
    upper_bound = in_q.quantize([6.])
    lower_bound = in_q.quantize([0.])
    return fac_1, upper_bound, lower_bound


def hsigmoid_mult(params,
                  in_tensors,
                  qrec: MultQuantizationRecord,
                  details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    fac_1, upper_bound, lower_bound = hsigmoid_mult_gen_factors(params, qrec)
    in_tensor = in_tensor.astype(np.int32)
    in_tensor_relued = np.minimum(np.maximum(in_tensor + fac_1, lower_bound), upper_bound)
    in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor_relued)
    return qrec.get_outputs(params,
                            [in_tensor],
                            ktype="symmetric")


def hsigmoid(params,
             in_tensors,
             qrec: QuantizationRecordBase,
             details=None):
    if isinstance(qrec, MultQuantizationRecord):
        return hsigmoid_mult(params, in_tensors, qrec, details=details)
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]

    calc_q = QType(bits=32, q=qrec.in_qs[0].q + 15, signed=True)

    fac_1 = qrec.in_qs[0].quantize(np.array([params.offset]))
    fac_2 = (1 << 15) // 6
    upper_bound = qrec.in_qs[0].quantize(np.array([6.]))
    lower_bound = qrec.in_qs[0].quantize(np.array([0.]))
    in_tensor = in_tensor.astype(np.int32)
    in_tensor = np.multiply(np.minimum(np.maximum(in_tensor + fac_1, lower_bound),
                                       upper_bound), fac_2, dtype=np.int32)
    return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, calc_q)], ktype="symmetric")


def hswish_mult_gen_factors(qrec):
    in_q = qrec.in_qs[0]
    fac_1 = in_q.quantize(np.array([3.]))
    # The scale of the result is actually in in_scale * in_scale since it is multiplied by itself
    qrec.set_scale(extra_scale=qrec.in_qs[0].scale * 1/6)
    upper_bound = in_q.quantize([6.])
    lower_bound = in_q.quantize([0.])
    return fac_1, upper_bound, lower_bound


def hswish_mult(params,
                in_tensors,
                qrec: MultQuantizationRecord,
                details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    fac_1, upper_bound, lower_bound = hswish_mult_gen_factors(qrec)
    in_tensor = in_tensor.astype(np.int32)
    in_tensor_relued = np.minimum(np.maximum(in_tensor + fac_1, lower_bound), upper_bound)
    in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor * in_tensor_relued)
    return qrec.get_outputs(params,
                            [in_tensor],
                            ktype="symmetric")


def hswish(params,
           in_tensors,
           qrec: QuantizationRecordBase,
           details=None):

    if isinstance(qrec, MultQuantizationRecord):
        return hswish_mult(params, in_tensors, qrec, details=details)

    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]

    calc_q = QType(bits=32, q=qrec.in_qs[0].q + 15, signed=True)
    fac_1 = qrec.in_qs[0].quantize(np.array([3.]))
    fac_2 = (1 << 15) // 6
    upper_bound = qrec.in_qs[0].quantize([6.])
    lower_bound = qrec.in_qs[0].quantize([0.])
    in_tensor = in_tensor.astype(np.int32)
    in_tensor = at_norm(np.multiply(np.minimum(np.maximum(in_tensor + fac_1, lower_bound), upper_bound),
                                    in_tensor,
                                    dtype=np.int32), qrec.in_qs[0].q)
    return qrec.get_outputs(params,
                            [qrec.out_qs[0].reduce_from(np.multiply(
                                in_tensor, fac_2, dtype=np.int32), calc_q)],
                            ktype="symmetric")
