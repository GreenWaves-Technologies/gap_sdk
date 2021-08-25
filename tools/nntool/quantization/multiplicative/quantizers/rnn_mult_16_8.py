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
@in_qs_constraint({'dtype': np.int16})
@out_qs_constraint({'dtype': np.int16})
@option_constraint(force_external_size=16)
class RNNMultMult16x8(RescaleConstantMixin, MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        if force_out_qs and any(force_out_q is not None for force_out_q in force_out_qs):
            return None
        in_qs = deepcopy(in_qs)
        in_qs = cls.force_symmetric_and_dtype(in_qs, dtype=np.int16, idx=0)
        if in_qs is None:
            return None
        opts = kwargs['opts']
        # qrecs = kwargs['qrecs']
        G = kwargs['G']

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')

        o_q = QType(q=15, dtype=np.int16)
        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
        in_qs[names['i_state']] = o_q

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
                dtype=np.int16, bits=opts['weight_bits'],
                narrow_range=opts.get('narrow_weights', True))

        act_input_scale = math.pow(2, -12)

        i_2_a_q = MultMulBiasScaleQType(
            scale=in_qs[0].scale * in_qs[names['i_2_i_w']].scale/act_input_scale)

        in_qs[names['i_b']].scale = o_q.scale * in_qs[names['r_2_i_w']].scale
        in_qs[names['i_b']].dtype = np.int32
        # cls.rescale_constant(input_nodes['i_b'], state_w_scale, qrecs, dtype=np.int32)
        act_output_scale = math.pow(2, -15)
        s_2_s_q = MultMulBiasScaleQType(
            scale=o_q.scale * in_qs[names['r_2_i_w']].scale/act_input_scale)
        return QRec.scaled(
            in_qs=in_qs,
            out_qs=[o_q],
            s_2_s_q=s_2_s_q,
            i_2_a_q=i_2_a_q,
            scales = {
                'int_scale': act_output_scale,
                'out_scale': o_q.scale
            }
        )
