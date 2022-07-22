# Copyright (C) 2021, 2022  GreenWaves Technologies, SAS

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

import numpy as np
from bfloat16 import bfloat16
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalResetArgInfo, LocalArgInfo
from nntool.generation.bindings import (CommentBindingList, GArgName, GNodeArgEdge, Imm, NoArg,
                                 NodeBindingList)
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                  paramstype)
from nntool.generation.generators.helpers.rnn import num_sequences
from nntool.graph.types import (GRUNode, LSTMNode, RNNNodeBase,
                         RNNNode)
from nntool.graph.types.constant_input import ConstantInputNode

LOG = logging.getLogger(__name__)


@paramstype(GRUNode, RNNNode, LSTMNode)
@ktype('float')
class RNNFP16Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if node.rnn_states_as_inputs:
            gen.globals.append(GlobalResetArgInfo(
                f"{node.name}_Reset", 'AT_MEM_L2', 'AT_MEM_UNDEF'))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, RNNNode):
            set_rnn_bindings(gen, step_idx, in_eparams,
                             out_eparams, cname, node, qrec)
        elif isinstance(node, LSTMNode):
            set_lstm_bindings(gen, step_idx, in_eparams,
                              out_eparams, cname, node, qrec)
        elif isinstance(node, GRUNode):
            set_gru_bindings(gen, step_idx, in_eparams,
                             out_eparams, cname, node, qrec)

        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
            return False
        gen.kernels.append(RNNFP16Kernel(
            node.name, cname, node, qrec, gen_ctrl=node.get_gen_ctrl()))
        return True


def set_lstm_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                      rnn_params, rnn_q):

    names = rnn_params.get_name_indexes()

    gen.bindings.append(
        CommentBindingList("Node {} inq {} outq {}", cname,
                           rnn_q.in_qs[0],
                           rnn_q.out_qs[0])
    )
    c_state_eparams = in_eparams[names['c_state']]
    i_state_eparams = in_eparams[names['i_state']]

    num_seq = num_sequences(rnn_params)
    if num_seq > 1:
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_CellInternal01" % step_idx))
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_StateInternal01" % step_idx))

    if num_seq > 2:
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_CellInternal02" % step_idx))
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_StateInternal02" % step_idx))

    if isinstance(i_state_eparams.creating_node, ConstantInputNode):
        reset_name = (i_state_eparams.creating_node.reset_name
                      if not rnn_params.rnn_states_as_inputs else
                      f"{rnn_params.name}_Reset")
    else:
        reset_name = None
    gen.bindings.append(
        NodeBindingList(cname,
                        GNodeArgEdge(c_state_eparams, direction="GNA_INOUT"),
                        GNodeArgEdge(i_state_eparams, direction="GNA_INOUT"),
                        GNodeArgEdge("S%s_CellInternal01" % step_idx, alias=c_state_eparams,
                                     direction="GNA_INOUT") if num_seq > 1 else NoArg(),
                        GNodeArgEdge("S%s_StateInternal01" % step_idx, alias=i_state_eparams,
                                     direction="GNA_INOUT") if num_seq > 1 else NoArg(),
                        GNodeArgEdge("S%s_CellInternal02" % step_idx, alias="S%s_CellInternal01" %
                                     step_idx, direction="GNA_INOUT") if num_seq > 2 else NoArg(),
                        GNodeArgEdge("S%s_StateInternal02" % step_idx, alias="S%s_CellInternal01" %
                                     step_idx, direction="GNA_INOUT") if num_seq > 2 else NoArg(),
                        GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[names['r_2_f_w']]),
                        GNodeArgEdge(in_eparams[names['f_b']]),
                        GNodeArgEdge(in_eparams[names['r_2_i_w']]),
                        GNodeArgEdge(in_eparams[names['i_b']]),
                        GNodeArgEdge(in_eparams[names['r_2_c_w']]),
                        GNodeArgEdge(in_eparams[names['c_b']]),
                        GNodeArgEdge(in_eparams[names['r_2_o_w']]),
                        GNodeArgEdge(in_eparams[names['o_b']]),
                        GNodeArgEdge(out_eparams[0], direction="GNA_OUT"),
                        GArgName(reset_name) if reset_name else Imm(0)
                        ))


def set_rnn_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                     rnn_params, rnn_q):
    names = rnn_params.get_name_indexes()

    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {} biasesq {}", cname,
                           rnn_q.in_qs[0], rnn_q.in_qs[names['r_2_i_w']],
                           rnn_q.out_qs[0], rnn_q.in_qs[names['i_b']])
    )
    num_seq = num_sequences(rnn_params)
    if num_seq > 1:
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_StateInternal01" % step_idx))

    if num_seq > 2:
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_StateInternal02" % step_idx))

    i_state_eparams = in_eparams[names['i_state']]
    if isinstance(i_state_eparams.creating_node, ConstantInputNode):
        reset_name = (i_state_eparams.creating_node.reset_name
                      if not rnn_params.rnn_states_as_inputs else
                      f"{rnn_params.name}_Reset")
    else:
        reset_name = None
    gen.bindings.append(
        NodeBindingList(cname,
                        GNodeArgEdge(i_state_eparams, direction="GNA_INOUT"),
                        GNodeArgEdge("S%s_StateInternal01" % step_idx, alias=i_state_eparams,
                                     direction="GNA_INOUT") if num_seq > 1 else NoArg(),
                        GNodeArgEdge("S%s_StateInternal02" % step_idx, alias="S%s_CellInternal01" %
                                     step_idx, direction="GNA_INOUT") if num_seq > 2 else NoArg(),
                        GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[names['r_2_i_w']]),
                        GNodeArgEdge(in_eparams[names['i_b']]),
                        GNodeArgEdge(out_eparams[0], direction="GNA_OUT"),
                        GArgName(reset_name) if reset_name else Imm(0)
                        ))


def set_gru_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                     rnn_params, rnn_q):
    names = rnn_params.get_name_indexes()

    gen.bindings.append(
        CommentBindingList("Node {} inq {} outq {}", cname,
                           rnn_q.in_qs[0],
                           rnn_q.out_qs[0])
    )
    num_seq = num_sequences(rnn_params)
    if num_seq > 1:
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_StateInternal01" % step_idx))

    if num_seq > 2:
        gen.locals.append(LocalArgInfo(
            "int8", "S%s_StateInternal02" % step_idx))

    i_state_eparams = in_eparams[names['h_state']]
    if isinstance(i_state_eparams.creating_node, ConstantInputNode):
        reset_name = (i_state_eparams.creating_node.reset_name
                      if not rnn_params.rnn_states_as_inputs else
                      f"{rnn_params.name}_Reset")
    else:
        reset_name = None
    gen.bindings.append(
        NodeBindingList(cname,
                        GNodeArgEdge(i_state_eparams, direction="GNA_INOUT"),
                        GNodeArgEdge("S%s_StateInternal01" % step_idx, alias=i_state_eparams,
                                     direction="GNA_INOUT") if num_seq > 1 else NoArg(),
                        GNodeArgEdge("S%s_StateInternal02" % step_idx, alias="S%s_CellInternal01" %
                                     step_idx, direction="GNA_INOUT") if num_seq > 2 else NoArg(),
                        GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[names['r_2_r_w']]),
                        GNodeArgEdge(in_eparams[names['r_b']]),
                        GNodeArgEdge(in_eparams[names['r_2_z_w']]),
                        GNodeArgEdge(in_eparams[names['z_b']]),
                        GNodeArgEdge(in_eparams[names['r_2_h_w']]),
                        GNodeArgEdge(in_eparams[names['w_h_b']]),
                        GNodeArgEdge(in_eparams[names['r_h_b']]),
                        GNodeArgEdge(out_eparams[0], direction="GNA_OUT"),
                        GArgName(reset_name) if reset_name else Imm(0)
                        ))


class RNNFP16Kernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
{kname}("{cname}", {gen_ctrl},
                       {n_cells}, {k0}, {k1}, {n_states},
                       {n_inputs}, 0, {revert});
"""

    def __init__(self, node_name, cname, rnn_params, rnn_qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if not rnn_params.hard_act:
            gen_ctrl.rnn_use_hardact = 0
        if not rnn_params.rnn_same_inout_scale:
            gen_ctrl.rnn_same_inout_scale = 0

        if isinstance(rnn_params, RNNNode):
            kname = "RNN_Stack_fp16"
        elif isinstance(rnn_params, LSTMNode):
            kname = "LSTM_Stack_fp16"
        elif isinstance(rnn_params, GRUNode):
            kname = "GRU_Stack_fp16"
            if not rnn_params.linear_before_reset:
                # gen_ctrl.linear_before_reset = 0
                raise ValueError(
                    f"In {rnn_params.name} linear_before_reset == 0 not supported by the Autotiler kernels")
        else:
            raise ValueError("unknown RNN parameter type")

        attrs = {
            'kname': kname,
            'n_cells': rnn_params.n_cells,
            'n_states': rnn_params.n_states,
            'n_inputs': rnn_params.n_inputs,
            'k0': rnn_params.n_input_cells,
            'k1': rnn_params.n_output_cells,
            'revert': 1 if rnn_params.revert else 0,
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
