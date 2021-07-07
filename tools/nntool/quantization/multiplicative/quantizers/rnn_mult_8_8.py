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
from copy import deepcopy

import numpy as np
from graph.types import RNNParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)


@params_type(RNNParameters)
# @options(
#     {
#         'name': 'state_width',
#         'type': int,
#         'choices': ['8', '16'],
#         'help': 'sets width of recurrent state',
#         'default': 8
#     })
@options(
    {
        'name': 'weight_bits',
        'type': int,
        'help': 'how many bits to use in weights',
        'choices' : list(range(2, 9)),
        'default': 8
    },
    {
        'name': 'force_external_size',
        'type': str,
        'help': 'bits to use for features and state',
        'choices': [8, 16],
        'default': 8
    },
    {
        'name': 'narrow_weights',
        'shortcut': 'n',
        'type': bool,
        'help': 'scales filter weights with a representation of both 1 and -1 (i.e. -127 - 127 in 8 bits)',
        'default': True
    },
)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
@option_constraint(force_external_size={8, None})
class RNNMultMult8x8(RescaleConstantMixin, MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        if force_out_qs and any(force_out_q is not None for force_out_q in force_out_qs):
            return None
        in_qs = deepcopy(in_qs)
        in_qs = cls.force_symmetric_and_dtype(in_qs, idx=0, dtype=np.int8)
        if in_qs is None:
            return None
        opts = kwargs['opts']
        # qrecs = kwargs['qrecs']
        G = kwargs['G']

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        o_q = QType.from_min_max_sq(min_val=stats['range_out'][0]['min'],
                                    max_val=stats['range_out'][0]['max'],
                                    dtype=np.int8)

        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
        # quantization_mode: extended, autotiler
        # state_width: 16bit or 8bit
        if np.isclose(in_qs[0].scale, o_q.scale, atol=1e-2):
            LOG.info(
                "node %s has similar input and i_state scales --> "
                "will be generated the same_scale kernel with better performances", params.name)
            params.rnn_same_inout_scale = True
            G.node_options[NodeId(params)] = params.at_options

        for weight_name in ['i_2_i_w', 'r_2_i_w']:
            w_q = in_qs[names[weight_name]]
            in_qs[names[weight_name]] = QType.from_min_max_sq(
                w_q.min_val, w_q.max_val,
                dtype=np.int8, bits=opts['weight_bits'],
                narrow_range=opts.get('narrow_weights', True))

        w_scales = np.maximum(
            in_qs[names['i_2_i_w']].scale, in_qs[names['r_2_i_w']].scale)
        if params.rnn_same_inout_scale:
            in_and_state_scale = np.maximum(in_qs[0].scale, o_q.scale)
            in_qs[0].scale = in_and_state_scale
            o_q.scale = in_and_state_scale
            if not params.rnn_states_as_inputs:
                in_qs[names['i_state']].scale = in_and_state_scale
            i_state_scale = in_and_state_scale
            i_2_a_q = MultMulBiasScaleQType(scale=1.0)  # will be ignored
        else:
            i_state_scale = in_qs[names['i_state']].scale
            i_2_a_q = MultMulBiasScaleQType(
                scale=in_qs[0].scale/i_state_scale)

        in_qs[names['i_2_i_w']].scale = w_scales
        in_qs[names['r_2_i_w']].scale = w_scales
        state_w_scale = i_state_scale * w_scales
        in_qs[names['i_b']].scale = state_w_scale
        in_qs[names['i_b']].dtype = np.int32

        if params.hard_act:
            s_2_s_q = MultMulBiasScaleQType(
                scale=state_w_scale/i_state_scale)
            s_2_o_q = MultMulBiasScaleQType(scale=1.0)  # will be ignored
            act_output_scale = math.pow(2, -7)
        else:
            act_input_scale = math.pow(2, -12)
            act_output_scale = math.pow(2, -15)
            s_2_s_q = MultMulBiasScaleQType(
                scale=state_w_scale/act_input_scale)
            s_2_o_q = MultMulBiasScaleQType(
                scale=act_output_scale/o_q.scale)
        return QRec.scaled(
            in_qs=in_qs,
            out_qs=[o_q],
            s_2_s_q=s_2_s_q,
            i_2_a_q=i_2_a_q,
            s_2_o_q=s_2_o_q,
            scales = {
                'int_scale': act_output_scale,
                'out_scale': o_q.scale
            }
        )
