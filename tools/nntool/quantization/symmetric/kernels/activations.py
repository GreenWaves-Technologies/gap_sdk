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
from graph.types import (HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         ReluActivationParameters)
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              qrec_type, quantization)
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.qtype import QType
from quantization.quantization_record_base import QuantizationRecordBase
from utils.at_norm import at_norm


class NNForceRelu:
    FORCE_RELU = True


def set_force_relu(force_relu: bool):
    NNForceRelu.FORCE_RELU = force_relu


def get_force_relu():
    return NNForceRelu.FORCE_RELU


def leak_mult_gen_factor_q7(params):
    leak_factor = np.array([params.leak_factor])
    leak_factor = (leak_factor * 2**7).astype(np.int8)
    return leak_factor


@params_type(LeakyActivationParameters)
@quantization('symmetric')
class LeakySymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        raise NotImplementedError()


@params_type(LeakyActivationParameters)
@quantization('symmetric')
@qrec_type(MultQuantizationRecord)
class LeakySymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
        qrec.set_scale()
        neg_in = at_norm(in_tensor * leak_mult_gen_factor_q7(params), 7)
        in_tensor = in_tensor * (in_tensor > 0) + neg_in * (in_tensor < 0)

        in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor)
        if qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, qrec.in_qs[0])], ktype="symmetric")
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


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


@params_type(ReluActivationParameters)
@quantization('symmetric')
@qrec_type(MultQuantizationRecord)
class ReluSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
        qrec.set_scale()
        relu_lb = qrec.in_qs[0].quantize(params.lower_bound)
        in_tensor = np.maximum(in_tensor, relu_lb)
        if params.upper_bound is not None and not NNForceRelu.FORCE_RELU:
            relu_ub = qrec.in_qs[0].quantize(params.upper_bound)
            in_tensor = np.minimum(in_tensor, relu_ub)
        in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor)
        if qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, qrec.in_qs[0])], ktype="symmetric")
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


@params_type(ReluActivationParameters)
@quantization('symmetric')
class ReluSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

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


@params_type(HSigmoidActivationParameters)
@quantization('symmetric')
@qrec_type(MultQuantizationRecord)
class HSigmoidSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
        fac_1, upper_bound, lower_bound = hsigmoid_mult_gen_factors(params, qrec)
        in_tensor = in_tensor.astype(np.int32)
        in_tensor_relued = np.minimum(np.maximum(in_tensor + fac_1, lower_bound), upper_bound)
        in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor_relued)
        return qrec.get_outputs(params,
                                [in_tensor],
                                ktype="symmetric")


@params_type(HSigmoidActivationParameters)
@quantization('symmetric')
class HSigmoidSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
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


@params_type(HSwishActivationParameters)
@quantization('symmetric')
@qrec_type(MultQuantizationRecord)
class HSwishSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
        fac_1, upper_bound, lower_bound = hswish_mult_gen_factors(qrec)
        in_tensor = in_tensor.astype(np.int32)
        in_tensor_relued = np.minimum(np.maximum(in_tensor + fac_1, lower_bound), upper_bound)
        in_tensor = qrec.scale_mul_biases_q.apply_scales(in_tensor * in_tensor_relued)
        return qrec.get_outputs(params,
                                [in_tensor],
                                ktype="symmetric")


@params_type(HSwishActivationParameters)
@quantization('symmetric')
class HSwishSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

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
