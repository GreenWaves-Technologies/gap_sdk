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


ACTIVATIONS = {
    "relu": relu,
    "sigmoid": sigmoid,
    "tanh": htanh
}


def rnn_step(params: RNNParameters,
             args: Mapping[str, Sequence],
             idx: int,
             input_tensor: np.ndarray,
             qrec):

    # biases already in recurrent_weight * input_state scale
    input_gate_scratch = args['i_b'][0].copy()

    # These two sections could be combined by stacking the weights horizontally
    # and the input and state vertically

    # For each cell: compute input_weight * input if there is an input
    if idx < params.n_input_cells:
        # scale result to recurrent_weight * input_state scale
        input_gate_scratch += qrec.scale_input(
            params,
            args['i_2_i_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            args,
            ktype="symmetric")

    # For each cell: compute recurrent_weight * input_state
    input_gate_scratch += args['r_2_i_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))

    # scale to state scale
    input_gate_scratch = qrec.scale_state(
        params,
        input_gate_scratch,
        0,
        args,
        ktype="symmetric")

    # apply activation at state scale
    input_gate_scratch = ACTIVATIONS[params.activation](input_gate_scratch, args['i_state'][1])
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

    # Initialize scratch buffers with bias for regular lstm
    input_gate_scratch = args['i_b'][0].astype(np.int32)
    forget_gate_scratch = args['f_b'][0].astype(np.int32)
    cell_scratch = args['c_b'][0].astype(np.int32)
    output_gate_scratch = args['o_b'][0].astype(np.int32)

    # These two sections could be combined by stacking the weights horizontally
    # and the input and state vertically

    # For each cell: compute input_weight * input if there is an input
    if idx < params.n_input_cells:
        input_gate_scratch += qrec.scale_input_input(
            args['i_2_i_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")
        forget_gate_scratch += qrec.scale_input_forget(
            args['i_2_f_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")
        cell_scratch += qrec.scale_input_cell(
            args['i_2_c_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")
        output_gate_scratch += qrec.scale_input_output(
            args['i_2_o_w'][0].astype(np.int32).dot(input_tensor[idx].astype(np.int32)),
            0,
            ktype="symmetric")

    # For each cell: compute recurrent_weight * output_state
    input_gate_scratch += args['r_2_i_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))
    forget_gate_scratch += args['r_2_f_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))
    cell_scratch += args['r_2_c_w'][0].astype(np.int32).dot(args['i_state'][0].astype(np.int32))
    output_gate_scratch += args['r_2_o_w'][0].astype(
        np.int32).dot(args['i_state'][0].astype(np.int32))

    input_gate_scratch = qrec.scale_internal_input(input_gate_scratch,
                                                   0, ktype="symmetric")
    forget_gate_scratch = qrec.scale_internal_forget(forget_gate_scratch,
                                                     0, ktype="symmetric")
    cell_scratch = qrec.scale_internal_cell(cell_scratch,
                                            0, ktype="symmetric")
    output_gate_scratch = qrec.scale_internal_output(output_gate_scratch,
                                                     0, ktype="symmetric")

    # Apply activations in internal Q * 1
    input_gate_scratch = hsigmoid(input_gate_scratch, qrec.internal_qtype)

    forget_gate_scratch = hsigmoid(forget_gate_scratch, qrec.internal_qtype)

    output_gate_scratch = hsigmoid(output_gate_scratch, qrec.internal_qtype)

    cell_scratch = htanh(cell_scratch, qrec.internal_qtype)

    # Scale cell state to internal Q * 1
    cstate = qrec.scale_cellin(args['c_state'][0].astype(np.int32), 0, ktype="symmetric")

    cstate = cstate * forget_gate_scratch + cell_scratch * input_gate_scratch

    # cstate now in (2 * Q) * 1

    # if params.cell_clip > 0.0:
    #     args['c_state'] = abs_clip(args['c_state'], params.cell_clip)
    # if there is a clip value this should override the min max here
    # clip here

    args['c_state'][0] = qrec.scale_cellout(cstate,
                                            0, ktype="symmetric")
    two_qtype = QType(qrec.internal_qtype, q = qrec.internal_qtype.q * 2)
    cell_scratch = htanh(cstate, two_qtype)

    output_gate_scratch *= cell_scratch
    # Assume scaling from internalq * 3 -> Q7 * 1
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
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
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

    if params.transpose_out:
        out_tensor = np.transpose(out_tensor, params.transpose_out[0])
    return [out_tensor]
