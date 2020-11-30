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
import math

import numpy as np
from graph.types import ConstantInputParameters, RNNParameters
from quantization.multiplicative.mult_quantization import \
    MultScalableRnnQuantizationRecord
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import \
    MultMulBiasScaleQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.quantization_handler import params_type, options
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)


@params_type(RNNParameters)
@options(
    {
        'name': 'mode',
        'type': str,
        'choices': ['autotiler', 'extended'],
        'help': 'scales state and input separately',
        'default': 'autotiler'
    },
    {
        'name': 'state_width',
        'type': int,
        'choices': ['8', '16'],
        'help': 'sets width of recurrent state',
        'default': 8
    },
    {
        'name': 'by_state',
        'type': bool,
        'help': 'sets width of recurrent state',
        'default': False
    })
class RNNMultMult(RescaleConstantMixin, MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        opts = kwargs['opts']
        qrecs = kwargs['qrecs']
        G = kwargs['G']

        o_q = SymmetricMultQType.from_min_max(min_val=stats['range_out'][0]['min'],
                                              max_val=stats['range_out'][0]['max'],
                                              dtype=out_dtype)
        input_nodes = {RNNParameters.INPUT_NAMES[edge.to_idx]: edge.from_node
                       for edge in G.in_edges(params.name)
                       if isinstance(edge.from_node, ConstantInputParameters)}
        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
        # quantization_mode: extended, autotiler
        # state_width: 16bit or 8bit
        mode = cls.get_options(params, opts)['mode']
        if mode == "extended":
            in_w_scale = in_qs[names['i_2_i_w']].scale * in_qs[0].scale
            state_w_scale = in_qs[names['r_2_i_w']].scale
            i_2_a_q = MultMulBiasScaleQType(scale=in_w_scale/state_w_scale)
            s_2_s_q = MultMulBiasScaleQType(scale=state_w_scale)
            s_2_o_q = MultMulBiasScaleQType(scale=1/o_q.scale)
            cls.rescale_constant(input_nodes['i_b'], state_w_scale, qrecs, dtype=np.int32)
            return MultScalableRnnQuantizationRecord(
                in_qs=in_qs,
                out_qs=[o_q],
                i_2_a_q=i_2_a_q,
                s_2_s_q=s_2_s_q,
                s_2_o_q=s_2_o_q
            )
        elif mode == 'autotiler':
            if np.isclose(in_qs[0].scale, o_q.scale, atol=1e-2):
                LOG.info(
                    "node %s has similar input and i_state scales --> "
                    "will be generated the same_scale kernel with better performances", params.name)
                params.rnn_same_inout_scale = True
                G.node_options[NodeId(params)] = params.at_options

            w_scales = np.maximum(in_qs[names['i_2_i_w']].scale, in_qs[names['r_2_i_w']].scale)
            if params.rnn_same_inout_scale:
                in_and_state_scale = np.maximum(in_qs[0].scale, o_q.scale)
                in_qs[0].scale = in_and_state_scale
                o_q.scale = in_and_state_scale
                cls.rescale_constant(input_nodes['i_state'], in_and_state_scale, qrecs)
                i_state_scale = in_and_state_scale
                i_2_a_q = MultMulBiasScaleQType(scale=1.0)  # will be ignored
            else:
                i_state_scale = in_qs[names['i_state']].scale
                i_2_a_q = MultMulBiasScaleQType(scale=in_qs[0].scale/i_state_scale)

            cls.rescale_constant(input_nodes['i_2_i_w'], w_scales, qrecs)
            cls.rescale_constant(input_nodes['r_2_i_w'], w_scales, qrecs)
            state_w_scale = i_state_scale * w_scales
            cls.rescale_constant(input_nodes['i_b'], state_w_scale, qrecs, dtype=np.int32)
            if params.hard_act:
                s_2_s_q = MultMulBiasScaleQType(scale=state_w_scale/i_state_scale)
                s_2_o_q = MultMulBiasScaleQType(scale=1.0)  # will be ignored
            else:
                act_input_scale = math.pow(2, -12)
                act_output_scale = math.pow(2, -15)
                s_2_s_q = MultMulBiasScaleQType(scale=state_w_scale/act_input_scale)
                s_2_o_q = MultMulBiasScaleQType(scale=act_output_scale/o_q.scale)
            return MultScalableRnnQuantizationRecord(
                in_qs=in_qs,
                out_qs=[o_q],
                s_2_s_q=s_2_s_q,
                i_2_a_q=i_2_a_q,
                s_2_o_q=s_2_o_q,
            )
