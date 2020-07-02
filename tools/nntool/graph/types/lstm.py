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

from .base import Transposable
from .rnn import RNNBaseParameters

LOG = logging.getLogger("nntool." + __name__)


class LSTMParameters(RNNBaseParameters):
    op_name = "lstm"

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

    def __init__(self, name, *args, cell_clip=0.0, proj_clip=0.0, **kwargs):
        super(LSTMParameters, self).__init__(name, *args, **kwargs)
        self.cell_clip = cell_clip
        self.proj_clip = proj_clip

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "{} Cell {} Prj {} {} {}".format(
            Transposable.__str__(self),
            self.cell_clip,
            self.proj_clip,
            self.activation,
            self.at_options
        )
