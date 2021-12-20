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
from graph.types import GRUParameters
from importer.common.constant_mixin import ConstantMixin
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .rnn_mixin import RNNMixin


@onnx_op("GRU")
class GRU(RNNMixin, ConstantMixin, BackendHandler):

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
        linear_before_reset = bool(node.attrs.get("linear_before_reset", 0))
        if not linear_before_reset:
            logger.warning("In %s linear_before_reset == 0 not supported by the Autotiler kernels", valid_name)
        # output_sequence = node.attrs.get("output_sequence", 0)

        i_weights = cls.get_constant(inputs[1])
        r_weights = cls.get_constant(inputs[2])
        biases = cls.get_constant(inputs[3]) if inputs[3] else np.zeros(
            [num_directions, 6 * hidden_size])
        tensors = {'forward': {}, 'backward': {}}
        # extract all the tensors in approriate format
        cls.deep_update(tensors, cls.extract_weights(
            i_weights, hidden_size,
            ('w_2_z_w', 'w_2_r_w', 'w_2_h_w'), num_directions))
        cls.deep_update(tensors, cls.extract_weights(
            r_weights, hidden_size,
            ('r_2_z_w', 'r_2_r_w', 'r_2_h_w'), num_directions))
        cls.deep_update(tensors, cls.extract_biases(
            biases, hidden_size,
            ("w_z_b", "w_r_b", "w_h_b", "r_z_b", "r_r_b", "r_h_b"), num_directions))
        cls.deep_update(tensors,
                        cls.get_state(G, inputs, 5, 'h_state', hidden_size, num_directions))
        cls.combine_biases_gru(tensors, ['z', 'r'], num_directions)
        extra_args = {
            'linear_before_reset': linear_before_reset
        }
        return cls.attach_rnn(G, x, GRUParameters, extra_args, valid_name, tensors,
                              used_tensors, hidden_size, input_size,
                              all_nodes, node, seq_len, num_directions)

    @classmethod
    def combine_biases_gru(cls, tensors, gates, num_directions):
        for i in range(num_directions):
            t = tensors['forward' if i == 0 else 'backward']
            for gate in gates:
                t['%s_b' % gate] = t['w_%s_b' % gate] + t['r_%s_b' % gate]
                del t['w_%s_b' % gate]
                del t['r_%s_b' % gate]

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_3(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)
