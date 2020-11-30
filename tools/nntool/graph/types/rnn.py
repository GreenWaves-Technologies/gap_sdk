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
from graph.types.base import Parameters, SensitiveToOrder, SingleInputAndOutput
from graph.types.input_output import ConstantInputParameters

LOG = logging.getLogger("nntool." + __name__)

#pylint: disable=abstract-method


class RNNBaseParameters(Parameters, SensitiveToOrder, SingleInputAndOutput):

    INPUT_NAMES = []
    STATE_PARAMETERS = []

    def __init__(self, name, *args, n_cells=None, n_states=None, n_inputs=None, n_input_cells=None, n_output_cells=None,
                 activation="tanh", revert=False, output_directions=False, **kwargs):
        super(RNNBaseParameters, self).__init__(name, *args, **kwargs)
        self.activation = activation
        self.n_cells = n_cells
        self.n_states = n_states
        self.n_inputs = n_inputs
        self.n_input_cells = n_input_cells
        self.n_output_cells = n_output_cells
        self.at_options.valid_options['PARALLELFEATURES'] = int
        self.at_options.valid_options['ENABLEIM2COL'] = int
        self.at_options.valid_options['RNN_USE_HARDACT'] = int
        self.at_options.valid_options['RNN_SAME_INOUT_SCALE'] = int
        self.revert = revert
        self.always_reset_state = True
        self.hard_act = False
        self.rnn_same_inout_scale = False
        self.output_directions = output_directions

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
        if self.output_directions:
            out_dims = [Dim.unnamed([1, self.n_output_cells, self.n_states])]
        else:
            out_dims = [Dim.unnamed([self.n_output_cells, self.n_states])]
        return out_dims

    @property
    def can_equalize(self):
        return False

    @property
    def hard_act(self):
        if hasattr(self.at_options, 'rnn_use_hardact'):
            return self.at_options.rnn_use_hardact == 1
        return False

    @hard_act.setter
    def hard_act(self, val):
        self.at_options.rnn_use_hardact = 1 if val else 0

    @property
    def rnn_same_inout_scale(self):
        if hasattr(self.at_options, 'rnn_same_inout_scale'):
            return self.at_options.rnn_same_inout_scale == 1
        return False

    @rnn_same_inout_scale.setter
    def rnn_same_inout_scale(self, val):
        self.at_options.rnn_same_inout_scale = 1 if val else 0

    def compute_load(self):
        return self.in_dims[0].size() * 2

    def __str__(self):
        return "{}{} {}".format(
            ("Reversed " if self.revert else ""),
            self.activation,
            self.at_options
        )


class RNNParameters(RNNBaseParameters):
    op_name = "rnn"

    INPUT_NAMES = [
        "input",
        "i_2_i_w",
        "r_2_i_w",
        "i_b",
        "i_state",
    ]

    STATE_PARAMETERS = ["i_state"]

    def get_parameter_size(self):
        return ((self.n_inputs + self.n_states) * (self.n_inputs + 1)) + self.n_states

    def clone(self, name, groupn=None):
        raise NotImplementedError()




class GRUParameters(RNNBaseParameters):
    op_name = "gru"

    INPUT_NAMES = [
        "input",
        "w_2_z_w",
        "w_2_r_w",
        "w_2_h_w",
        "r_2_z_w",
        "r_2_r_w",
        "r_2_h_w",
        "w_z_b",
        "w_r_b",
        "w_h_b",
        "r_z_b",
        "r_r_b",
        "r_h_b",
        "h_state",
    ]

    STATE_PARAMETERS = ["h_state"]

    def __init__(self, *args, linear_before_reset=False, activation_zr=None, **kwargs) -> None:
        super(GRUParameters, self).__init__(*args, **kwargs)
        self.activation_zr = "sigmoid" if activation_zr is None else activation_zr
        self.linear_before_reset = linear_before_reset

    def get_parameter_size(self):
        return 3 * ((self.n_inputs + self.n_states) * (self.n_inputs + 1)) + self.n_states

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "{}{} {} {}{}".format(
            ("Reversed " if self.revert else ""),
            self.activation,
            self.activation_zr,
            "linear before reset " if self.linear_before_reset else "",
            self.at_options
        )
