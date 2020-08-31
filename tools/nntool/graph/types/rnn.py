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

from graph.dim import Dim
from graph.types.base import Transposable, SingleInputAndOutput
from graph.types.input_output import ConstantInputParameters

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=abstract-method
class RNNBaseParameters(Transposable, SingleInputAndOutput):

    INPUT_NAMES = []

    def __init__(self, name, *args, n_cells=None, n_states=None, n_inputs=None, n_input_cells=None, n_output_cells=None,
                 activation="tanh", **kwargs):
        super(RNNBaseParameters, self).__init__(name, *args, **kwargs)
        self.activation = activation
        self.n_cells = n_cells
        self.n_states = n_states
        self.n_inputs = n_inputs
        self.n_input_cells = n_input_cells
        self.n_output_cells = n_output_cells
        self.at_options.valid_options['PARALLELFEATURES'] = int
        self.at_options.valid_options['ENABLEIM2COL'] = int

    def get_parameter_size(self):
        return 0


    def get_name_indexes(self):
        return {name: idx for idx, name in enumerate(self.INPUT_NAMES)}

    def get_param_node(self, G, name):
        idx = self.INPUT_NAMES.index(name)
        return next([edge.from_node for edge in G.in_edges(
            self.name) if edge.to_idx == idx], None)

    def get_param(self, G, name):
        const_node = self.get_param_node(G, name)
        assert isinstance(const_node, ConstantInputParameters), "parameter is not a constant"
        return const_node.value if const_node else None

    def get_params(self, G, names):
        idxes = [idx for idx, input_name
                 in enumerate(self.INPUT_NAMES)
                 if input_name in names]
        return {self.INPUT_NAMES[edge.to_idx]: edge.from_node.value
                for edge in G.in_edges(self.name) if edge.to_idx in idxes}

    def get_param_nodes(self, G, names):
        idxes = [idx for idx, input_name
                 in enumerate(self.INPUT_NAMES)
                 if input_name in names]
        return {self.INPUT_NAMES[edge.to_idx]: edge.from_node
                for edge in G.in_edges(self.name) if edge.to_idx in idxes}

    def set_param(self, G, name, value):
        const_node = self.get_param_node(G, name)
        assert isinstance(const_node, ConstantInputParameters), "parameter is not a constant"
        const_node.value = value

    def get_output_size(self, in_dims):
        out_dim = Dim.unnamed([self.n_output_cells, self.n_states])
        if self.transpose_out:
            out_dim.transpose(self.transpose_out[0])
        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def __str__(self):
        return "{} {} {}".format(
            Transposable.__str__(self),
            self.activation,
            self.at_options
        )


class RNNParameters(RNNBaseParameters):

    INPUT_NAMES = [
        "input",
        "i_2_i_w",
        "r_2_i_w",
        "i_b",
        "i_state",
    ]

    def get_parameter_size(self):
        return ((self.n_inputs + self.n_states) * (self.n_inputs + 1)) + self.n_states

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    op_name = "rnn"
