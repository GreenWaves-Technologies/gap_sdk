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

from generation.at_types.tc_arg_info import GlobalResetArgInfo
from generation.generator_decorators import QREC_FLOAT, generation_function
from graph.types import GRUParameters, LSTMParameters, RNNParameters
from .global_names import *


@generation_function("globals",
                     (RNNParameters, LSTMParameters, GRUParameters),
                     qrec_types=(QREC_FLOAT,))
def fp16_rnn_infos_generator(gen, node, qrec, pnode, fnode) -> bool:
    del pnode, qrec
    if fnode is not None:
        return False
    if node.rnn_states_as_inputs:
        gen.globals.append(GlobalResetArgInfo(
            f"{node.name}_Reset", 'AT_MEM_L2', 'AT_MEM_UNDEF'))
    return True
