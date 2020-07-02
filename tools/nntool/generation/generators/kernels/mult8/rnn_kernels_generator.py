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

from generation.at_types.at_params import (NO_ACTIVATION, gen_active_at_params,
                                           gen_globalpool_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import RNNParameters, LSTMParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (RNNParameters, LSTMParameters), qrec_types=(QREC_MULT8, ))
def rnn_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams, qrec
    gen.kernels.append(RNNKernel(node.name, cname, node,
                                 at_ver=gen.opts['at_ver'],
                                 gen_ctrl=node.get_gen_ctrl()))
    return True

# int RNN_Stack_SQ8(
# 	char *Name,
# 	CNN_GenControl_T *Ctrl,

# 	int NCells,
# 	int K0,
# 	int K1,
# 	int DimState,
# 	int DimIn
# 	)
# int LSTM_Stack_SQ8(
#         char *Name,
#         CNN_GenControl_T *Ctrl,

#         int NCells,
#         int K0,
#         int K1,
#         int DimState,
#         int DimIn
#         )


def gen_rnn_sq8(code_block, kname, cname, ctrl, ncells, k0, k1, dim_state, dim_in):
    code_block.write(
        '{}("{}", {}, 4, 1, {}, {}, {}, {}, {}, 0, 0);'.format(kname, cname, ctrl,
                                                               ncells, k0,
                                                               k1, dim_state,
                                                               dim_in))


class RNNKernel(AutotilerKernel):
    def __init__(self, node_name, cname, rnn_params, gen_ctrl=None, at_ver=3):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        if isinstance(rnn_params, RNNParameters):
            self.kname = "RNN_Stack_SQ8"
        elif isinstance(rnn_params, LSTMParameters):
            self.kname = "LSTM_Stack_SQ8"
        else:
            raise ValueError("unknown RNN parameter type")
        self.n_cells = rnn_params.n_cells
        self.n_states = rnn_params.n_states
        self.n_inputs = rnn_params.n_inputs
        self.n_input_cells = rnn_params.n_input_cells
        self.n_output_cells = rnn_params.n_output_cells
        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)
            gen_ctrl = self.gen_ctrl.ctrl_name
        else:
            gen_ctrl = "0"

        gen_rnn_sq8(code_block, self.kname, self.cname, gen_ctrl, self.n_cells,
                    self.n_input_cells, self.n_output_cells,
                    self.n_states,
                    self.n_inputs)
        return code_block
