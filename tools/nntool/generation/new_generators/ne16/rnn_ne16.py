# Copyright (C) 2021  GreenWaves Technologies, SAS

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
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import (GlobalArgInfo, GlobalResetArgInfo,
                                             LocalArgInfo)
from generation.bindings import (CommentBindingList, GArgName, GNodeArgEdge,
                                 GNodeArgNode, NoArg, NodeBindingList)
from generation.generators.bindings.float16.rnn_bindings_generator import \
    num_sequences
from generation.generators.globals.global_names import INFOS
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.new_generators.generator_base import (GeneratorBase,
                                                      paramstype, qrec_options)
from graph.types import RNNParameters
from quantization.qtype import QType
from utils.numpy_helpers import interleave

LOG = logging.getLogger("nntool." + __name__)

@paramstype(RNNParameters)
@qrec_options(ne16=True)
class RNNNE16Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
        w_q = qrec.in_qs[names['r_2_i_w']]
        out_q = qrec.out_qs[0]
        out_scale = qrec.cache["s_2_o_q"]
        assert len(w_q.zero_point) == 1
        assert len(out_scale.qbiases) == 1
        assert len(out_scale.qnorms) == 1
        if out_q.dtype == np.uint8:
            if qrec.cache['act_qtype']:
                min_val = qrec.cache['act_qtype'].quantize(-1)
                max_val = qrec.cache['act_qtype'].quantize(1)
            else:
                min_val = max_val = 0
            contents = np.array([
                min_val,
                max_val,
                (-w_q.zero_point[0]).astype(np.int8),
                out_q.zero_point[0],
                0,
                out_scale.qbiases[0].astype(np.int8),
                out_scale.qnorms[0].astype(np.int8),
                0,
                0
            ], dtype=np.int8)
        else:
            out_zp = out_q.zero_point[0].astype(np.uint16)
            contents = np.array([
                0,
                0,
                (-w_q.zero_point[0]).astype(np.int8),
                out_zp & 0xff,
                out_zp >> 8,
                out_scale.qbiases[0].astype(np.int8),
                out_scale.qnorms[0].astype(np.int8),
                qrec.cache["i_2_s_q"].pre_normalization,
                qrec.cache["s_2_s_q"].pre_normalization
            ], dtype=np.int8)
        comment = f"A0: {1} B0: {-1}, ZP: {w_q.zero_point}, OutS: {out_scale.qbiases[0]}, OutN: {out_scale.qnorms[0]}"

        cname, file_name = gen_constant(gen, pnode, pnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)

        gen.globals.append(GlobalArgInfo("int8", cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info,
                                         comment=comment))

        state_scale = qrec.cache["s_2_s_q"]
        if node.rnn_same_inout_scale:
            contents = interleave(state_scale.qbiases, state_scale.qnorms)
        else:
            input_scale = qrec.cache["i_2_s_q"]
            contents = interleave(state_scale.qbiases, input_scale.qbiases,
                                  state_scale.qnorms, input_scale.qnorms)

        cname, file_name = gen_constant(gen, pnode, pnode, "scalenorm")
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=False), contents=contents)
        gen.globals.append(GlobalArgInfo("uint8", cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info,
                                         comment=f"{node.name} scales and norms"))
        if node.rnn_states_as_inputs:
            gen.globals.append(GlobalResetArgInfo(
                f"{node.name}_Reset", 'AT_MEM_L2', 'AT_MEM_UNDEF'))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        names = node.get_name_indexes()

        gen.bindings.append(
            CommentBindingList("Node {} inq {} weightsq {} outq {} biasesq {}", cname,
                               qrec.in_qs[0], qrec.in_qs[names['r_2_i_w']],
                               qrec.out_qs[0], qrec.in_qs[names['i_b']])
        )
        num_seq = num_sequences(node)
        if num_seq > 1:
            gen.locals.append(LocalArgInfo(
                "uint8", f"S{node.step_idx}_StateInternal01"))

        if num_seq > 2:
            gen.locals.append(LocalArgInfo(
                "uint8", f"S{node.step_idx}_StateInternal02"))

        i_state_eparams = in_eparams[names['i_state']]
        if node.rnn_states_as_inputs:
            reset_name = f'{node.name}_Reset'
        else:
            reset_name = 'Reset'
        gen.bindings.append(
            NodeBindingList(cname,
                            GNodeArgEdge(i_state_eparams,
                                         direction="GNA_INOUT"),
                            GNodeArgEdge(f"S{node.step_idx}_StateInternal01",
                                         alias=i_state_eparams,
                                         direction="GNA_INOUT") if num_seq > 1 else NoArg(),
                            GNodeArgEdge(f"S{node.step_idx}_StateInternal02",
                                         alias=f"S{node.step_idx}_StateInternal01",
                                         direction="GNA_INOUT") if num_seq > 2 else NoArg(),
                            GNodeArgEdge(in_eparams[0]),
                            GNodeArgNode(node, "scalenorm"),
                            GNodeArgEdge(in_eparams[names['r_2_i_w']]),
                            GNodeArgEdge(
                                in_eparams[names['i_2_i_w']]) if not node.rnn_same_inout_scale else NoArg(),
                            GNodeArgEdge(in_eparams[names['i_b']]),
                            GNodeArgEdge(out_eparams[0], direction="GNA_OUT"),
                            GNodeArgNode(node, INFOS),
                            GArgName(reset_name)
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        gen.kernels.append(
            RNNNE16Kernel(
                node.name, cname, node, qrec)
        )
        return True


class RNNNE16Kernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
RNN_Stack_NE16("{cname}", {gen_ctrl}, {bias_size}, {feat_size}, {filter_bits},
               {n_cells}, {k0}, {k1}, {dim_state}, {dim_in}, {always_reset}, {revert});
"""

    def __init__(self, node_name, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if params.hard_act:
            gen_ctrl.rnn_use_hardact = 1

        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
        in_qs = qrec.in_qs
        if in_qs[names['i_2_i_w']].bits != in_qs[names['r_2_i_w']].bits:
            ValueError(f'bit width of gates differs in {params.name}')

        attrs = {
            'bias_size': in_qs[names['i_b']].dtype_bits//8,
            'feat_size': -in_qs[0].dtype_bits//8,
            'filter_bits': in_qs[names['i_2_i_w']].bits,
            'n_cells': params.n_cells,
            'k0': params.n_input_cells,
            'k1': params.n_output_cells,
            'dim_state': params.n_states,
            'dim_in': params.n_inputs,
            'always_reset': 0,
            'revert': 1 if params.revert else 0,
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
