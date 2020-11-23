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
from typing import Mapping, Sequence, Any

import numpy as np

from graph.types import LSTMParameters, RNNBaseParameters, RNNParameters
from quantization.multiplicative.mult_quantization import \
    MultScalableRnnQuantizationRecord
from quantization.qtype_base import QTypeBase
from quantization.quantized_ndarray import QuantizedNdarray
from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)

# Q16 (uint16) LUT for sigmoid function
sigmoid_table = np.array(
    [32768, 33451, 34133, 34813, 35493, 36169, 36843, 37513, 38180, 38841, 39498,
    40149, 40794, 41432, 42064, 42688, 43304, 43912, 44511, 45102, 45683, 46255,
    46817, 47369, 47911, 48443, 48964, 49475, 49975, 50464, 50942, 51409, 51865,
    52311, 52745, 53169, 53581, 53983, 54374, 54755, 55125, 55485, 55834, 56174,
    56503, 56823, 57133, 57433, 57724, 58007, 58280, 58544, 58800, 59048, 59288,
    59519, 59743, 59959, 60168, 60370, 60565, 60753, 60935, 61110, 61279, 61441,
    61599, 61750, 61896, 62036, 62172, 62302, 62428, 62549, 62666, 62778, 62886,
    62990, 63090, 63186, 63279, 63368, 63454, 63536, 63615, 63691, 63765, 63835,
    63903, 63968, 64030, 64090, 64148, 64204, 64257, 64308, 64357, 64405, 64450,
    64494, 64536, 64576, 64614, 64652, 64687, 64721, 64754, 64786, 64816, 64845,
    64873, 64900, 64926, 64950, 64974, 64997, 65019, 65039, 65060, 65079, 65097,
    65115, 65132, 65149, 65164, 65179, 65194, 65208, 65221, 65234, 65246, 65258,
    65269, 65280, 65291, 65301, 65310, 65319, 65328, 65337, 65345, 65352, 65360,
    65367, 65374, 65381, 65387, 65393, 65399, 65404, 65410, 65415, 65420, 65425,
    65429, 65433, 65438, 65442, 65445, 65449, 65453, 65456, 65459, 65462, 65465,
    65468, 65471, 65474, 65476, 65479, 65481, 65483, 65485, 65488, 65489, 65491,
    65493, 65495, 65497, 65498, 65500, 65501, 65503, 65504, 65505, 65507, 65508,
    65509, 65510, 65511, 65512, 65513, 65514, 65515, 65516, 65517, 65517, 65518,
    65519, 65520, 65520, 65521, 65522, 65522, 65523, 65523, 65524, 65524, 65525,
    65525, 65526, 65526, 65526, 65527, 65527, 65528, 65528, 65528, 65529, 65529,
    65529, 65529, 65530, 65530, 65530, 65530, 65531, 65531, 65531, 65531, 65531,
    65532, 65532, 65532, 65532, 65532, 65532, 65533, 65533, 65533, 65533, 65533,
    65533, 65533, 65533, 65534, 65534, 65534, 65534, 65534, 65534, 65534, 65534,
    65534, 65534, 65535])

NEAREST = True

def sigmoidLUT(x, qtype):
    del qtype
    # Scale [-8:8] into [-10.7:10.7] --> *3/4
    result = np.empty_like(x)
    if not NEAREST:
        for i, _ in enumerate(x):
            abs_x = (np.abs(x[i]) * 3) >> 9 #input in Q12
            if abs_x >= 255:
                result[i] = 0x7FFF << 10
            else:
                ua = sigmoid_table[abs_x]
                ub = sigmoid_table[abs_x+1]
                ut = abs_x & 0xFF
                result[i] = (ua << 9) + ut * (ub-ua) #Q16*Q8 = Q24
        result = np.where(x>0, result + (1<<9), (1<<(9+16))-result+(1<<9)-1)
        return result >> 10
    else:
        for i, _ in enumerate(x):
            abs_x = (np.abs(x[i]) * 3) >> 9 #input in Q12
            if abs_x >= 255:
                result[i] = 0xFFFF
            else:
                result[i] = sigmoid_table[abs_x]
        result = np.where(x>0, result, (1<<16)-result)
        return result >> 1


def tanhLUT(x, qtype):
    del qtype
    # Scale [-8:8] into [-10.7:10.7] --> *3/4
    result = np.empty_like(x)
    if not NEAREST:
        for i, _ in enumerate(x):
            abs_x = (np.abs(x[i]) * 3) >> 8 # 2*abs_x
            if abs_x >= 255:
                result[i] = 0xFFFF << 8
            else:
                ua = sigmoid_table[abs_x]
                ub = sigmoid_table[abs_x+1]
                ut = abs_x & 0xFF
                result[i] = (ua << 8) + ut * (ub-ua) #Q16*Q8 = Q24
        result = np.where(x>0, result - (1<<(9+14)) + (1<<(9-2)), -result + (1<<(9+14)) + (1<<(9-2)) - 1)
        return result >> 8
    else:
        for i, _ in enumerate(x):
            abs_x = (np.abs(x[i]) * 3) >> 8 # 2*abs_x
            if abs_x >= 255:
                result[i] = 0xFFFF
            else:
                result[i] = sigmoid_table[abs_x]
        result = np.where(x>0, result - (1<<15), -result + (1<<15))
        return result


# Another TANH and SIGMOID approx -> less precise
# def exp_taylor_quant(x, qtype, order='third'):
#     ONE_OVER_3 = qtype.quantize(np.array([1.0 / 3.0]))
#     ONE = qtype.quantize(np.array([1]))
#     x2 = (x.astype(np.int32)*x) >> qtype.q
#     x3 = (x2*x) >> qtype.q
#     if order == 'third':
#         x3_over_6_plus_x2_over_2 = (((x3 * ONE_OVER_3) >> qtype.q) + x2) >> 1
#         return ONE + ((ONE * (x + x3_over_6_plus_x2_over_2)) >> qtype.q)
#     x4 = (x3*x) >> qtype.q
#     if order == 'fourth':
#         x4_over_4 = x4>>2
#         x4_over_24_plus_x3_over_6_plus_x2_over_2 = ((((x4_over_4 + x3) * ONE_OVER_3) >> qtype.q) + x2) >> 1
#         return ONE + ((ONE * (x + x4_over_24_plus_x3_over_6_plus_x2_over_2)) >> qtype.q)
# def quant_tanh(x, qtype, k=3):
#     K = qtype.quantize(np.array([k])).astype(np.int32)
#     ONE = qtype.quantize(np.array([1])).astype(np.int32)
#     result_neg = ((ONE-exp_taylor_quant(-2*x, qtype).astype(np.int32)).astype(np.int32)<<qtype.q)//(ONE+exp_taylor_quant(-2*x, qtype))
#     result_pos = ((ONE-exp_taylor_quant(2*x, qtype).astype(np.int32)).astype(np.int32)<<qtype.q)//(ONE+exp_taylor_quant(2*x, qtype))
#     return np.where(x<(-K), -ONE, np.where(x>K, ONE, np.where(x<0, result_neg, -result_pos)))
# def quant_sigmoid(x, qtype):
#     ONE = qtype.quantize(np.array([1])).astype(np.int32)
#     return np.where(x>0, (exp_taylor_quant(x, qtype) << qtype.q) // (ONE + exp_taylor_quant(x, qtype)), 
#                          (ONE << qtype.q) // (ONE + exp_taylor_quant(-x, qtype)))


def abs_clip(arr: np.ndarray, abs_limit):
    return np.clip(arr, -abs_limit, abs_limit)


def relu(x, qtype):
    del qtype
    return np.minimum(x, 0)


def sigmoid(x, qtype):
    x = qtype.dequantize(x)
    pos_mask = (x >= 0)
    neg_mask = (x < 0)
    z = np.zeros_like(x)
    z[pos_mask] = np.exp(-x[pos_mask])
    z[neg_mask] = np.exp(x[neg_mask])
    top = np.ones_like(x)
    top[neg_mask] = z[neg_mask]
    return qtype.quantize(top / (1 + z))


def hsigmoid(x, qtype):
    x = x.astype(np.int32)
    relued = np.maximum(0,
                        np.minimum(qtype.quantize(np.array([3])) + x,
                                   qtype.quantize(np.array([6]))))
    relued *= qtype.quantize(np.array(1/6))
    relued += (1 << (qtype.q - 1))
    relued >>= qtype.q
    return relued


def mean_stddev_normalization(arr: np.ndarray):
    mean = np.mean(arr)
    variance = np.sum(np.square(arr - mean)) / arr.size()
    stddev_inv = 1.0 / np.sqrt(variance + 1e-8)
    return (arr - mean) * stddev_inv


def htanh(x, qtype):
    one = qtype.quantize(np.array([1]))
    return np.minimum(np.maximum(x, -one), one)


def tanh(x, qtype):
    return qtype.quantize(np.tanh(qtype.dequantize(x)))


def get_activation(name, use_hard):
    if name == 'relu':
        return relu
    if name == 'sigmoid':
        return hsigmoid if use_hard else sigmoidLUT
    if name == 'tanh':
        return htanh if use_hard else tanhLUT
    raise NotImplementedError("This activation is not implemented")


def rnn_step(params: RNNParameters,
             args: Mapping[str, Sequence],
             idx: int,
             input_tensor: np.ndarray,
             qrec):

    # These two sections could be combined by stacking the weights horizontally
    # and the input and state vertically

    # For each cell: compute input_weight * input if there is an input
    if idx < params.n_input_cells:
        # scale result to recurrent_weight * input_state scale
        input_gate_scratch = qrec.scale_input(
            params,
            args['i_2_i_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            args,
            ktype="symmetric")

    # biases already in recurrent_weight * input_state scale
    input_gate_scratch_state = args['i_b'][0].copy()

    # For each cell: compute recurrent_weight * input_state
    input_gate_scratch_state += args['r_2_i_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))

    # scale to state scale
    input_gate_scratch = qrec.scale_state(
        params,
        input_gate_scratch+input_gate_scratch_state,
        0,
        args,
        ktype="symmetric")

    # apply activation at state scale
    input_gate_scratch = get_activation(params.activation, params.hard_act)(input_gate_scratch, args['i_state'][1])
    # store the state
    args['i_state'][0] = input_gate_scratch.copy()

    # scale the state scale to the output scale
    return qrec.scale_output(
        params,
        input_gate_scratch,
        0,
        args,
        ktype="symmetric")


def lstm_step(params: LSTMParameters,
              args: Mapping[str, np.ndarray],
              idx: int,
              input_tensor: np.ndarray,
              qrec):

    use_cifg = 'i_2_i_w' in args and args['i_2_i_w'][0] is None
    use_peephole = 'c_2_o_w' in args and args['c_2_o_w'][0] is not None
    use_layer_norm = 'f_norm' in args and args['f_norm'][0] is not None
    if use_cifg:
        raise NotImplementedError("cifg mode is not supported")
    if use_peephole:
        raise NotImplementedError("peephole mode is not supported")
    if use_layer_norm:
        raise NotImplementedError("layer norm mode is not supported")

    sigmoid = hsigmoid if params.hard_act else sigmoidLUT
    tanh = htanh if params.hard_act else tanhLUT

    # INPUT vs WEIGHTS
    # For each cell: compute input_weight * input if there is an input
    if idx < params.n_input_cells:
        input_gate_scratch = qrec.scale_input_input(
            args['i_2_i_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")
        forget_gate_scratch = qrec.scale_input_forget(
            args['i_2_f_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")
        cell_scratch = qrec.scale_input_cell(
            args['i_2_c_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")
        output_gate_scratch = qrec.scale_input_output(
            args['i_2_o_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")

    # Initialize scratch buffers with bias for regular lstm
    input_gate_scratch_state = args['i_b'][0].astype(np.int32)
    forget_gate_scratch_state = args['f_b'][0].astype(np.int32)
    cell_scratch_state = args['c_b'][0].astype(np.int32)
    output_gate_scratch_state = args['o_b'][0].astype(np.int32)

    # STATE vs WEIGHTS INITIALIZED WITH BIASES
    # For each cell: compute recurrent_weight * output_state
    input_gate_scratch_state += args['r_2_i_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))
    forget_gate_scratch_state += args['r_2_f_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))
    cell_scratch_state += args['r_2_c_w'][0].astype(np.int32).dot(args['i_state'][0].astype(np.int32))
    output_gate_scratch_state += args['r_2_o_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))

    input_gate_scratch = qrec.scale_internal_input(input_gate_scratch_state+input_gate_scratch,
                                                   0, ktype="symmetric")
    forget_gate_scratch = qrec.scale_internal_forget(forget_gate_scratch_state+forget_gate_scratch,
                                                     0, ktype="symmetric")
    cell_scratch = qrec.scale_internal_cell(cell_scratch_state+cell_scratch,
                                            0, ktype="symmetric")
    output_gate_scratch = qrec.scale_internal_output(output_gate_scratch_state+output_gate_scratch,
                                                     0, ktype="symmetric")

    # Apply activations in internal Q * 1
    input_gate_scratch = sigmoid(input_gate_scratch, qrec.internal_qtype)

    forget_gate_scratch = sigmoid(forget_gate_scratch, qrec.internal_qtype)

    output_gate_scratch = sigmoid(output_gate_scratch, qrec.internal_qtype)

    cell_scratch = tanh(cell_scratch, qrec.internal_qtype)

    # cstate = cstate * Of + Og * Oi
    if params.hard_act:
        # Scale cell state to internal Q * 1
        cstate = qrec.scale_cellin(args['c_state'][0].astype(np.int32), 0, ktype="symmetric")
        cstate = cstate * forget_gate_scratch + cell_scratch * input_gate_scratch
        # cstate now in (2 * Q) * 1
    else:
        # Multiply cstate [Scstate] * Of [Sq15] and scale to [Sq12]
        # Multiply Og [Sq15] * Oi [Sq15] --> [Sq30] >> 30-12 --> [Sq12]
        # cstate is now in q12 = internal_qtype
        cstate = qrec.scale_cellin(args['c_state'][0] * forget_gate_scratch, 0, ktype='symmetric') \
                 + ((cell_scratch * input_gate_scratch) >> (15+(15-qrec.internal_qtype.q)))

    # if params.cell_clip > 0.0:
    #     args['c_state'] = abs_clip(args['c_state'], params.cell_clip)
    # if there is a clip value this should override the min max here
    # clip here

    args['c_state'][0] = qrec.scale_cellout(cstate,
                                            0, ktype="symmetric")

    if params.hard_act:
        two_qtype = QType(qrec.internal_qtype, q = qrec.internal_qtype.q * 2)
        cell_scratch = tanh(cstate, two_qtype)
        # Assume scaling from internalq * 3 -> Q7 * 1
        output_gate_scratch *= cell_scratch
    else:
        cell_scratch = tanh(cstate, qrec.internal_qtype)
        # output = Og[Sq15] * tanh(cell_scratch)[Sq15] -> [Sq30] >> 15 -> [Sq15]
        output_gate_scratch = (output_gate_scratch * cell_scratch) >> 15

    output_gate_scratch = qrec.scale_output(output_gate_scratch,
                                            0, ktype="symmetric")
    output_gate_scratch = qrec.out_qs[0].clip(output_gate_scratch)

    use_projection_weight = 'proj_w' in args and args['proj_w'][0] is not None
    use_projection_bias = 'proj_b' in args and args['proj_b'][0] is not None

    if use_projection_weight or use_projection_bias:
        raise NotImplementedError("LSTMP is not yet supported by kernel")


    args['i_state'][0] = output_gate_scratch.copy()
    return output_gate_scratch


STEP_KERNEL = {
    RNNParameters: rnn_step,
    LSTMParameters: lstm_step
}


def rnn(params: RNNBaseParameters,
        in_tensors: Sequence[np.ndarray],
        qrec: MultScalableRnnQuantizationRecord,
        details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    args = {params.INPUT_NAMES[idx]: [in_tensors[idx], qrec.in_qs[idx]]
            for idx in range(1, len(in_tensors))}
    if params.revert:
        in_tensor = np.flip(in_tensor, axis=0)
    assert in_tensor.shape[0] == params.n_input_cells, "input shape incorrect - n_input_cells"
    assert in_tensor.shape[1] == params.n_inputs, "input shape incorrect - n_inputs"
    out_tensor = np.zeros([params.n_output_cells, params.n_states], dtype=qrec.out_qs[0].dtype)
    out_idx = 0

    step_kernel = STEP_KERNEL[params.__class__]
    for idx in range(params.n_cells):
        res = step_kernel(params, args, idx, in_tensor, qrec)
        if idx >= (params.n_cells - params.n_output_cells):
            out_tensor[out_idx] = res
            out_idx += 1

    if params.revert:
        out_tensor = np.flip(out_tensor, axis=0)
    return [out_tensor]
