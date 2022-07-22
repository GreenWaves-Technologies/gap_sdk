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
from nntool.graph.types import RNNNode
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.quantizer_options import *
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(RNNNode)
@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION
)
@in_qs_constraint({'dtype': np.int16})
@out_qs_constraint({'dtype': np.int16})
@option_constraint(force_external_size=16, use_ne16={False, None})
class RNNMultMult16x8(MultQuantizionHandler):
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

        o_q = QType(q=15, dtype=np.int16)
        names = {val: idx for idx, val in enumerate(RNNNode.INPUT_NAMES)}
        in_qs[names['i_state']] = o_q

        if np.isclose(in_qs[0].scale, o_q.scale, atol=1e-2):
            LOG.info(
                "node %s has similar input and i_state scales --> "
                "will be generated the same_scale kernel with better performances", params.name)
            params.rnn_same_inout_scale = True
            G.node_options[params.name] = params.at_options

        edges = G.indexed_in_edges(params.name)
        w_q = in_qs[names['i_2_i_w']]
        in_qs[names['i_2_i_w']] = QType.from_min_max_sq(
            w_q.min_val, w_q.max_val,
            dtype=np.int8, bits=opts['weight_bits'],
            narrow_range=opts.get('narrow_weights', True),
            dont_generate_value=True)

        w_q = in_qs[names['r_2_i_w']]
        in_qs[names['r_2_i_w']] = QType.from_min_max_sq(
            w_q.min_val, w_q.max_val,
            dtype=np.int8, bits=opts['weight_bits'],
            narrow_range=opts.get('narrow_weights', True),
            concatenated_nodes=[edges[names['i_2_i_w']].from_node.name])

        act_input_scale = math.pow(2, -12)

        i_2_a_q = MultMulBiasScaleQType(
            scale=in_qs[0].scale * in_qs[names['i_2_i_w']].scale/act_input_scale)
        i_2_a_q.pre_normalization = min(
            opts['weight_bits'], np.min(i_2_a_q.qnorms))

        in_qs[names['i_b']] = QType(scale=o_q.scale * in_qs[names['r_2_i_w']].scale, dtype=np.int32)
        act_output_scale = math.pow(2, -15)
        s_2_s_q = MultMulBiasScaleQType(
            scale=o_q.scale * in_qs[names['r_2_i_w']].scale/act_input_scale)
        s_2_s_q.pre_normalization = min(
            opts['weight_bits'], np.min(s_2_s_q.qnorms))

        out_qs = [o_q]
        if params.output_h_state:
            out_qs.append(o_q)

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=out_qs,
            s_2_s_q=s_2_s_q,
            i_2_a_q=i_2_a_q,
            scales={
                'int_scale': act_output_scale,
                'out_scale': o_q.scale
            }
        )
