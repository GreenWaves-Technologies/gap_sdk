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
from quantization.new_qrec import QRec

from graph.dim import Dim
from graph.types.base import NNEdge, cls_op_name

from utils.node_id import NodeId

from .rnn import RNNBaseParameters

LOG = logging.getLogger("nntool." + __name__)

@cls_op_name('lstm')
class LSTMParameters(RNNBaseParameters):

    INPUT_NAMES = [
        "input",
        "i_2_i_w",
        "i_2_f_w",
        "i_2_c_w",
        "i_2_o_w",
        "r_2_i_w",
        "r_2_f_w",
        "r_2_c_w",
        "r_2_o_w",
        "c_2_i_w",
        "c_2_f_w",
        "c_2_o_w",
        "i_b",
        "f_b",
        "c_b",
        "o_b",
        "proj_w",
        "proj_b",
        "i_state",
        "c_state",
        "i_norm",
        "f_norm",
        "c_norm",
        "o_norm"
    ]

    STATE_PARAMETERS = ["i_state", "c_state"]

    def __init__(self, name, *args, cell_clip=0.0, proj_clip=0.0, **kwargs):
        super(LSTMParameters, self).__init__(name, *args, **kwargs)
        self.cell_clip = cell_clip
        self.proj_clip = proj_clip
        self.at_options.valid_options['LSTM_OUTPUT_C_STATE'] = int
        self.at_options.valid_options['QUANT_C_STATE_WITH_STAT'] = int
        self.lstm_output_c_state = (False, None)
        self.quant_c_state_with_stat = False

    def set_c_state_as_output(self, G):
        output_c_state = G.add_output()
        lstm_qrec = G.quantization and G.quantization.get(NodeId(self))
        if lstm_qrec:
            c_state_idx = self.INPUT_NAMES.index('c_state')
            in_q = lstm_qrec.in_qs[c_state_idx]
            lstm_qrec.out_qs.append(in_q)
            c_state_q = QRec.scaled(in_qs=[in_q], out_qs=[in_q])
            G.quantization[NodeId(output_c_state)] = c_state_q

        G.add_edge(NNEdge(self, output_c_state, from_idx=1))
        G.add_dimensions()

    @property
    def lstm_output_c_state(self):
        if hasattr(self.at_options, 'lstm_output_c_state'):
            return self.at_options.lstm_output_c_state == 1
        return False

    @lstm_output_c_state.setter
    def lstm_output_c_state(self, val_and_graph):
        self.at_options.lstm_output_c_state = 1 if val_and_graph[0] else 0
        if val_and_graph[0]:
            self.set_c_state_as_output(val_and_graph[1])

    @property
    def quant_c_state_with_stat(self):
        if hasattr(self.at_options, 'quant_c_state_with_stat'):
            return self.at_options.quant_c_state_with_stat == 1
        return False

    @quant_c_state_with_stat.setter
    def quant_c_state_with_stat(self, val):
        self.at_options.quant_c_state_with_stat = 1 if val else 0

    def get_output_size(self, in_dims):
        output_dims = super(LSTMParameters, self).get_output_size(in_dims)
        if self.lstm_output_c_state:
            output_dims.append(Dim.unnamed([1, self.n_states]))
        return output_dims

    def __str__(self):
        return "Number of cells: {}/{}, {} Cell {} Prj {} {} {}".format(
            self.n_input_cells, self.n_cells,
            ("Reversed " if self.revert else ""),
            self.cell_clip,
            self.proj_clip,
            self.activation,
            self.at_options
        )

    def compute_load(self):
        in_sizes = {self.INPUT_NAMES[idx]: self.in_dims[idx] for idx in range(len(self.in_dims))}
        return in_sizes["i_state"].size() * (self.n_cells*in_sizes["i_state"].size()*4 + self.n_input_cells*in_sizes["input"].size()*4) + \
            self.n_cells*(in_sizes["i_state"].size()*4 + in_sizes["c_state"].size()*3)
