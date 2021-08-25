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
import math
from typing import Mapping

import numpy as np
from graph.types import LSTMParameters, RNNParameters
from graph.types.rnn import GRUParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import QRec
from quantization.qtype import QType
from utils.at_norm import at_norm
from utils.diag_collector import DiagCollector
from utils.sigmoid_tanh_lut import sigmoid_lut, tanh_lut

LOG = logging.getLogger("nntool." + __name__)

# for debugging this can be switched to np.in64
INT_DTYPE = np.int32

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
        return hsigmoid if use_hard else sigmoid_lut
    if name == 'tanh':
        return htanh if use_hard else tanh_lut
    raise NotImplementedError("This activation is not implemented")


class RnnSymmetricMixin():
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        del kwargs
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        args = {params.INPUT_NAMES[idx]: [in_tensors[idx], qrec.in_qs[idx]]
                for idx in range(1, len(in_tensors))}
        if params.revert:
            in_tensor = np.flip(in_tensor, axis=0)
        assert in_tensor.shape[0] == params.n_input_cells, "input shape incorrect - n_input_cells"
        assert in_tensor.shape[1] == params.n_inputs, "input shape incorrect - n_inputs"
        out_tensor = np.zeros(
            [params.n_output_cells, params.n_states], dtype=qrec.out_qs[0].dtype)
        out_idx = 0

        new_c_state = None
        for idx in range(params.n_cells):
            if isinstance(params, LSTMParameters):
                res, new_c_state = cls.step_kernel(
                    params, args, idx, in_tensor, qrec)
            else:
                res = cls.step_kernel(params, args, idx, in_tensor, qrec)
            if idx >= (params.n_cells - params.n_output_cells):
                out_tensor[out_idx] = res
                out_idx += 1

        if params.revert:
            out_tensor = np.flip(out_tensor, axis=0)
        if params.output_directions:
            out_tensor = np.expand_dims(out_tensor, 0)
        if new_c_state is not None:
            return [out_tensor, new_c_state]
        return [out_tensor]


def scale_rnn_input(qrec: QRec,
                    weighted_input_tensor: np.ndarray,
                    axis: int):
    # For AT model creation this should not be set. This is just for simulation
    # i.e. input scale == state scale == output scale
    # scale input_scale * input_weights to state_scale * recurrent_weights_scale
    weighted_input_tensor = weighted_input_tensor.astype(np.int32)
    return qrec.cache['i_2_a_q'].apply_scales(weighted_input_tensor, axis)


def scale_rnn_output(qrec,
                     state_tensor: np.ndarray,
                     axis: int):
    o_q = qrec.out_qs[0]
    # scale state_scale to output_scale
    return o_q.clip(qrec.cache['s_2_o_q'].apply_scales(state_tensor, axis))


def scale_rnn_state(qrec,
                    state_tensor: np.ndarray,
                    axis: int):
    # scale state_scale * recurrent_weights_scale to internal_scale
    return qrec.cache['s_2_s_q'].apply_scales(state_tensor, axis)


@params_type(RNNParameters)
@qrec_type('scaled')
class RNNSymmetric(RnnSymmetricMixin, KernelBase):
    @classmethod
    def step_kernel(cls, params: GRUParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    qrec):
        if args['i_state'][1].dtype == np.int16:
            return cls.step_kernel16_8(params, args, idx, input_tensor, qrec)
        return cls.step_kernel8_8(params, args, idx, input_tensor, qrec)

    @classmethod
    def step_kernel8_8(cls, params: GRUParameters,
                       args: Mapping[str, np.ndarray],
                       idx: int,
                       input_tensor: np.ndarray,
                       qrec):

        # These two sections could be combined by stacking the weights horizontally
        # and the input and state vertically
        scales = qrec.cache['scales']

        # For each cell: compute input_weight * input if there is an input
        if idx < params.n_input_cells:
            # scale result to recurrent_weight * input_state scale
            input_gate_scratch = scale_rnn_input(
                qrec,
                args['i_2_i_w'][0].astype(INT_DTYPE).dot(
                    input_tensor[idx].astype(INT_DTYPE)),
                0)
        # biases already in recurrent_weight * input_state scale
        input_gate_scratch_state = args['i_b'][0].copy()

        # For each cell: compute recurrent_weight * input_state
        input_gate_scratch_state += args['r_2_i_w'][0].astype(
            INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))

        # scale to state scale
        input_gate_scratch = scale_rnn_state(qrec,
                                             input_gate_scratch+input_gate_scratch_state,
                                             0)

        # apply activation at state scale
        input_gate_scratch = get_activation(params.activation, params.hard_act)(
            input_gate_scratch, args['i_state'][1])

        DiagCollector.record(
            'h_state_prescale', input_gate_scratch, scale=scales['int_scale'], node=params)
        # scale the state scale to the output scale
        output_gate_scratch = scale_rnn_output(qrec, input_gate_scratch, 0)
        DiagCollector.record(
            'h_state_out', output_gate_scratch, scale=scales['out_scale'], node=params)
        # store the state
        args['i_state'][0] = output_gate_scratch.copy()
        return output_gate_scratch

    @classmethod
    def step_kernel16_8(cls, params: GRUParameters,
                        args: Mapping[str, np.ndarray],
                        idx: int,
                        input_tensor: np.ndarray,
                        qrec):

        # These two sections could be combined by stacking the weights horizontally
        # and the input and state vertically
        scales = qrec.cache['scales']

        # For each cell: compute input_weight * input if there is an input
        if idx < params.n_input_cells:
            # scale result to recurrent_weight * input_state scale
            input_gate_scratch = scale_rnn_input(
                qrec,
                args['i_2_i_w'][0].astype(INT_DTYPE).dot(
                    input_tensor[idx].astype(INT_DTYPE)),
                0)
        # biases already in recurrent_weight * input_state scale
        input_gate_scratch_state = args['i_b'][0].copy()

        # For each cell: compute recurrent_weight * input_state
        input_gate_scratch_state += args['r_2_i_w'][0].astype(
            INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))

        # scale to state scale
        input_gate_scratch = input_gate_scratch + scale_rnn_state(qrec,
                                                                  input_gate_scratch_state,
                                                                  0)

        # apply activation at state scale
        input_gate_scratch = get_activation(params.activation, params.hard_act)(
            input_gate_scratch, args['i_state'][1])

        DiagCollector.record(
            'h_state_prescale', input_gate_scratch, scale=scales['int_scale'], node=params)
        # scale the state scale to the output scale
        output_gate_scratch = qrec.out_qs[0].clip(input_gate_scratch)
        DiagCollector.record(
            'h_state_out', output_gate_scratch, scale=scales['out_scale'], node=params)
        # store the state
        args['i_state'][0] = output_gate_scratch.copy()
        return output_gate_scratch


def scale_to(qrec,
             var,
             tensor: np.ndarray,
             axis: int):
    qtype = qrec.cache[var]
    return qtype.apply_scales(tensor, axis)


def internal_qtype(qrec):
    return qrec.cache.get('i_qtype') or QType(bits=8, q=7, signed=True)


def scale_gru_z_input2_z_HtxW(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_z_WR_q', tensor, axis)


def scale_gru_r_input2_r_HtxW(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_r_WR_q', tensor, axis)


def scale_gru_h_input2_h_HtxW(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_h_WR_q', tensor, axis)


def scale_gru_z_internal(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'z_WR_2_int_q', tensor, axis)


def scale_gru_r_internal(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_WR_2_int_q', tensor, axis)


def scale_gru_h_internal(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'h_WR_2_int_q', tensor, axis)


@params_type(GRUParameters)
@qrec_type('scaled')
class GRUSymmetric(RnnSymmetricMixin, KernelBase):
    @classmethod
    def step_kernel(cls, params: GRUParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    qrec):
        if args['h_state'][1].dtype == np.int16:
            return cls.step_kernel16_8(params, args, idx, input_tensor, qrec)
        return cls.step_kernel8_8(params, args, idx, input_tensor, qrec)

    @classmethod
    def step_kernel8_8(cls, params: GRUParameters,
                       args: Mapping[str, np.ndarray],
                       idx: int,
                       input_tensor: np.ndarray,
                       qrec):

        z_gate_scratch = 0
        hr_gate_scratch = 0
        scales = qrec.cache['scales']

        DiagCollector.record(
            'h_state', args['h_state'][0], scale=scales['state'], node=params)
        DiagCollector.record(
            'input', input_tensor[idx], scale=scales['in'][0], node=params)

        in_tensor = input_tensor[idx].astype(INT_DTYPE)
        state_tensor = args['h_state'][0].astype(INT_DTYPE)

        DiagCollector.record('z_weigths', args['w_2_z_w'][0],
                             scale=scales['r_2_z_w'], node=params)
        if idx < params.n_input_cells:
            # calculate z gate on input
            z_gate_scratch += args['w_2_z_w'][0].astype(
                INT_DTYPE).dot(in_tensor)
            # calculate r gate on input
            hr_gate_scratch += args['w_2_r_w'][0].astype(
                INT_DTYPE).dot(in_tensor)
            # scale to recurrent * state scale if input scale is different
            DiagCollector.record('z_gate_inp', z_gate_scratch,
                                 scale=scales['w_2_z_w'] * scales['state'], node=params)
            DiagCollector.record('r_gate_inp', hr_gate_scratch,
                                 scale=scales['w_2_r_w'] * scales['state'], node=params)
            if not params.rnn_same_inout_scale:
                z_gate_scratch = scale_gru_z_input2_z_HtxW(qrec,
                                                           z_gate_scratch, 0)
                hr_gate_scratch = scale_gru_r_input2_r_HtxW(qrec,
                                                            hr_gate_scratch, 0)

        # calculate z gate on recurrent
        z_gate_scratch += args['r_2_z_w'][0].astype(
            INT_DTYPE).dot(state_tensor) + args['z_b'][0].copy()
        DiagCollector.record('z_gate', z_gate_scratch,
                             scale=scales['r_2_z_w'] * scales['state'], node=params)
        # if not hard_act then the scale will scale to Q15
        z_gate_scratch = get_activation(params.activation_zr, params.hard_act)(
            scale_gru_z_internal(qrec, z_gate_scratch, 0), internal_qtype(qrec))
        # normalise to internal Q
        if not params.hard_act and internal_qtype(qrec).q != 15:
            z_gate_scratch = at_norm(
                z_gate_scratch, 15 - internal_qtype(qrec).q)
        DiagCollector.record('z_gate_sigmoid', z_gate_scratch,
                             scale=internal_qtype(qrec).scale, node=params)

        # same as above on r gate
        hr_gate_scratch += args['r_2_r_w'][0].astype(
            INT_DTYPE).dot(state_tensor) + args['r_b'][0].copy()
        DiagCollector.record('r_gate', hr_gate_scratch,
                             scale=scales['r_2_r_w'] * scales['state'], node=params)
        hr_gate_scratch = get_activation(params.activation_zr, params.hard_act)(
            scale_gru_r_internal(qrec, hr_gate_scratch, 0), internal_qtype(qrec))
        if not params.hard_act and internal_qtype(qrec).q != 15:
            hr_gate_scratch = at_norm(
                hr_gate_scratch, 15 - internal_qtype(qrec).q)
        DiagCollector.record('r_gate_sigmoid', hr_gate_scratch,
                             scale=internal_qtype(qrec).scale, node=params)

        if params.linear_before_reset:
            # haddamard after linear
            # r_gate_scratch = (rt (.) (Ht-1*(Rh^T) + Rbh))
            h_gate_recurrent = args['r_2_h_w'][0].astype(
                INT_DTYPE).dot(state_tensor) + args['r_h_b'][0]
            DiagCollector.record(
                'h_gate_state', h_gate_recurrent,
                scale=math.pow(2, -7) * scales['r_2_h_w'],
                node=params)
            # this is int_q_scale * state_q_scale * h_recurrent_weights_scale
            hr_gate_scratch = hr_gate_scratch * h_gate_recurrent
            DiagCollector.record(
                'hr_haddamard', hr_gate_scratch,
                scale=math.pow(2, -7) * math.pow(2, -
                                                 internal_qtype(qrec).q) * scales['r_2_h_w'],
                node=params)

            # normalize to state_q_scale * h_recurrent_weights_scale
            hr_gate_scratch = at_norm(hr_gate_scratch, internal_qtype(qrec).q)

            # ht = g(Xt*(Wh^T) + (rt (.) (Ht-1*(Rh^T) + Rbh)) + Wbh) # when linear_before_reset != 0
            if idx < params.n_input_cells:
                if not params.rnn_same_inout_scale:
                    # scale input_scale * h_input_weights_scale to state_q_scale * h_recurrent_weights_scale
                    hr_gate_input = scale_gru_h_input2_h_HtxW(qrec,
                                                              (args['w_2_h_w'][0].astype(INT_DTYPE).dot(
                                                                  in_tensor) + args['w_h_b'][0]),
                                                              0)
                else:
                    # since input_scale == state scale and h_input_weights_scale == h_recurrent_weights_scale
                    # no scaling is necessary
                    hr_gate_input = args['w_2_h_w'][0].astype(
                        INT_DTYPE).dot(in_tensor) + args['w_h_b'][0]
            else:
                # Is this correct if there is no input (and below)? This is not a mode that
                # exists in any framework and will not ever be used at present
                if not params.rnn_same_inout_scale:
                    hr_gate_input = qrec.scale_h_input2_h_HtxW(
                        args['w_h_b'][0], 0)
                else:
                    hr_gate_input = args['w_h_b'][0]
        else:
            # haddamard on state before linear
            # r_gate_scratch = (rt (.) Ht-1)*(Rh^T) + Rbh + Wbh

            # this is int_q_scale * state_q_scale * h_recurrent_weights_scale
            # normalize to state_q_scale * h_recurrent_weights_scale
            hr_gate_scratch = state_tensor * hr_gate_scratch
            DiagCollector.record(
                'hr_haddamard', hr_gate_scratch,
                scale=math.pow(2, -7) * math.pow(2, -internal_qtype(qrec).q),
                node=params)
            hr_gate_scratch = at_norm(args['r_2_h_w'][0].astype(INT_DTYPE).dot(
                hr_gate_scratch), internal_qtype(qrec).q) + args['r_h_b'][0]
            DiagCollector.record(
                'h_gate_state', hr_gate_scratch,
                scale=math.pow(2, -7) * scales['r_2_h_w'],
                node=params)

            if idx < params.n_input_cells:
                if not params.rnn_same_inout_scale:
                    # scale input_scale * h_input_weights_scale to state_q_scale * h_recurrent_weights_scale
                    hr_gate_input = scale_gru_h_input2_h_HtxW(
                        qrec,
                        args['w_2_h_w'][0].dot(in_tensor) + args['w_h_b'][0],
                        0)
                else:
                    hr_gate_input = args['w_2_h_w'][0].astype(
                        INT_DTYPE).dot(in_tensor) + args['w_h_b'][0]
            else:
                if not params.rnn_same_inout_scale:
                    hr_gate_input = qrec.scale_h_input2_h_HtxW(
                        args['w_h_b'][0], 0)
                else:
                    hr_gate_input = args['w_h_b'][0]

        DiagCollector.record(
            'h_gate_input', hr_gate_input,
            scale=math.pow(2, -7) * scales['r_2_h_w'],
            node=params)
        hr_gate_scratch += hr_gate_input
        DiagCollector.record(
            'h_gate', hr_gate_scratch,
            scale=math.pow(2, -7) * scales['r_2_h_w'],
            node=params)

        # scale to q15 or internal Q depending on activation type
        hr_gate_scratch = get_activation(params.activation, params.hard_act)(
            scale_gru_h_internal(qrec, hr_gate_scratch, 0), internal_qtype(qrec))
        # if not hard then go from Q15 -> int_q
        if not params.hard_act and internal_qtype(qrec).q != 15:
            hr_gate_scratch = at_norm(
                hr_gate_scratch, 15 - internal_qtype(qrec).q)

        DiagCollector.record('hr_gate_tanh', hr_gate_scratch,
                             scale=math.pow(2, -internal_qtype(qrec).q), node=params)

        # ----------- SCALE Q7 -----------

        # Ht = (1 - zt) (.) ht + zt (.) Ht-1
        # zt = (1 - int_q) * Q7 + Q7 * Q7 = INT_Q * 2
        # >> and clip

        h_state = state_tensor.copy() << (internal_qtype(qrec).q - 7)
        h_state = (((internal_qtype(qrec).quantize(1) - z_gate_scratch) * hr_gate_scratch) +
                   (z_gate_scratch * h_state))
        DiagCollector.record('h_state_out_prenorm', h_state,
                             scale=math.pow(2, -(internal_qtype(qrec).q * 2)),
                             node=params)
        h_state = qrec.out_qs[0].clip(
            at_norm(
                h_state,
                (internal_qtype(qrec).q * 2) - 7)).astype(qrec.out_qs[0].dtype)
        DiagCollector.record('h_state_out', h_state,
                             scale=math.pow(2, -7),
                             node=params)
        args['h_state'][0] = h_state.copy()
        return h_state

    @classmethod
    def step_kernel16_8(cls, params: GRUParameters,
                        args: Mapping[str, np.ndarray],
                        idx: int,
                        input_tensor: np.ndarray,
                        qrec):

        z_gate_scratch = 0
        hr_gate_scratch = 0
        scales = qrec.cache['scales']
        DiagCollector.record(
            'h_state', args['h_state'][0], scale=scales['state'], node=params)
        DiagCollector.record(
            'input', input_tensor[idx], scale=scales['in'][0], node=params)

        in_tensor = input_tensor[idx].astype(INT_DTYPE)
        state_tensor = args['h_state'][0]

        DiagCollector.record('z_weigths', args['w_2_z_w'][0],
                             scale=scales['r_2_z_w'], node=params)
        if idx < params.n_input_cells:
            # calculate z gate on input
            z_gate_scratch += args['w_2_z_w'][0].astype(
                INT_DTYPE).dot(in_tensor)
            # calculate r gate on input
            hr_gate_scratch += args['w_2_r_w'][0].astype(
                INT_DTYPE).dot(in_tensor)
            # scale to recurrent * state scale if input scale is different
            DiagCollector.record('z_gate_inp', z_gate_scratch,
                                 scale=scales['w_2_z_w'] * scales['in'][0], node=params)
            DiagCollector.record('r_gate_inp', hr_gate_scratch,
                                 scale=scales['w_2_r_w'] * scales['in'][0], node=params)

            z_gate_scratch = scale_to(qrec,
                                      "input_z_w_internal",
                                      z_gate_scratch, 0)
            hr_gate_scratch = scale_to(qrec,
                                       "input_r_w_internal",
                                       hr_gate_scratch, 0)

        # calculate z gate on recurrent
        z_gate_state_scratch = args['r_2_z_w'][0].astype(
            INT_DTYPE).dot(state_tensor)

        z_gate_state_scratch = scale_to(qrec,
                                        "state_z_w_internal",
                                        z_gate_state_scratch, 0)

        # bias in Q12 input already in Q12
        z_gate_scratch += args['z_b'][0].copy() + z_gate_state_scratch

        DiagCollector.record(
            'z_gate', z_gate_scratch, scale=internal_qtype(qrec).scale, node=params)

        # will output Q15
        z_gate_scratch = get_activation(
            params.activation_zr, False)(z_gate_scratch, internal_qtype(qrec))

        # leave z in Q15

        DiagCollector.record('z_gate_sigmoid', z_gate_scratch,
                             scale=scales['act'], node=params)

        # same as above on r gate
        hr_gate_state_scratch = args['r_2_r_w'][0].astype(
            INT_DTYPE).dot(state_tensor)
        hr_gate_state_scratch = scale_to(
            qrec, "state_r_w_internal", hr_gate_state_scratch, 0)

        # bias in Q12 input already in Q12
        hr_gate_scratch += hr_gate_state_scratch + args['r_b'][0].copy()

        DiagCollector.record('r_gate', hr_gate_scratch,
                             scale=internal_qtype(qrec).scale, node=params)

        hr_gate_scratch = get_activation(params.activation_zr, False)(
            hr_gate_scratch, internal_qtype(qrec))

        DiagCollector.record('r_gate_sigmoid', hr_gate_scratch,
                             scale=scales['act'], node=params)

        if params.linear_before_reset:
            # haddamard after linear
            # r_gate_scratch = (rt (.) (Ht-1*(Rh^T) + Rbh))
            # h bias is in state_scale * h_w scale NOT Q12
            h_gate_recurrent = args['r_2_h_w'][0].astype(
                INT_DTYPE).dot(state_tensor) + args['r_h_b'][0]
            h_gate_recurrent = scale_to(
                qrec, "state_h_w_internal", h_gate_recurrent, 0)
            # now in Q12
            hr_gate_scratch = hr_gate_scratch * h_gate_recurrent
            DiagCollector.record(
                'hr_haddamard', hr_gate_scratch,
                scale=scales['act'] * math.pow(2, -internal_qtype(qrec).q),
                node=params)

            # now in Q12 + Q15
            # normalize to Q12
            hr_gate_scratch = at_norm(hr_gate_scratch, 15)

            # ht = g(Xt*(Wh^T) + (rt (.) (Ht-1*(Rh^T) + Rbh)) + Wbh) # when linear_before_reset != 0
            assert idx < params.n_input_cells
            # scale input_scale * h_input_weights_scale to Q12
            # h bias is in input_scale * h_w scale NOT Q12
            hr_gate_scratch += scale_to(
                qrec,
                "input_h_w_internal",
                args['w_2_h_w'][0].astype(INT_DTYPE).dot(
                    in_tensor) + args['w_h_b'][0],
                0)
        else:
            # haddamard on state before linear
            # r_gate_scratch = (rt (.) Ht-1)*(Rh^T) + Rbh + Wbh

            # Q15 * stateQ -> stateQ
            h_gate_recurrent = at_norm(
                state_tensor * hr_gate_scratch, 15)
            DiagCollector.record(
                'hr_haddamard', h_gate_recurrent,
                scale=scales['state'],
                node=params)

            h_gate_recurrent = args['r_2_h_w'][0].astype(
                INT_DTYPE).dot(h_gate_recurrent) + args['r_h_b'][0]
            hr_gate_scratch = scale_to(
                qrec, "state_h_w_internal", h_gate_recurrent, 0)

            assert idx < params.n_input_cells
            # scale input_scale * h_input_weights_scale to internal
            hr_gate_scratch += scale_to(
                qrec,
                "input_h_w_internal",
                args['w_2_h_w'][0].astype(INT_DTYPE).dot(
                    in_tensor) + args['w_h_b'][0],
                0)

        # outputs q15
        hr_gate_scratch = get_activation(params.activation, False)(
            hr_gate_scratch, internal_qtype(qrec))
        DiagCollector.record('hr_gate_tanh', hr_gate_scratch,
                             scale=scales['act'], node=params)

        # Ht = (1 - zt) (.) ht + zt (.) Ht-1
        # zt = (1 - Q15) * Q15 + Q15 * Q15 = Q30
        # >> 15 and clip

        # h state is in Q15 * 1 or Q14
        h_state = state_tensor.copy()
        state_q = args['h_state'][1].q
        if state_q == 14:
            h_state <<= 1

        h_state = (
            ((qrec.cache['act_qtype'].quantize(1) - z_gate_scratch) * hr_gate_scratch) +
            (z_gate_scratch * h_state)
        )
        DiagCollector.record('h_state_out_prenorm', h_state,
                             scale=math.pow(2, -30), node=params)

        if state_q == 14:
            h_state = at_norm(h_state, 16)
        else:
            h_state = at_norm(h_state, 15)
        h_state = qrec.out_qs[0].clip(h_state)
        DiagCollector.record('h_state_out', h_state,
                             scale=scales['state'], node=params)
        args['h_state'][0] = h_state.copy()
        return h_state


def scale_lstm_input_input(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_i_q', tensor, axis)


def scale_lstm_input_forget(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_f_q', tensor, axis)


def scale_lstm_input_cell(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_c_q', tensor, axis)


def scale_lstm_input_output(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'i_2_o_q', tensor, axis)


def scale_lstm_sum_input(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'gate_sum_i', tensor, axis)


def scale_lstm_sum_forget(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'gate_sum_f', tensor, axis)


def scale_lstm_sum_cell(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'gate_sum_c', tensor, axis)


def scale_lstm_sum_output(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'gate_sum_o', tensor, axis)


def scale_lstm_istate_input(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_i_q', tensor, axis)


def scale_lstm_istate_forget(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_f_q', tensor, axis)


def scale_lstm_istate_cell(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_c_q', tensor, axis)


def scale_lstm_istate_output(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_o_q', tensor, axis)


def scale_lstm_cellin(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'cell_in_q', tensor, axis)


def scale_lstm_cellout(qrec, tensor: np.ndarray, axis: int):
    external_type = qrec.in_qs[LSTMParameters.INPUT_NAMES.index('c_state')]
    return external_type.clip(scale_to(qrec, 'cell_out_q', tensor, axis))


def scale_lstm_output(qrec, tensor: np.ndarray, axis: int):
    return qrec.out_qs[0].clip(scale_to(qrec, 'state_out_q', tensor, axis))


@ params_type(LSTMParameters)
@ qrec_type('scaled')
class LSTMSymmetric(RnnSymmetricMixin, KernelBase):
    @ classmethod
    def step_kernel(cls, params: LSTMParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    qrec):
        if args['i_state'][1].dtype == np.int16:
            return cls.step_kernel16_8(params, args, idx, input_tensor, qrec)
        return cls.step_kernel8_8(params, args, idx, input_tensor, qrec)

    @ classmethod
    def step_kernel8_8(cls, params: LSTMParameters,
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

        # scales = qrec.cache['scales']
        # DiagCollector.record(
        #     'input', input_tensor[idx], scale=scales['in'][0], node=params)

        # INPUT vs WEIGHTS
        # For each cell: compute input_weight * input if there is an input
        input_gate_scratch = np.full([params.n_states], 0, dtype=INT_DTYPE)
        forget_gate_scratch = np.full([params.n_states], 0, dtype=INT_DTYPE)
        cell_scratch = np.full([params.n_states], 0, dtype=INT_DTYPE)
        output_gate_scratch = np.full([params.n_states], 0, dtype=INT_DTYPE)
        DiagCollector.record(
            'i_state', args['i_state'][0], scale=args['i_state'][1].scale, node=params)
        DiagCollector.record(
            'c_state', args['c_state'][0], scale=args['c_state'][1].scale, node=params)
        DiagCollector.record(
            'input', input_tensor[idx], scale=qrec.in_qs[0].scale, node=params)

        r_pscales = qrec.cache['r_pscales']
        i_pscales = qrec.cache['i_pscales']
        if idx < params.n_input_cells:
            input_gate_scratch += scale_lstm_input_input(qrec,
                                                         args['i_2_i_w'][0].astype(INT_DTYPE).dot(
                                                             input_tensor[idx].astype(INT_DTYPE)),
                                                         0)
            forget_gate_scratch += scale_lstm_input_forget(qrec,
                                                           args['i_2_f_w'][0].astype(INT_DTYPE).dot(
                                                               input_tensor[idx].astype(INT_DTYPE)),
                                                           0)
            cell_scratch += scale_lstm_input_cell(qrec,
                                                  args['i_2_c_w'][0].astype(INT_DTYPE).dot(
                                                      input_tensor[idx].astype(INT_DTYPE)),
                                                  0)
            output_gate_scratch += scale_lstm_input_output(qrec,
                                                           args['i_2_o_w'][0].astype(INT_DTYPE).dot(
                                                               input_tensor[idx].astype(INT_DTYPE)),
                                                           0)
            DiagCollector.record(
                'i_gate_i', input_gate_scratch, scale=i_pscales['i'], node=params)
            DiagCollector.record(
                'f_gate_i', forget_gate_scratch, scale=i_pscales['f'], node=params)
            DiagCollector.record(
                'c_gate_i', cell_scratch, scale=i_pscales['c'], node=params)
            DiagCollector.record(
                'o_gate_i', output_gate_scratch, scale=i_pscales['o'], node=params)

        # perceptron scaling
        # 16 bit act(scale(scale(i*iw) + scale(r*rw) + b)) - internal max_accum
        # 8 bit different input + i_state
        # scale(scale(i*iw) + r*rw + b) internal max(sum(r*w))
        # 8 bit same input and i_state
        # scale(i*iw + r*rw + b) internal - no scaling

        # For each cell: compute recurrent_weight * output_state
        input_gate_scratch_state = args['r_2_i_w'][0].astype(
            INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))
        forget_gate_scratch_state = args['r_2_f_w'][0].astype(
            INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))
        cell_gate_scratch_state = args['r_2_c_w'][0].astype(
            INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))
        output_gate_scratch_state = args['r_2_o_w'][0].astype(
            INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))

        DiagCollector.record(
            'i_gate_r', input_gate_scratch_state, scale=r_pscales['i'], node=params)
        DiagCollector.record(
            'f_gate_r', forget_gate_scratch_state, scale=r_pscales['f'], node=params)
        DiagCollector.record(
            'c_gate_r', cell_gate_scratch_state, scale=r_pscales['c'], node=params)
        DiagCollector.record(
            'o_gate_r', output_gate_scratch_state, scale=r_pscales['o'], node=params)

        # Add bias for regular lstm
        input_gate_scratch += args['i_b'][0].astype(
            INT_DTYPE).copy() + input_gate_scratch_state
        forget_gate_scratch += args['f_b'][0].astype(
            INT_DTYPE).copy() + forget_gate_scratch_state
        cell_scratch += args['c_b'][0].astype(INT_DTYPE).copy() + \
            cell_gate_scratch_state
        output_gate_scratch += args['o_b'][0].astype(
            INT_DTYPE).copy() + output_gate_scratch_state

        DiagCollector.record(
            'i_gate_post_bias', input_gate_scratch, scale=r_pscales['i'], node=params)
        DiagCollector.record(
            'f_gate_post_bias', forget_gate_scratch, scale=r_pscales['f'], node=params)
        DiagCollector.record(
            'c_gate_post_bias', cell_scratch, scale=r_pscales['c'], node=params)
        DiagCollector.record(
            'o_gate_post_bias', output_gate_scratch, scale=r_pscales['o'], node=params)

        input_gate_scratch = scale_lstm_istate_input(
            qrec, input_gate_scratch, 0)
        forget_gate_scratch = scale_lstm_istate_forget(
            qrec, forget_gate_scratch, 0)
        cell_scratch = scale_lstm_istate_cell(
            qrec, cell_scratch, 0)
        output_gate_scratch = scale_lstm_istate_output(
            qrec, output_gate_scratch, 0)

        int_qtype = internal_qtype(qrec)
        DiagCollector.record('i_gate', input_gate_scratch,
                             scale=r_pscales['int_scale'], node=params)
        DiagCollector.record('f_gate', forget_gate_scratch,
                             scale=r_pscales['int_scale'], node=params)
        DiagCollector.record('c_gate', cell_scratch,
                             scale=r_pscales['int_scale'], node=params)
        DiagCollector.record('o_gate', output_gate_scratch,
                             scale=r_pscales['int_scale'], node=params)

        # Apply activations in internal Q * 1
        input_gate_scratch = get_activation('sigmoid', params.hard_act)(
            input_gate_scratch, int_qtype)
        DiagCollector.record('i_gate_after_act', input_gate_scratch,
                             scale=r_pscales['act_out_scale'], node=params)

        forget_gate_scratch = get_activation('sigmoid', params.hard_act)(
            forget_gate_scratch, int_qtype)
        DiagCollector.record('f_gate_after_act', forget_gate_scratch,
                             scale=r_pscales['act_out_scale'], node=params)

        output_gate_scratch = get_activation('sigmoid', params.hard_act)(
            output_gate_scratch, int_qtype)
        DiagCollector.record('o_gate_after_act', output_gate_scratch,
                             scale=r_pscales['act_out_scale'], node=params)

        cell_scratch = get_activation('tanh', params.hard_act)(
            cell_scratch, int_qtype)
        DiagCollector.record('c_gate_after_act', cell_scratch,
                             scale=r_pscales['act_out_scale'], node=params)

        # cstate = cstate * Of + Og * Oi
        if params.hard_act:
            # Scale cell state * Of to internal Q * 2
            cstate = scale_lstm_cellin(
                qrec, args['c_state'][0].astype(INT_DTYPE) * forget_gate_scratch, 0)
            DiagCollector.record('cstate_cbar_f', cstate,
                                 scale=r_pscales['c_before_scale'], node=params)
            cstate_c_i = cell_scratch * input_gate_scratch
            DiagCollector.record('cstate_c_i', cstate_c_i,
                                 scale=r_pscales['c_before_scale'], node=params)
            cstate += cstate_c_i
            DiagCollector.record('c_state_before_scale', cstate,
                                 scale=r_pscales['c_before_scale'], node=params)
            # cstate now in (2 * Q) * 1
        else:
            # Multiply cstate [Scstate] * Of [Sq15] and scale to [Sq12]
            # Multiply Og [Sq15] * Oi [Sq15] --> [Sq30] >> 30-12 --> [Sq12]
            # cstate is now in q12 = internal_qtype
            cstate_cbar_f = scale_lstm_cellin(
                qrec,
                args['c_state'][0].astype(INT_DTYPE) * forget_gate_scratch,
                0)
            DiagCollector.record('cstate_cbar_f', cstate_cbar_f,
                                 scale=int_qtype.scale, node=params)
            cstate_c_i = at_norm(
                cell_scratch * input_gate_scratch,
                (30-int_qtype.q))
            DiagCollector.record('cstate_c_i', cstate_c_i,
                                 scale=int_qtype.scale, node=params)
            cstate = cstate_cbar_f + cstate_c_i
            DiagCollector.record('c_state_before_scale', cstate,
                                 scale=int_qtype.scale, node=params)

        # if params.cell_clip > 0.0:
        #     args['c_state'] = abs_clip(args['c_state'], params.cell_clip)
        # if there is a clip value this should override the min max here
        # clip here

        args['c_state'][0] = scale_lstm_cellout(qrec, cstate, 0)
        DiagCollector.record(
            'c_state_out', args['c_state'][0], scale=args['c_state'][1].scale, node=params)

        if params.hard_act:
            two_qtype = QType.Pow2(
                int_qtype.bits, int_qtype.q * 2, True)
            cell_scratch = get_activation(
                'tanh', params.hard_act)(cstate, two_qtype)
            # Assume scaling from internalq * 3 -> Q7 * 1
            output_gate_scratch *= cell_scratch
        else:
            cell_scratch = get_activation('tanh', params.hard_act)(
                cstate, int_qtype)
            # output = Og[Sq15] * tanh(cell_scratch)[Sq15] -> [Sq30] >> 15 -> [Sq15]
            output_gate_scratch = (output_gate_scratch * cell_scratch) >> 15

        output = scale_lstm_output(qrec, output_gate_scratch, 0)
        DiagCollector.record(
            'output', output, scale=qrec.out_qs[0].scale, node=params)

        use_projection_weight = 'proj_w' in args and args['proj_w'][0] is not None
        use_projection_bias = 'proj_b' in args and args['proj_b'][0] is not None

        if use_projection_weight or use_projection_bias:
            raise NotImplementedError("LSTMP is not yet supported by kernel")

        # args['i_state'][0] = qrec.scale_i_state(output_gate_scratch.copy(), 0, ktype="symmetric")
        args['i_state'][0] = output.copy()

        if params.lstm_output_c_state:
            return output, args['c_state'][0]
        return output, None

    @classmethod
    def step_kernel16_8(cls, params: LSTMParameters,
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

        # INPUT vs WEIGHTS
        # For each cell: compute input_weight * input if there is an input
        input_scratch = {k: np.full([params.n_states], 0, dtype=INT_DTYPE)
                         for k in ['i', 'f', 'c', 'o']}

        DiagCollector.record(
            'i_state', args['i_state'][0], scale=args['i_state'][1].scale, node=params)
        DiagCollector.record(
            'c_state', args['c_state'][0], scale=args['c_state'][1].scale, node=params)
        DiagCollector.record(
            'input', input_tensor[idx], scale=qrec.in_qs[0].scale, node=params)

        r_pscales = qrec.cache['r_pscales']
        i_pscales = qrec.cache['i_pscales']
        int_qtype = internal_qtype(qrec)

        if idx < params.n_input_cells:
            for k in ['i', 'f', 'c', 'o']:
                input_scratch[k] += args[f'i_2_{k}_w'][0].astype(INT_DTYPE).dot(
                    input_tensor[idx].astype(INT_DTYPE))
                input_scratch[k] = scale_to(
                    qrec, f'i_2_{k}_q', input_scratch[k], 0)
                DiagCollector.record(
                    f'{k}_gate_i', input_scratch[k], scale=int_qtype.scale, node=params)

        state_scratch = {}
        for k in ['i', 'f', 'c', 'o']:
            state_scratch[k] = args[f'r_2_{k}_w'][0].astype(
                INT_DTYPE).dot(args['i_state'][0].astype(INT_DTYPE))
            state_scratch[k] = scale_to(
                qrec, f'r_2_{k}_q', state_scratch[k], 0)
            DiagCollector.record(
                f'{k}_gate_r', state_scratch[k], scale=int_qtype.scale, node=params)

        for k in ['i', 'f', 'c', 'o']:
            # Add bias for regular lstm
            input_scratch[k] += args[f'{k}_b'][0].astype(
                INT_DTYPE).copy() + state_scratch[k]

            DiagCollector.record(f'{k}_gate', input_scratch[k],
                                 scale=int_qtype.scale, node=params)
            DiagCollector.record_ref(
                f'{k}_gate_post_bias', f'{k}_gate', node=params)

        # Apply activations in internal Q * 1
        for k in ['i', 'f', 'o']:
            input_scratch[k] = get_activation('sigmoid', False)(
                input_scratch[k], int_qtype)
            DiagCollector.record(f'{k}_gate_after_act', input_scratch[k],
                                 scale=r_pscales['act_out_scale'], node=params)

        input_scratch['c'] = get_activation('tanh', False)(
            input_scratch['c'], int_qtype)
        DiagCollector.record('c_gate_after_act', input_scratch['c'],
                             scale=r_pscales['act_out_scale'], node=params)

        # Multiply cstate [Scstate] * Of [Sq15] and scale to [Sq12]
        # Multiply Og [Sq15] * Oi [Sq15] --> [Sq30] >> 30-12 --> [Sq12]
        # cstate is now in q12 = internal_qtype

        # NOTE: for int16 scale apply 8 bit norm to product before mult_bias then norm - 8 in kernel
        # this is done by prenormalization in scaled qtype set by quantizer

        cstate_cbar_f = scale_lstm_cellin(
            qrec,
            args['c_state'][0].astype(INT_DTYPE) * input_scratch['f'],
            0)
        DiagCollector.record('cstate_cbar_f', cstate_cbar_f,
                             scale=int_qtype.scale, node=params)
        cstate_c_i = at_norm(
            input_scratch['c'] * input_scratch['i'],
            (30-int_qtype.q))
        DiagCollector.record('cstate_c_i', cstate_c_i,
                             scale=int_qtype.scale, node=params)
        cstate = cstate_cbar_f + cstate_c_i

        DiagCollector.record('c_state_before_scale', cstate,
                             scale=int_qtype.scale, node=params)

        # if params.cell_clip > 0.0:
        #     args['c_state'] = abs_clip(args['c_state'], params.cell_clip)
        # if there is a clip value this should override the min max here
        # clip here

        args['c_state'][0] = scale_lstm_cellout(qrec, cstate, 0)
        DiagCollector.record(
            'c_state_out', args['c_state'][0], scale=args['c_state'][1].scale, node=params)

        cell_gate_scratch = get_activation('tanh', False)(
            cstate, int_qtype)
        output_gate_scratch = (input_scratch['o'] * cell_gate_scratch)

        output = scale_lstm_output(qrec, output_gate_scratch, 0)
        DiagCollector.record(
            'output', output, scale=qrec.out_qs[0].scale, node=params)

        use_projection_weight = 'proj_w' in args and args['proj_w'][0] is not None
        use_projection_bias = 'proj_b' in args and args['proj_b'][0] is not None

        if use_projection_weight or use_projection_bias:
            raise NotImplementedError("LSTMP is not yet supported by kernel")

        # args['i_state'][0] = qrec.scale_i_state(output_gate_scratch.copy(), 0, ktype="symmetric")
        args['i_state'][0] = output.copy()

        if params.lstm_output_c_state:
            return output, args['c_state'][0]
        return output, None
