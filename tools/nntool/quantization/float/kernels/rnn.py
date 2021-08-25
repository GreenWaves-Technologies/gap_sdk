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
from quantization.new_qrec import AllFloatQRec, QRec
from utils.diag_collector import DiagCollector

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


def init_stats(details, *names):
    for name in names:
        details[name] = {
            'min': float('inf'), 'max': float('-inf')}


def record_stat(details, name, val):
    details[name]['min'] = min(
        details[name]['min'], val.min())
    details[name]['max'] = max(
        details[name]['max'], val.max())


class RnnFloat32Mixin():
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        details = kwargs['details']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        args = {params.INPUT_NAMES[idx]: in_tensors[idx]
                for idx in range(1, len(in_tensors))}
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
            init_stats(
                details,
                'range_state')
            if isinstance(params, LSTMParameters):
                DiagCollector.active_set('__rnn_quant')
                init_stats(
                    details,
                    'range_cell',
                )

        new_c_state = None
        for idx in range(params.n_cells):
            if isinstance(params, LSTMParameters):
                res, new_c_state = cls.step_kernel(
                    params, args, idx, in_tensor, details=details)
            else:
                res = cls.step_kernel(
                    params, args, idx, in_tensor, details=details)
            if idx >= (params.n_cells - params.n_output_cells):
                out_tensor[out_idx] = res
                out_idx += 1

            if details is not None:
                record_stat(details, 'range_state', res)
                if isinstance(params, LSTMParameters):
                    record_stat(details, 'range_cell', args['c_state'])
        
        if details is not None and isinstance(params, LSTMParameters):
            DiagCollector.store_ranges(
                details,
                '__rnn_quant',
                'i_gate_i',
                'c_gate_i',
                'f_gate_i',
                'o_gate_i',
                'i_gate_r',
                'c_gate_r',
                'f_gate_r',
                'o_gate_r',
                'i_gate',
                'c_gate',
                'f_gate',
                'o_gate',
            )
            DiagCollector.deactivate()
            DiagCollector.clear(set_name='__rnn_quant')

        if params.revert:
            out_tensor = np.flip(out_tensor, axis=0)
        if params.output_directions:
            out_tensor = np.expand_dims(out_tensor, 0)
        if new_c_state is not None:
            return [out_tensor, new_c_state]
        return [out_tensor]


@params_type(RNNParameters)
@qrec_type('float')
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
        DiagCollector.record(
            'h_state_out', input_gate_scratch, node=params)
        DiagCollector.record_ref(
            'h_state_prescale', 'h_state_out', node=params)

        args['i_state'] = input_gate_scratch.copy()
        return input_gate_scratch


@params_type(GRUParameters)
@qrec_type('float')
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
            h_gate_recurrent = (args['r_2_h_w'].dot(
                args['h_state']) + args['r_h_b'])
            DiagCollector.record(
                'h_gate_state', h_gate_recurrent,
                node=params)
            h_gate_scratch = h_gate_recurrent * r_gate_scratch
            DiagCollector.record(
                'hr_haddamard', h_gate_scratch,
                node=params)

        else:
            # ht = g(Xt*(Wh^T) + (rt (.) Ht-1)*(Rh^T) + Rbh + Wbh) # default, when linear_before_reset = 0
            # Rbh + Wbh is done above
            h_gate_scratch = args['h_state'] * r_gate_scratch
            DiagCollector.record(
                'hr_haddamard', h_gate_scratch,
                node=params)
            h_gate_scratch = args['r_2_h_w'].dot(
                h_gate_scratch) + args['r_h_b']
            DiagCollector.record(
                'h_gate_state', h_gate_scratch,
                node=params)

        h_gate_input = args['w_h_b'].copy()
        if idx < params.n_input_cells:
            h_gate_input += args['w_2_h_w'].dot(input_tensor[idx])
        DiagCollector.record(
            'h_gate_input', h_gate_input,
            node=params)
        h_gate_scratch += h_gate_input
        DiagCollector.record(
            'h_gate', h_gate_scratch,
            node=params)
        h_gate_scratch = tanh(h_gate_scratch)
        DiagCollector.record('hr_gate_tanh', h_gate_scratch, node=params)

        # Ht = (1 - zt) (.) ht + zt (.) Ht-1
        args['h_state'] = (1 - z_gate_scratch) * \
            h_gate_scratch + z_gate_scratch * args['h_state']
        DiagCollector.record('h_state_out', args['h_state'], node=params)
        DiagCollector.record_ref('h_state_out_prenorm', 'h_state_out', node=params)

        return args['h_state'].copy()


@params_type(LSTMParameters)
@qrec_type('float')
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
        DiagCollector.record('i_state', args['i_state'], node=params)
        DiagCollector.record('c_state', args['c_state'], node=params)
        DiagCollector.record('input', input_tensor[idx], node=params)

        if not use_cifg:
            input_gate_scratch = np.zeros(
                [params.n_states]).astype(args['i_b'].dtype)
        forget_gate_scratch = np.zeros(
            [params.n_states]).astype(args['f_b'].dtype)
        cell_scratch = np.zeros([params.n_states]).astype(args['c_b'].dtype)
        output_gate_scratch = np.zeros(
            [params.n_states]).astype(args['o_b'].dtype)

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
            DiagCollector.record('i_gate_i', input_gate_scratch, node=params)
            DiagCollector.record('f_gate_i', forget_gate_scratch, node=params)
            DiagCollector.record('c_gate_i', cell_scratch, node=params)
            DiagCollector.record('o_gate_i', output_gate_scratch, node=params)

        # For each cell: compute recurrent_weight * output_state
        if not use_cifg:
            input_gate_scratch_state = args['r_2_i_w'].dot(args['i_state'])
            DiagCollector.record('i_gate_r', input_gate_scratch_state, node=params)
        # in + ScaleRW * ScaleOState
        forget_gate_scratch_state = args['r_2_f_w'].dot(args['i_state'])
        cell_scratch_state = args['r_2_c_w'].dot(args['i_state'])
        output_gate_scratch_state = args['r_2_o_w'].dot(args['i_state'])
        DiagCollector.record('f_gate_r', forget_gate_scratch_state, node=params)
        DiagCollector.record('c_gate_r', cell_scratch_state, node=params)
        DiagCollector.record('o_gate_r', output_gate_scratch_state, node=params)

        if not use_cifg:
            input_gate_scratch += args['i_b'].copy() + input_gate_scratch_state

        forget_gate_scratch += args['f_b'].copy() + forget_gate_scratch_state
        cell_scratch += args['c_b'].copy() + cell_scratch_state
        output_gate_scratch += args['o_b'].copy() + output_gate_scratch_state

        DiagCollector.record('i_gate', input_gate_scratch, node=params)
        DiagCollector.record('f_gate', forget_gate_scratch, node=params)
        DiagCollector.record('c_gate', cell_scratch, node=params)
        DiagCollector.record('o_gate', output_gate_scratch, node=params)

        DiagCollector.record_ref('i_gate_post_bias', 'i_gate', node=params)
        DiagCollector.record_ref('f_gate_post_bias', 'f_gate', node=params)
        DiagCollector.record_ref('c_gate_post_bias', 'c_gate', node=params)
        DiagCollector.record_ref('o_gate_post_bias', 'o_gate', node=params)

        # For each cell: update input gate.
        if not use_cifg:
            if use_peephole:
                input_gate_scratch += args['c_2_i_w'] * args['c_state']
            if use_layer_norm:
                input_gate_scratch = mean_stddev_normalization(
                    input_gate_scratch)
                input_gate_scratch *= args['i_norm']
                input_gate_scratch += args['i_b']
            input_gate_scratch = sigmoid(input_gate_scratch)
            # sigmoid at scale
            DiagCollector.record('i_gate_after_act',
                                 input_gate_scratch, node=params)

        # For each cell: update forget gate.
        if use_peephole:
            forget_gate_scratch += args['c_2_f_w'] * args['c_state']

        if use_layer_norm:
            forget_gate_scratch = mean_stddev_normalization(
                forget_gate_scratch)
            forget_gate_scratch *= args['f_norm']
            forget_gate_scratch += args['f_b']

        forget_gate_scratch = sigmoid(forget_gate_scratch)
        DiagCollector.record('f_gate_after_act',
                             forget_gate_scratch, node=params)

        # For each cell: update the cell.
        args['c_state'] *= forget_gate_scratch
        DiagCollector.record('cstate_cbar_f', args['c_state'], node=params)

        if use_layer_norm:
            cell_scratch = mean_stddev_normalization(cell_scratch)
            cell_scratch *= args['c_norm']
            cell_scratch += args['c_b']

        cell_scratch = tanh(cell_scratch)
        DiagCollector.record('c_gate_after_act', cell_scratch, node=params)

        if use_cifg:
            forget_gate_scratch = 1.0 - forget_gate_scratch
            cstate_c_i = cell_scratch * forget_gate_scratch
        else:
            cstate_c_i = cell_scratch * input_gate_scratch
        DiagCollector.record('cstate_c_i', cstate_c_i, node=params)
        args['c_state'] += cstate_c_i
        DiagCollector.record('c_state_before_scale',
                             args['c_state'], node=params)
        if params.cell_clip > 0.0:
            args['c_state'] = abs_clip(args['c_state'], params.cell_clip)

        # For each cell: update the output gate.
        if use_peephole:
            output_gate_scratch += args['c_2_o_w'] * args['c_state']

        if use_layer_norm:
            output_gate_scratch = mean_stddev_normalization(
                output_gate_scratch)
            output_gate_scratch *= args['o_norm']
            output_gate_scratch += args['o_b']

        output_gate_scratch = sigmoid(output_gate_scratch)
        DiagCollector.record('o_gate_after_act',
                             output_gate_scratch, node=params)
        DiagCollector.record('c_state_out', args['c_state'], node=params)
 
        cell_scratch = tanh(args['c_state'])
        output_gate_scratch *= cell_scratch
        DiagCollector.record('output', output_gate_scratch, node=params)

        use_projection_weight = args.get('proj_w') is not None
        use_projection_bias = args.get('proj_b') is not None

        if use_projection_weight or use_projection_bias:
            raise NotImplementedError("LSTMP is not yet supported by kernel")

        args['i_state'] = output_gate_scratch.copy()
        if params.lstm_output_c_state:
            return output_gate_scratch, args['c_state']
        return output_gate_scratch, None
