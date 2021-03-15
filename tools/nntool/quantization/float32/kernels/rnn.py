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
from utils.diag_collector import DiagCollector

import numpy as np
from graph.types import LSTMParameters, RNNParameters
from graph.types.rnn import GRUParameters
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              quantization)
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

class RnnFloat32Mixin():
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        if qrec is None:
            qrec = Float32QuantizationRecord()
        details = kwargs['details']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
        args = {params.INPUT_NAMES[idx]: in_tensors[idx] for idx in range(1, len(in_tensors))}
        if params.always_reset_state:
            for state_key in params.STATE_PARAMETERS:
                args[state_key] = args[state_key].copy()
        assert in_tensor.shape[0] == params.n_input_cells, "input shape incorrect - n_input_cells"
        assert in_tensor.shape[1] == params.n_inputs, "input shape incorrect - n_inputs"
        if params.revert:
            in_tensor = np.flip(in_tensor, axis=0)
        out_tensor = np.zeros([params.n_output_cells, params.n_states])
        out_idx = 0
        if details is not None:
            details['range_state'] = {'min': float('inf'), 'max': float('-inf')}
            if isinstance(params, LSTMParameters):
                details['range_cell'] = {'min': float('inf'), 'max': float('-inf')}

        new_c_state = None
        for idx in range(params.n_cells):
            if isinstance(params, LSTMParameters):
                res, new_c_state = cls.step_kernel(params, args, idx, in_tensor, details=details)
            else:
                res = cls.step_kernel(params, args, idx, in_tensor, details=details)
            if idx >= (params.n_cells - params.n_output_cells):
                out_tensor[out_idx] = res
                out_idx += 1

            if details is not None:
                details['range_state']['min'] = min(details['range_state']['min'], res.min())
                details['range_state']['max'] = max(details['range_state']['max'], res.max())
                if isinstance(params, LSTMParameters):
                    details['range_cell']['min'] = min(
                        details['range_cell']['min'], args['c_state'].min())
                    details['range_cell']['max'] = max(
                        details['range_cell']['max'], args['c_state'].max())

        if params.revert:
            out_tensor = np.flip(out_tensor, axis=0)
        if params.output_directions:
            out_tensor = np.expand_dims(out_tensor, 0)
        if new_c_state is not None:
            return [out_tensor, new_c_state]
        return [out_tensor]

@params_type(RNNParameters)
@quantization('float32')
class RNNFloat32(RnnFloat32Mixin, KernelBase):
    @classmethod
    def step_kernel(cls, params: RNNParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    **kwargs):

        del kwargs
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

@params_type(GRUParameters)
@quantization('float32')
class GRUFloat32(RnnFloat32Mixin, KernelBase):
    @classmethod
    def step_kernel(cls, params: GRUParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    **kwargs):

        del kwargs
        DiagCollector.record('h_state', args['h_state'], node=params)
        DiagCollector.record('input', input_tensor[idx], node=params)
        z_gate_scratch = args['z_b'].copy()
        r_gate_scratch = args['r_b'].copy()

        DiagCollector.record('z_weigths', args['w_2_z_w'], node=params)
        if idx < params.n_input_cells:
            z_gate_scratch += args['w_2_z_w'].dot(input_tensor[idx])
            r_gate_scratch += args['w_2_r_w'].dot(input_tensor[idx])
            DiagCollector.record('z_gate_inp', z_gate_scratch, node=params)
            DiagCollector.record('r_gate_inp', r_gate_scratch, node=params)

        # zt = f(Xt*(Wz^T) + Ht-1*(Rz^T) + Wbz + Rbz)
        z_gate_scratch += args['r_2_z_w'].dot(args['h_state'])
        DiagCollector.record('z_gate', z_gate_scratch, node=params)
        z_gate_scratch = sigmoid(z_gate_scratch)
        DiagCollector.record('z_gate_sigmoid', z_gate_scratch, node=params)
        # rt = f(Xt*(Wr^T) + Ht-1*(Rr^T) + Wbr + Rbr)
        r_gate_scratch += args['r_2_r_w'].dot(args['h_state'])
        DiagCollector.record('r_gate', r_gate_scratch, node=params)
        r_gate_scratch = sigmoid(r_gate_scratch)
        DiagCollector.record('r_gate_sigmoid', r_gate_scratch, node=params)
        if params.linear_before_reset:
            # ht = g(Xt*(Wh^T) + (rt (.) (Ht-1*(Rh^T) + Rbh)) + Wbh) # when linear_before_reset != 0
            # h_gate_scratch already has Wbh in it
            h_gate_recurrent = (args['r_2_h_w'].dot(args['h_state']) + args['r_h_b'])
            h_gate_scratch = h_gate_recurrent * r_gate_scratch
        else:
            # ht = g(Xt*(Wh^T) + (rt (.) Ht-1)*(Rh^T) + Rbh + Wbh) # default, when linear_before_reset = 0
            # Rbh + Wbh is done above
            h_gate_scratch = args['r_2_h_w'].dot(args['h_state'] * r_gate_scratch) + args['r_h_b']

        h_gate_scratch += args['w_h_b']
        if idx < params.n_input_cells:
            h_gate_scratch += args['w_2_h_w'].dot(input_tensor[idx])
        h_gate_scratch = tanh(h_gate_scratch)

        # Ht = (1 - zt) (.) ht + zt (.) Ht-1
        args['h_state'] = (1 - z_gate_scratch) * h_gate_scratch + z_gate_scratch * args['h_state']

        return args['h_state'].copy()


@params_type(LSTMParameters)
@quantization('float32')
class LSTMFloat32(RnnFloat32Mixin, KernelBase):
    @classmethod
    def step_kernel(cls, params: LSTMParameters,
                    args: Mapping[str, np.ndarray],
                    idx: int,
                    input_tensor: np.ndarray,
                    **kwargs):

        del kwargs
        use_cifg = args.get('i_2_i_w') is None
        use_peephole = args.get('c_2_o_w') is not None
        use_layer_norm = args.get('f_norm') is not None

        # Initialize scratch buffers with bias for regular lstm or initialize with
        # zero for layer norm lstm.
        input_gate_scratch = None

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
        if params.lstm_output_c_state:
            return output_gate_scratch, args['c_state']
        return output_gate_scratch, None
