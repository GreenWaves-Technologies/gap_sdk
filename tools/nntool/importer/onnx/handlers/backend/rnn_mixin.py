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

from graph.types.rnn import RNNParameters
from graph.types.others import ConcatParameters, TransposeParameters
import numpy as np
from graph.dim import Dim
from graph.types import (ConstantInputParameters, NNEdge, ReshapeParameters,
                         SplitParameters, LSTMParameters, GRUParameters)
from importer.common.provisional_dim import ProvisionalDim


class RNNMixin(object):
    @classmethod
    def deep_update(cls, x, y):
        for k in y:
            if k in x:
                x[k].update(y[k])
            else:
                x[k] = y[k]
        return x

    @classmethod
    def extract_weights(cls, weights, hidden_size, keys, num_directions):
        return {
            'forward' if dir == 0 else 'backward':
                {keys[i]: arr.reshape((hidden_size, -1))
                 for i, arr in enumerate(np.split(dir_arr, len(keys), axis=1))}
            for dir, dir_arr in enumerate(np.split(weights, num_directions, axis=0))
        }

    @classmethod
    def extract_biases(cls, biases, hidden_size, keys, num_directions):
        biases = biases.reshape((num_directions, len(keys), hidden_size))
        return {
            'forward' if dir == 0 else 'backward':
                {keys[i]: arr.reshape((hidden_size))
                 for i, arr in enumerate(np.split(dir_arr, len(keys), axis=1))}
            for dir, dir_arr in enumerate(np.split(biases, num_directions, axis=0))
        }

    @classmethod
    def combine_biases(cls, tensors, gates, num_directions):
        for i in range(num_directions):
            t = tensors['forward' if i == 0 else 'backward']
            for gate in gates:
                t['%s_b' % gate] = t['i_%s_b' % gate] + t['r_%s_b' % gate]
                del t['i_%s_b' % gate]
                del t['r_%s_b' % gate]

    @classmethod
    def get_state(cls, inputs, idx, name, hidden_size, num_directions=1):
        state = cls.get_constant(inputs[idx]) if inputs[idx] else np.zeros((num_directions, hidden_size))
        return {
            'forward' if dir == 0 else 'backward':
                {name: dir_arr.reshape((hidden_size,))}
            for dir, dir_arr in enumerate(np.split(state, num_directions, axis=0))
        }

    @staticmethod
    def attach_rnn(G, x, rnn_params_class, extra_args, valid_name, tensors,
                   used_tensors, hidden_size, input_size,
                   all_nodes, node, seq_len, num_directions):
        # check if both outputs are used
        used_outputs = tuple(outp in used_tensors for outp in node.output)
        if len(used_outputs) == 3 and used_outputs[2]:
            raise ValueError("outputting the cell state of an LSTM is not supported")
        # if the full state output is used we need to output all cells
        n_output_cells = seq_len if used_outputs[0] else 1
        # if both outputs are used then
        both_dir = []
        for i in range(num_directions):
            node_name = valid_name if i == 0 else valid_name + '_rev'
            rnn_params = rnn_params_class(node_name, n_cells=seq_len, n_states=hidden_size,
                                          n_inputs=input_size, n_input_cells=seq_len,
                                          n_output_cells=n_output_cells,
                                          output_directions=True, revert=(i==1), **extra_args)
            both_dir.append(rnn_params)
            t = tensors['forward' if i == 0 else 'backward']
            for idx, name in enumerate(rnn_params.INPUT_NAMES):
                if name == 'input':
                    G.add_edge(NNEdge(from_node=x[0], to_node=rnn_params, from_idx=x[1], to_idx=0))
                    continue
                if name not in t:
                    continue
                cparams = ConstantInputParameters("%s_%s" % (
                    node_name, name), value=t[name], dims=Dim.unnamed(t[name].shape))
                G.add_edge(NNEdge(from_node=cparams, to_node=rnn_params, from_idx=0, to_idx=idx))

            # Link the state weights to the input weights
            # The autotiler expects the state and input weights to be
            # concatenated. This tells the constant code generator to do this
            rnn_in_edges = [in_edge for in_edge in G.in_edges(rnn_params.name)]
            in_nodes = {}
            for edge in rnn_in_edges:
                in_nodes[edge.to_idx] = edge.from_node
            if isinstance(rnn_params, LSTMParameters):
                for gate in ['i', 'o', 'c', 'f']:
                    i_w_node = in_nodes[LSTMParameters.INPUT_NAMES.index('i_2_%s_w' % gate)]
                    r_w_node = in_nodes[LSTMParameters.INPUT_NAMES.index('r_2_%s_w' % gate)]
                    r_w_node.concated_nodes.append(i_w_node)
                    i_w_node.generate_value = False
            elif isinstance(rnn_params, GRUParameters):
                for gate in ['r', 'z', 'h']:
                    i_w_node = in_nodes[GRUParameters.INPUT_NAMES.index('w_2_%s_w' % gate)]
                    r_w_node = in_nodes[GRUParameters.INPUT_NAMES.index('r_2_%s_w' % gate)]
                    r_w_node.concated_nodes.append(i_w_node)
                    i_w_node.generate_value = False
            elif isinstance(rnn_params, RNNParameters):
                for gate in ['i']:
                    i_w_node = in_nodes[RNNParameters.INPUT_NAMES.index('i_2_%s_w' % gate)]
                    r_w_node = in_nodes[RNNParameters.INPUT_NAMES.index('r_2_%s_w' % gate)]
                    r_w_node.concated_nodes.append(i_w_node)
                    i_w_node.generate_value = False

            # trim batch dimension from state values
            for state_node_name in ['i_state', 'c_state', 'h_state']:
                if state_node_name not in rnn_params.INPUT_NAMES:
                    continue
                state_node = in_nodes[rnn_params.INPUT_NAMES.index(state_node_name)]
                # set by default as allocated
                state_node.at_options.allocate = True
                state_node.is_constant = False
                # reset state after each invocation
                state_node.always_copy = True
                # add a single reset
                state_node.reset_name = "Reset"

        out_idx = 0 if used_outputs[0] else 1
        if num_directions > 1:
            # if it is bidir then we need a concat
            concat_params = ConcatParameters(valid_name + '_bidir', axis=0)
            for idx in range(num_directions):
                G.add_edge(NNEdge(from_node=both_dir[idx],
                                  to_node=concat_params,
                                  from_idx=out_idx, to_idx=idx))
            out_edge = (concat_params, 0)
        else:
            out_edge = (both_dir[0], out_idx)


        if out_idx == 0:
            # if output 0 is used then the expected dims are (steps, dirs, hidden_size)
            trans_params = TransposeParameters(valid_name + '_trans', transpose=(1, 0, 2))
            G.add_edge(NNEdge(out_edge[0], trans_params, from_idx=out_edge[1]))
            out_edge = (trans_params, 0)

        if used_outputs[0] and used_outputs[1]:
            raise ValueError('recurrent network with both last output and all states output is not supported')

        if used_outputs[0]:
            all_nodes[node.output[0]] = (out_edge[0], out_edge[1], ProvisionalDim(
                (n_output_cells, num_directions, None, hidden_size)))
            out_edge[0].meta['onnx_output'] = [node.output[0]]
        else:
            all_nodes[node.output[1]] = (out_edge[0], out_edge[1], ProvisionalDim((num_directions, None, hidden_size)))
            out_edge[0].meta['onnx_output'] = [node.output[1]]

        return out_edge[0]
