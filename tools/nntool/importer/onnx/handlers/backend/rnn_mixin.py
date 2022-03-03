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
from numpy.core.fromnumeric import reshape
from graph.dim import Dim
from graph.types import (ConstantInputParameters, GRUParameters,
                         LSTMParameters, NNEdge)
from graph.types.others import ConcatParameters, ReshapeParameters, SplitParameters, StridedSliceParameters, TransposeParameters
from graph.types.rnn import RNNParameters
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
            'forward' if direction == 0 else 'backward':
                {keys[i]: arr.reshape((hidden_size, -1))
                 for i, arr in enumerate(np.split(dir_arr, len(keys), axis=1))}
            for direction, dir_arr in enumerate(np.split(weights, num_directions, axis=0))
        }

    @classmethod
    def extract_biases(cls, biases, hidden_size, keys, num_directions):
        biases = biases.reshape((num_directions, len(keys), hidden_size))
        return {
            'forward' if direction == 0 else 'backward':
                {keys[i]: arr.reshape((hidden_size))
                 for i, arr in enumerate(np.split(dir_arr, len(keys), axis=1))}
            for direction, dir_arr in enumerate(np.split(biases, num_directions, axis=0))
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
    def get_state(cls, G, inputs, idx, name, hidden_size, num_directions=1):
        if not inputs[idx]:
            state = np.zeros((num_directions, hidden_size))
        elif cls.is_constant(inputs[idx]):
            state = cls.get_constant(inputs[idx])
        else:
            state_inp = inputs[idx]
            if num_directions == 2:
                act_slices = (((0, 1, 1), (0, hidden_size, 1)),
                              ((1, 2, 1), (0, hidden_size, 1)))
                out_shapes = ((1, hidden_size), (1, hidden_size))
                split = SplitParameters(G.unique_name(f'{name}_split'), act_slices=act_slices,
                                        out_shapes=out_shapes, axis=0)
                G.add_edge(
                    NNEdge(from_node=state_inp[0], to_node=split, from_idx=state_inp[1]))
                return {
                    'forward': {name: (split, 0)},
                    'backward': {name: (split, 0)},
                }
            else:
                reshape = ReshapeParameters(G.unique_name(f'{name}_reshape'), old_shape=(1, hidden_size),
                                            shape=(hidden_size,))
                G.add_edge(
                    NNEdge(from_node=state_inp[0], to_node=reshape, from_idx=state_inp[1]))
                return {
                    'forward': {name: (reshape, 0)},
                }

        state = cls.get_constant(inputs[idx]) if inputs[idx] else np.zeros(
            (num_directions, hidden_size))
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
        output_cell = len(used_outputs) == 3 and used_outputs[2]
        # if the full state output is used we need to output all cells
        n_output_cells = seq_len if used_outputs[0] else 1
        # if both outputs are used then
        both_dir = []
        for i in range(num_directions):
            node_name = valid_name if i == 0 else valid_name + '_rev'
            rnn_params = rnn_params_class(node_name, n_cells=seq_len, n_states=hidden_size,
                                          n_inputs=input_size, n_input_cells=seq_len,
                                          n_output_cells=n_output_cells,
                                          output_directions=True, revert=(i == 1), **extra_args)
            if output_cell:
                rnn_params.lstm_output_c_state = True
            both_dir.append(rnn_params)
            t = tensors['forward' if i == 0 else 'backward']
            for idx, name in enumerate(rnn_params.INPUT_NAMES):
                if name == 'input':
                    # x_shape = x[2].shape
                    # new_shape = [x_shape[0] if x_shape[0] is not None else 1, x_shape[-1]]
                    # reshape_param = ReshapeParameters(f"{valid_name}_reshape", old_shape=x_shape, shape=new_shape)
                    # G.add_edge(
                    #     NNEdge(from_node=x[0], to_node=reshape_param, from_idx=x[1], to_idx=0))
                    # G.add_edge(
                    #     NNEdge(from_node=reshape_param, to_node=rnn_params, from_idx=0, to_idx=0))
                    G.add_edge(
                        NNEdge(from_node=x[0], to_node=rnn_params, from_idx=x[1], to_idx=0))
                    continue
                if name not in t:
                    continue
                if isinstance(t[name], tuple):
                    cparams = t[name][0]
                    cidx = t[name][1]
                else:
                    cparams = ConstantInputParameters("%s_%s" % (
                        node_name, name), value=t[name], dims=Dim.unnamed(t[name].shape))
                    cidx = 0
                G.add_edge(NNEdge(from_node=cparams,
                                  to_node=rnn_params, from_idx=cidx, to_idx=idx))

            # TODO - Move to the quantizer (and make a qtype attr) since this depends on the kernel used
            # Link the state weights to the input weights
            # The autotiler expects the state and input weights to be
            # concatenated. This tells the constant code generator to do this
            rnn_in_edges = [in_edge for in_edge in G.in_edges(rnn_params.name)]
            in_nodes = {}
            for edge in rnn_in_edges:
                in_nodes[edge.to_idx] = edge.from_node

            # trim batch dimension from state values
            for state_node_name in ['i_state', 'c_state', 'h_state']:
                if state_node_name not in rnn_params.INPUT_NAMES:
                    continue
                state_node = in_nodes[rnn_params.INPUT_NAMES.index(
                    state_node_name)]
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
            concat_params = ConcatParameters(G.unique_name(valid_name + '_bidir'), axis=0)
            if output_cell:
                output_cell = (ConcatParameters(G.unique_name(valid_name + '_cstate_bidir'), axis=0), 0)

            for idx in range(num_directions):
                G.add_edge(NNEdge(from_node=both_dir[idx],
                                  to_node=concat_params,
                                  from_idx=out_idx, to_idx=idx))
                if output_cell:
                    G.add_edge(NNEdge(from_node=both_dir[idx],
                                    to_node=output_cell[0],
                                    from_idx=2, to_idx=idx))
            out_edge = (concat_params, 0)
        else:
            out_edge = (both_dir[0], out_idx)
            if output_cell:
                output_cell = (both_dir[0], 1)

        if out_idx == 0:
            # if output 0 is used then the expected dims are (steps, dirs, hidden_size)
            trans_params = TransposeParameters(
                G.unique_name(f'{valid_name}_trans'), transpose=(1, 0, 2))
            G.add_edge(NNEdge(out_edge[0], trans_params, from_idx=out_edge[1]))
            out_edge = (trans_params, 0)

        if used_outputs[0]:
            all_nodes[node.output[0]] = (out_edge[0], out_edge[1], ProvisionalDim(
                (n_output_cells, num_directions, None, hidden_size)), None)
            out_edge[0].meta['onnx_output'] = [node.output[0]]
            if used_outputs[1]:
                sslice = StridedSliceParameters(
                    G.unique_name(f'{valid_name}_split'),
                    act_slice=((n_output_cells - 1, n_output_cells, 1), (0, num_directions, 1), (0, hidden_size, 1)), out_shape=(num_directions, hidden_size))
                G.add_edge(NNEdge(from_node=out_edge[0], from_idx=out_edge[1], to_node=sslice))
                all_nodes[node.output[1]] = (sslice, 0, ProvisionalDim(
                    (num_directions, None, hidden_size)), None)
        else:
            all_nodes[node.output[1]] = (out_edge[0], out_edge[1], ProvisionalDim(
                (num_directions, None, hidden_size)), None)
            out_edge[0].meta['onnx_output'] = [node.output[1]]

        if output_cell:
            all_nodes[node.output[2]] = (output_cell[0], output_cell[1], ProvisionalDim(
                (num_directions, None, hidden_size)), None)

        return out_edge[0]
