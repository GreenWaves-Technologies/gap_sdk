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
from nntool.graph.types import RNNNode

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from nntool.importer.common.constant_mixin import ConstantMixin
from .rnn_mixin import RNNMixin


@onnx_op("RNN")
class RNN(RNNMixin, ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        used_tensors = kwargs['used_tensors']
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        input_shapes = [inp[2].shape if inp else None for inp in inputs]
        x = inputs[0]

        seq_len = input_shapes[0][1] if node.attrs.get("layout", 0) else input_shapes[0][0]
        if seq_len is None:
            seq_len = 1

        input_size = input_shapes[0][2]
        hidden_size = node.attrs["hidden_size"]
        direction = node.attrs.get("direction", "forward")
        num_directions = 2 if direction == "bidirectional" else 1

        i_weights = cls.get_constant(inputs[1])
        r_weights = cls.get_constant(inputs[2])
        biases = cls.get_constant(inputs[3]) if inputs[3] else np.zeros(
            [num_directions, 2 * hidden_size])
        tensors = {'forward': {}, 'backward': {}}
        # extract all the tensors in approriate format
        cls.deep_update(tensors, cls.extract_weights(
            i_weights, hidden_size,
            ('i_2_i_w',), num_directions))
        cls.deep_update(tensors, cls.extract_weights(
            r_weights, hidden_size,
            ('r_2_i_w',), num_directions))
        cls.deep_update(tensors, cls.extract_biases(
            biases, hidden_size,
            ("w_b", "r_b"), num_directions))

        for i in range(num_directions):
            t = tensors['forward' if i == 0 else 'backward']
            t['i_b'] = t['w_b'] + t['r_b']
            del t['w_b']
            del t['r_b']

        cls.deep_update(tensors,
                        cls.get_state(G, inputs, 5, 'i_state', hidden_size, num_directions))

        return cls.attach_rnn(G, x, RNNNode, {}, valid_name, tensors,
                              used_tensors, hidden_size, input_size,
                              all_nodes, node, seq_len, num_directions)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_14(cls, node, **kwargs):
        return cls._common(node, **kwargs)
