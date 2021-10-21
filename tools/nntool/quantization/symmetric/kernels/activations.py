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

import math

import numpy as np
from graph.types import (HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         ReluActivationParameters, SigmoidActivationParameters)
from graph.types.activations import (HTanHActivationParameters,
                                     TanHActivationParameters)
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.multiplicative.mulbias import compute_in_out_scale
from quantization.multiplicative.utils.scale import compute_scales
from quantization.new_qrec import QRec
from quantization.qtype import QType
from utils.at_norm import at_norm
from utils.sigmoid_tanh_lut import sigmoid_lut, tanh_lut


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
@qrec_type('symmetric')
class LeakySymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        raise NotImplementedError()


@params_type(LeakyActivationParameters)
@qrec_type('scaled')
class LeakySymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        compute_in_out_scale(qrec)
        in_tensor = in_tensor.astype(np.int32)
        neg_in = at_norm(in_tensor * leak_mult_gen_factor_q7(params), 7)
        in_tensor = in_tensor * (in_tensor > 0) + neg_in * (in_tensor < 0)
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
        in_tensor = scale_mul_biases_q.apply_scales(in_tensor)
        if qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, qrec.in_qs[0])], ktype="symmetric")
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


def sigmoid(params,
            in_tensors,
            qrec: QRec,
            details=None):
    del details
    if qrec.ktype == 'scaled':
        raise NotImplementedError()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    dqinput = qrec.in_qs[0].dequantize(in_tensor)
    return qrec.get_outputs(params, [qrec.out_qs[0].quantize(1/(1 + np.exp(-dqinput)))], ktype="symmetric")


@params_type(ReluActivationParameters)
@qrec_type('scaled')
class ReluSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        compute_in_out_scale(qrec)
        relu_lb = qrec.in_qs[0].quantize(params.lower_bound)
        in_tensor = np.maximum(in_tensor, relu_lb)
        if params.upper_bound is not None and not NNForceRelu.FORCE_RELU:
            relu_ub = qrec.in_qs[0].quantize(params.upper_bound)
            in_tensor = np.minimum(in_tensor, relu_ub)
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
        in_tensor = scale_mul_biases_q.apply_scales(in_tensor)
        if qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor.astype(np.int32), qrec.in_qs[0])], ktype="symmetric")
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


@params_type(ReluActivationParameters)
@qrec_type('symmetric')
class ReluSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]

        relu_lb = qrec.in_qs[0].quantize(params.lower_bound)
        in_tensor = np.maximum(in_tensor, relu_lb)
        if params.upper_bound is not None:
            relu_ub = qrec.in_qs[0].quantize(params.upper_bound)
            in_tensor = np.minimum(in_tensor, relu_ub)

        if qrec.out_qs[0] != qrec.in_qs[0]:
            return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor.astype(np.int32), qrec.in_qs[0])], ktype="symmetric")
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


def hsigmoid_mult_gen_factors(params, qrec):
    in_q = qrec.in_qs[0]
    fac_1 = in_q.quantize(np.array([params.offset]))
    compute_in_out_scale(qrec, extra_scale=1/6)
    upper_bound = in_q.quantize([6.])
    lower_bound = in_q.quantize([0.])
    return fac_1, upper_bound, lower_bound


@params_type(HSigmoidActivationParameters)
@qrec_type('scaled')
class HSigmoidSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        fac_1, upper_bound, lower_bound = hsigmoid_mult_gen_factors(
            params, qrec)
        in_tensor = in_tensor.astype(np.int32)
        in_tensor_relued = np.minimum(np.maximum(
            in_tensor + fac_1, lower_bound), upper_bound)
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
        in_tensor = scale_mul_biases_q.apply_scales(in_tensor_relued)
        return qrec.get_outputs(params,
                                [in_tensor],
                                ktype="symmetric")


@params_type(HSigmoidActivationParameters)
@qrec_type('symmetric')
class HSigmoidSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]

        calc_q = QType.Pow2(bits=32, q=qrec.in_qs[0].q + 15, signed=True)

        fac_1 = qrec.in_qs[0].quantize(np.array([params.offset]))
        fac_2 = (1 << 15) // 6
        upper_bound = qrec.in_qs[0].quantize(np.array([6.]))
        lower_bound = qrec.in_qs[0].quantize(np.array([0.]))
        in_tensor = in_tensor.astype(np.int32)
        in_tensor = np.multiply(np.minimum(np.maximum(in_tensor + fac_1, lower_bound),
                                           upper_bound), fac_2, dtype=np.int32)
        return qrec.get_outputs(params, [qrec.out_qs[0].reduce_from(in_tensor, calc_q)], ktype="symmetric")


def sigmoid_mult_gen_factors(params, qrec):
    in_q = qrec.in_qs[0]
    mul_bias_sigmoid = in_q.scale / math.pow(2, -12)
    return compute_scales(mul_bias_sigmoid)


@params_type(HTanHActivationParameters)
@qrec_type('symmetric')
class HTanHSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]

        upper_bound = qrec.in_qs[0].quantize(np.array([1.]))
        in_tensor = np.minimum(np.maximum(in_tensor, -upper_bound),
                               upper_bound)
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


@params_type(SigmoidActivationParameters)
@qrec_type('scaled')
class SigmoidScaledSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        out_q15 = sigmoid_lut(in_tensor.astype(np.int32) << 8)
        compute_in_out_scale(qrec, extra_scale=QType.Pow2(
            bits=32, q=7, signed=True).scale/qrec.in_qs[0].scale)
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
        output = scale_mul_biases_q.apply_scales(out_q15 >> 8)

        return qrec.get_outputs(params,
                                [output],
                                ktype="symmetric")


@params_type(SigmoidActivationParameters)
@qrec_type('symmetric')
class SigmoidSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        in_q = qrec.in_qs[0]
        out_q = qrec.out_qs[0]
        in_tensor = in_tensor.astype(np.int32)
        if in_q.q < 12:
            in_tensor <<= 12 - in_q.q
        elif in_q.q > 12:
            in_tensor = at_norm(in_tensor, in_q.q - 12)
        out_tensor = sigmoid_lut(in_tensor)
        if out_q.q < 15:
            out_tensor = at_norm(out_tensor, 15 - out_q.q)

        return qrec.get_outputs(params,
                                [out_tensor],
                                ktype="symmetric")


@params_type(TanHActivationParameters)
@qrec_type('scaled')
class TanHScaledMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        out_q15 = tanh_lut(in_tensor.astype(np.int32) << 8)
        compute_in_out_scale(qrec, extra_scale=QType.Pow2(
            bits=32, q=7, signed=True).scale/qrec.in_qs[0].scale)
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
        output = scale_mul_biases_q.apply_scales(out_q15 >> 8)

        return qrec.get_outputs(params,
                                [output],
                                ktype="symmetric")


@params_type(TanHActivationParameters)
@qrec_type('symmetric')
class TanHSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        in_q = qrec.in_qs[0]
        out_q = qrec.out_qs[0]
        in_tensor = in_tensor.astype(np.int32)
        if in_q.q < 12:
            in_tensor <<= 12 - in_q.q
        elif in_q.q > 12:
            in_tensor = at_norm(in_tensor, in_q.q - 12)
        out_tensor = tanh_lut(in_tensor)
        if out_q.q < 15:
            out_tensor = at_norm(out_tensor, 15 - out_q.q)

        return qrec.get_outputs(params,
                                [out_tensor],
                                ktype="symmetric")


def hswish_mult_gen_factors(qrec):
    in_q = qrec.in_qs[0]
    fac_1 = in_q.quantize(np.array([3.]))
    # The scale of the result is actually in in_scale * in_scale since it is multiplied by itself
    compute_in_out_scale(qrec, extra_scale=qrec.in_qs[0].scale * 1/6)
    upper_bound = in_q.quantize([6.])
    lower_bound = in_q.quantize([0.])
    return fac_1, upper_bound, lower_bound


@params_type(HSwishActivationParameters)
@qrec_type('scaled')
class HSwishSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        fac_1, upper_bound, lower_bound = hswish_mult_gen_factors(qrec)
        in_tensor = in_tensor.astype(np.int32)
        in_tensor_relued = np.minimum(np.maximum(
            in_tensor + fac_1, lower_bound), upper_bound)
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
        in_tensor = scale_mul_biases_q.apply_scales(
            in_tensor * in_tensor_relued)
        return qrec.get_outputs(params,
                                [in_tensor],
                                ktype="symmetric")


@params_type(HSwishActivationParameters)
@qrec_type('symmetric')
class HSwishSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]

        calc_q = QType.Pow2(bits=32, q=qrec.in_qs[0].q + 15, signed=True)
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
