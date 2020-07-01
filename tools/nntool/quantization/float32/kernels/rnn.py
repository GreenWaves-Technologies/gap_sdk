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
from typing import Sequence, Mapping
import numpy as np

from graph.types import RNNParameters, LSTMParameters, RNNBaseParameters
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.quantization_record_base import QuantizationRecordBase

LOG = logging.getLogger("nntool." + __name__)


def abs_clip(arr: np.ndarray, abs_limit):
    return np.clip(arr, -abs_limit, abs_limit)


def relu(x):
    return np.minimum(x, 0)


def sigmoid(x):
    pos_mask = (x >= 0)
    neg_mask = (x < 0)
    z = np.zeros_like(x)
    z[pos_mask] = np.exp(-x[pos_mask])
    z[neg_mask] = np.exp(x[neg_mask])
    top = np.ones_like(x)
    top[neg_mask] = z[neg_mask]
    return top / (1 + z)


def mean_stddev_normalization(arr: np.ndarray):
    mean = np.mean(arr)
    variance = np.sum(np.square(arr - mean)) / arr.size()
    stddev_inv = 1.0 / np.sqrt(variance + 1e-8)
    return (arr - mean) * stddev_inv

def htanh(x):
    return np.minimum(np.maximum(x, -1.0), 1.0)

def tanh(x):
    return np.tanh(x)

ACTIVATIONS = {
    "relu": relu,
    "sigmoid": sigmoid,
    "tanh": tanh
}


def rnn_step(params: RNNParameters,
             args: Mapping[str, np.ndarray],
             idx: int,
             input_tensor: np.ndarray,
             details=None):

    input_gate_scratch = args['i_b'].copy()

    # These two sections could be combined by stacking the weights horizontally
    # and the input and state vertically

    # For each cell: compute input_weight * input if there is an input
    if idx < params.n_input_cells:
        input_gate_scratch += args['i_2_i_w'].dot(input_tensor[idx])

    # For each cell: compute recurrent_weight * output_state
    input_gate_scratch += args['r_2_i_w'].dot(args['i_state'])

    input_gate_scratch = ACTIVATIONS[params.activation](input_gate_scratch)
    args['i_state'] = input_gate_scratch.copy()
    return input_gate_scratch


def lstm_step(params: LSTMParameters,
              args: Mapping[str, np.ndarray],
              idx: int,
              input_tensor: np.ndarray,
              details=None):

    use_cifg = args.get('i_2_i_w') is None
    use_peephole = args.get('c_2_o_w') is not None
    use_layer_norm = args.get('f_norm') is not None

    # Initialize scratch buffers with bias for regular lstm or initialize with
    # zero for layer norm lstm.
    if use_layer_norm:
        if not use_cifg:
            input_gate_scratch = np.zeros([params.n_cells])
        forget_gate_scratch = np.zeros([params.n_cells])
        cell_scratch = np.zeros([params.n_cells])
        output_gate_scratch = np.zeros([params.n_cells])
    else:
        if not use_cifg:
            input_gate_scratch = args['i_b'].copy()

        forget_gate_scratch = args['f_b'].copy()
        cell_scratch = args['c_b'].copy()
        output_gate_scratch = args['o_b'].copy()

    # These two sections could be combined by stacking the weights horizontally
    # and the input and state vertically

    # For each cell: compute input_weight * input if there is an input
    if idx < params.n_input_cells:
        if not use_cifg:
            input_gate_scratch += args['i_2_i_w'].dot(input_tensor[idx])
        # in = ScaleW * ScaleI
        forget_gate_scratch += args['i_2_f_w'].dot(input_tensor[idx])
        cell_scratch += args['i_2_c_w'].dot(input_tensor[idx])
        output_gate_scratch += args['i_2_o_w'].dot(input_tensor[idx])

    # For each cell: compute recurrent_weight * output_state
    if not use_cifg:
        input_gate_scratch += args['r_2_i_w'].dot(args['i_state'])
    # in + ScaleRW * ScaleOState
    forget_gate_scratch += args['r_2_f_w'].dot(args['i_state'])
    cell_scratch += args['r_2_c_w'].dot(args['i_state'])
    output_gate_scratch += args['r_2_o_w'].dot(args['i_state'])

    # For each cell: update input gate.
    if not use_cifg:
        if use_peephole:
            input_gate_scratch += args['c_2_i_w'] * args['c_state']
        if use_layer_norm:
            input_gate_scratch = mean_stddev_normalization(input_gate_scratch)
            input_gate_scratch *= args['i_norm']
            input_gate_scratch += args['i_b']
        input_gate_scratch = sigmoid(input_gate_scratch)
        # sigmoid at scale

    # For each cell: update forget gate.
    if use_peephole:
        forget_gate_scratch += args['c_2_f_w'] * args['c_state']

    if use_layer_norm:
        forget_gate_scratch = mean_stddev_normalization(forget_gate_scratch)
        forget_gate_scratch *= args['f_norm']
        forget_gate_scratch += args['f_b']

    forget_gate_scratch = sigmoid(forget_gate_scratch)

    # For each cell: update the cell.
    args['c_state'] *= forget_gate_scratch

    if use_layer_norm:
        cell_scratch = mean_stddev_normalization(cell_scratch)
        cell_scratch *= args['c_norm']
        cell_scratch += args['c_b']

    cell_scratch = tanh(cell_scratch)

    if use_cifg:
        forget_gate_scratch = 1.0 - forget_gate_scratch
        args['c_state'] += cell_scratch * forget_gate_scratch
    else:
        args['c_state'] += cell_scratch * input_gate_scratch

    if params.cell_clip > 0.0:
        args['c_state'] = abs_clip(args['c_state'], params.cell_clip)

    # For each cell: update the output gate.
    if use_peephole:
        output_gate_scratch += args['c_2_o_w'] * args['c_state']

    if use_layer_norm:
        output_gate_scratch = mean_stddev_normalization(output_gate_scratch)
        output_gate_scratch *= args['o_norm']
        output_gate_scratch += args['o_b']

    output_gate_scratch = sigmoid(output_gate_scratch)
    cell_scratch = tanh(args['c_state'])
    output_gate_scratch *= cell_scratch

    use_projection_weight = args.get('proj_w') is not None
    use_projection_bias = args.get('proj_b') is not None

    if use_projection_weight or use_projection_bias:
        raise NotImplementedError("LSTMP is not yet supported by kernel")

    args['i_state'] = output_gate_scratch.copy()
    return output_gate_scratch


STEP_KERNEL = {
    RNNParameters: rnn_step,
    LSTMParameters: lstm_step
}


def rnn(params: RNNBaseParameters,
        in_tensors: Sequence[np.ndarray],
        qrec: QuantizationRecordBase,
        details=None):

    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    args = {params.INPUT_NAMES[idx]: in_tensors[idx] for idx in range(1, len(in_tensors))}
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    assert in_tensor.shape[0] == params.n_input_cells, "input shape incorrect - n_input_cells"
    assert in_tensor.shape[1] == params.n_inputs, "input shape incorrect - n_inputs"
    out_tensor = np.zeros([params.n_output_cells, params.n_states])
    out_idx = 0
    if details is not None:
        details['range_state'] = {'min': float('inf'), 'max': float('-inf')}
        if isinstance(params, LSTMParameters):
            details['range_cell'] = {'min': float('inf'), 'max': float('-inf')}

    step_kernel = STEP_KERNEL[params.__class__]
    for idx in range(params.n_cells):
        res = step_kernel(params, args, idx, in_tensor, details=details)
        if idx >= (params.n_cells - params.n_output_cells):
            out_tensor[out_idx] = res
            out_idx += 1

        if details is not None:
            details['range_state']['min'] = min(details['range_state']['min'], res.min())
            details['range_state']['max'] = max(details['range_state']['max'], res.max())
            if isinstance(params, LSTMParameters):
                details['range_cell']['min'] = min(details['range_cell']['min'], args['c_state'].min())
                details['range_cell']['max'] = max(details['range_cell']['max'], args['c_state'].max())

    if params.transpose_out:
        out_tensor = np.transpose(out_tensor, params.transpose_out[0])
    return [out_tensor]
