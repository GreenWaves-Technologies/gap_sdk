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
    def step_kernel(cls, params: RNNParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    qrec):

        # These two sections could be combined by stacking the weights horizontally
        # and the input and state vertically

        # For each cell: compute input_weight * input if there is an input
        if idx < params.n_input_cells:
            # scale result to recurrent_weight * input_state scale
            input_gate_scratch = scale_rnn_input(
                qrec,
                args['i_2_i_w'][0].astype(np.int32).dot(
                    input_tensor[idx].astype(np.int32)),
                0)
        # biases already in recurrent_weight * input_state scale
        input_gate_scratch_state = args['i_b'][0].copy()

        # For each cell: compute recurrent_weight * input_state
        input_gate_scratch_state += args['r_2_i_w'][0].astype(
            np.int32).dot(args['i_state'][0].astype(np.int32))

        # scale to state scale
        input_gate_scratch = scale_rnn_state(qrec,
                                             input_gate_scratch+input_gate_scratch_state,
                                             0)

        # apply activation at state scale
        input_gate_scratch = get_activation(params.activation, params.hard_act)(
            input_gate_scratch, args['i_state'][1])

        # scale the state scale to the output scale
        output_gate_scratch = scale_rnn_output(qrec, input_gate_scratch, 0)
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

        z_gate_scratch = 0
        hr_gate_scratch = 0
        scales = qrec.cache['scales']
        DiagCollector.record(
            'h_state', args['h_state'][0], scale=scales['state'], node=params)
        DiagCollector.record(
            'input', input_tensor[idx], scale=scales['in'][0], node=params)

        DiagCollector.record('z_weigths', args['w_2_z_w'][0],
                             scale=scales['r_2_z_w'], node=params)
        if idx < params.n_input_cells:
            # calculate z gate on input
            z_gate_scratch += args['w_2_z_w'][0].astype(
                np.int32).dot(input_tensor[idx])
            # calculate r gate on input
            hr_gate_scratch += args['w_2_r_w'][0].astype(
                np.int32).dot(input_tensor[idx])
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
            np.int32).dot(args['h_state'][0]) + args['z_b'][0].copy()
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
            np.int32).dot(args['h_state'][0]) + args['r_b'][0].copy()
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
                np.int32).dot(args['h_state'][0]) + args['r_h_b'][0]
            # this is int_q_scale * state_q_scale * h_recurrent_weights_scale
            hr_gate_scratch = hr_gate_scratch * h_gate_recurrent
            # normalize to state_q_scale * h_recurrent_weights_scale
            hr_gate_scratch = at_norm(hr_gate_scratch, internal_qtype(qrec).q)

            # ht = g(Xt*(Wh^T) + (rt (.) (Ht-1*(Rh^T) + Rbh)) + Wbh) # when linear_before_reset != 0
            if idx < params.n_input_cells:
                if not params.rnn_same_inout_scale:
                    # scale input_scale * h_input_weights_scale to state_q_scale * h_recurrent_weights_scale
                    hr_gate_scratch += scale_gru_h_input2_h_HtxW(qrec,
                                                                 (args['w_2_h_w'][0].astype(np.int32).dot(
                                                                     input_tensor[idx]) + args['w_h_b'][0]),
                                                                 0)
                else:
                    # since input_scale == state scale and h_input_weights_scale == h_recurrent_weights_scale
                    # no scaling is necessary
                    hr_gate_scratch += args['w_2_h_w'][0].astype(
                        np.int32).dot(input_tensor[idx]) + args['w_h_b'][0]
            else:
                # Is this correct if there is no input (and below)? This is not a mode that
                # exists in any framework and will not ever be used at present
                if not params.rnn_same_inout_scale:
                    hr_gate_scratch += qrec.scale_h_input2_h_HtxW(
                        args['w_h_b'][0], 0)
                else:
                    hr_gate_scratch += args['w_h_b'][0]
        else:
            # haddamard on state before linear
            # r_gate_scratch = (rt (.) Ht-1)*(Rh^T) + Rbh + Wbh

            # this is int_q_scale * state_q_scale * h_recurrent_weights_scale
            # normalize to state_q_scale * h_recurrent_weights_scale
            hr_gate_scratch = at_norm(args['r_2_h_w'][0].astype(np.int32).dot(
                args['h_state'][0] * hr_gate_scratch), internal_qtype(qrec).q) + args['r_h_b'][0]

            if idx < params.n_input_cells:
                if not params.rnn_same_inout_scale:
                    # scale input_scale * h_input_weights_scale to state_q_scale * h_recurrent_weights_scale
                    hr_gate_scratch += qrec.scale_h_input_2_h_HtxW(
                        args['w_2_h_w'][0].dot(input_tensor[idx]) + args['w_h_b'][0], 0)
                else:
                    hr_gate_scratch += args['w_2_h_w'][0].astype(
                        np.int32).dot(input_tensor[idx]) + args['w_h_b'][0]
            else:
                if not params.rnn_same_inout_scale:
                    hr_gate_scratch += qrec.scale_h_input2_h_HtxW(
                        args['w_h_b'][0], 0)
                else:
                    hr_gate_scratch += args['w_h_b'][0]

        # scale to q15 or internal Q depending on activation type
        hr_gate_scratch = get_activation(params.activation, params.hard_act)(
            scale_gru_h_internal(qrec, hr_gate_scratch, 0), internal_qtype(qrec))
        # if not hard then go from Q15 -> int_q
        if not params.hard_act and internal_qtype(qrec).q != 15:
            hr_gate_scratch = at_norm(
                hr_gate_scratch, 15 - internal_qtype(qrec).q)

        # ----------- SCALE Q7 -----------

        # Ht = (1 - zt) (.) ht + zt (.) Ht-1
        # zt = (1 - int_q) * Q7 + Q7 * Q7 = INT_Q * 2
        # >> and clip

        h_state = (args['h_state'][0].copy()).astype(
            np.int32) << (internal_qtype(qrec).q - 7)
        h_state = qrec.out_qs[0].clip(at_norm((internal_qtype(qrec).quantize(1) - z_gate_scratch) *
                                              hr_gate_scratch + z_gate_scratch * h_state,
                                              (internal_qtype(qrec).q * 2) - 7)).astype(qrec.out_qs[0].dtype)
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


def scale_lstm_internal_input(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_i_q', tensor, axis)


def scale_lstm_internal_forget(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_f_q', tensor, axis)


def scale_lstm_internal_cell(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_c_q', tensor, axis)


def scale_lstm_internal_output(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'r_2_o_q', tensor, axis)


def scale_lstm_cellin(qrec, tensor: np.ndarray, axis: int):
    return scale_to(qrec, 'cell_in_q', tensor, axis)


def scale_lstm_cellout(qrec, tensor: np.ndarray, axis: int):
    external_type = qrec.cache.get('external_type') or QType(
        bits=8, q=0, signed=True)
    return external_type.clip(scale_to(qrec, 'cell_out_q', tensor, axis))


def scale_lstm_output(qrec, tensor: np.ndarray, axis: int):
    external_type = qrec.cache.get('external_type') or QType(
        bits=8, q=0, signed=True)
    return external_type.clip(scale_to(qrec, 'state_out_q', tensor, axis))


@params_type(LSTMParameters)
@qrec_type('scaled')
class LSTMSymmetric(RnnSymmetricMixin, KernelBase):
    @classmethod
    def step_kernel(cls, params: LSTMParameters,
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
        input_gate_scratch = 0
        forget_gate_scratch = 0
        cell_scratch = 0
        output_gate_scratch = 0
        if idx < params.n_input_cells:
            input_gate_scratch += scale_lstm_input_input(qrec,
                                                         args['i_2_i_w'][0].astype(np.int32).dot(
                                                             input_tensor[idx].astype(np.int32)),
                                                         0)
            forget_gate_scratch += scale_lstm_input_forget(qrec,
                                                           args['i_2_f_w'][0].astype(np.int32).dot(
                                                               input_tensor[idx].astype(np.int32)),
                                                           0)
            cell_scratch += scale_lstm_input_cell(qrec,
                                                  args['i_2_c_w'][0].astype(np.int32).dot(
                                                      input_tensor[idx].astype(np.int32)),
                                                  0)
            output_gate_scratch += scale_lstm_input_output(qrec,
                                                           args['i_2_o_w'][0].astype(np.int32).dot(
                                                               input_tensor[idx].astype(np.int32)),
                                                           0)

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
        cell_scratch_state += args['r_2_c_w'][0].astype(
            np.int32).dot(args['i_state'][0].astype(np.int32))
        output_gate_scratch_state += args['r_2_o_w'][0].astype(
            np.int32).dot(args['i_state'][0].astype(np.int32))

        input_gate_scratch = scale_lstm_internal_input(
            qrec, input_gate_scratch_state+input_gate_scratch, 0)
        forget_gate_scratch = scale_lstm_internal_forget(
            qrec, forget_gate_scratch_state+forget_gate_scratch, 0)
        cell_scratch = scale_lstm_internal_cell(
            qrec, cell_scratch_state+cell_scratch, 0)
        output_gate_scratch = scale_lstm_internal_output(
            qrec, output_gate_scratch_state+output_gate_scratch, 0)

        # Apply activations in internal Q * 1
        input_gate_scratch = get_activation('sigmoid', params.hard_act)(
            input_gate_scratch, internal_qtype(qrec))

        forget_gate_scratch = get_activation('sigmoid', params.hard_act)(
            forget_gate_scratch, internal_qtype(qrec))

        output_gate_scratch = get_activation('sigmoid', params.hard_act)(
            output_gate_scratch, internal_qtype(qrec))

        cell_scratch = get_activation('tanh', params.hard_act)(
            cell_scratch, internal_qtype(qrec))

        # cstate = cstate * Of + Og * Oi
        if params.hard_act:
            # Scale cell state to internal Q * 1
            cstate = scale_lstm_cellin(
                qrec, args['c_state'][0].astype(np.int32), 0)
            cstate = cstate * forget_gate_scratch + cell_scratch * input_gate_scratch
            # cstate now in (2 * Q) * 1
        else:
            # Multiply cstate [Scstate] * Of [Sq15] and scale to [Sq12]
            # Multiply Og [Sq15] * Oi [Sq15] --> [Sq30] >> 30-12 --> [Sq12]
            # cstate is now in q12 = internal_qtype
            cstate = scale_lstm_cellin(qrec, args['c_state'][0] * forget_gate_scratch, 0) \
                + ((cell_scratch * input_gate_scratch)
                   >> (15+(15-internal_qtype(qrec).q)))

        # if params.cell_clip > 0.0:
        #     args['c_state'] = abs_clip(args['c_state'], params.cell_clip)
        # if there is a clip value this should override the min max here
        # clip here

        args['c_state'][0] = scale_lstm_cellout(qrec, cstate, 0)

        if params.hard_act:
            two_qtype = QType.Pow2(
                internal_qtype(qrec).bits, internal_qtype(qrec).q * 2, True)
            cell_scratch = get_activation(
                'tanh', params.hard_act)(cstate, two_qtype)
            # Assume scaling from internalq * 3 -> Q7 * 1
            output_gate_scratch *= cell_scratch
        else:
            cell_scratch = get_activation('tanh', params.hard_act)(
                cstate, internal_qtype(qrec))
            # output = Og[Sq15] * tanh(cell_scratch)[Sq15] -> [Sq30] >> 15 -> [Sq15]
            output_gate_scratch = (output_gate_scratch * cell_scratch) >> 15

        output = scale_lstm_output(qrec, output_gate_scratch, 0)
        output = qrec.out_qs[0].clip(output)

        use_projection_weight = 'proj_w' in args and args['proj_w'][0] is not None
        use_projection_bias = 'proj_b' in args and args['proj_b'][0] is not None

        if use_projection_weight or use_projection_bias:
            raise NotImplementedError("LSTMP is not yet supported by kernel")

        #args['i_state'][0] = qrec.scale_i_state(output_gate_scratch.copy(), 0, ktype="symmetric")
        args['i_state'][0] = output.copy()
        if params.lstm_output_c_state:
            return output, args['c_state'][0]
        return output, None
