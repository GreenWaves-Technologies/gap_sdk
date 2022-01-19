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
from copy import deepcopy

import numpy as np
from graph.types import GRUParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantizer_options import *
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

BIAS_DTYPE = np.int32
WEIGHTS_DTYPE = np.int8

# pylint: disable=invalid-name


@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION
)
@params_type(GRUParameters)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
@option_constraint(force_external_size={8, None}, use_ne16={False, None})
class GRUMult8x8(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        if force_out_qs and any(force_out_q is not None for force_out_q in force_out_qs):
            return None
        in_qs = cls.force_symmetric_and_dtype(in_qs, idx=0)
        if in_qs is None:
            return None
        in_qs = deepcopy(in_qs)
        opts = kwargs['opts']

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        o_q = QType.from_min_max_sq(min_val=stats['range_out'][0]['min'],
                                    max_val=stats['range_out'][0]['max'],
                                    dtype=out_dtype)

        names = {val: idx for idx, val in enumerate(GRUParameters.INPUT_NAMES)}
        edges = kwargs['G'].indexed_in_edges(params.name)

        for gate in ['r', 'z', 'h']:
            w_q = in_qs[names[f'w_2_{gate}_w']]
            in_qs[names[f'w_2_{gate}_w']] = QType.from_min_max_sq(
                w_q.min_val, w_q.max_val,
                dtype=np.int8, bits=opts['weight_bits'],
                narrow_range=opts.get('narrow_weights', True),
                dont_generate_value=True)
            w_q = in_qs[names[f'r_2_{gate}_w']]
            in_qs[names[f'r_2_{gate}_w']] = QType.from_min_max_sq(
                w_q.min_val, w_q.max_val,
                dtype=np.int8, bits=opts['weight_bits'],
                narrow_range=opts.get('narrow_weights', True),
                concatenated_nodes=[edges[names[f'w_2_{gate}_w']].from_node.name])

        if params.rnn_same_inout_scale:
            wWz_scale = rWz_scale = np.maximum(
                in_qs[names['w_2_z_w']].scale, in_qs[names['r_2_z_w']].scale)
            wWr_scale = rWr_scale = np.maximum(
                in_qs[names['w_2_r_w']].scale, in_qs[names['r_2_r_w']].scale)
            wWh_scale = rWh_scale = np.maximum(
                in_qs[names['w_2_h_w']].scale, in_qs[names['r_2_h_w']].scale)
            i_2_z_WR_q = i_2_r_WR_q = i_2_h_WR_q = None
            in_q = state_q = QType(bits=8, q=7, signed=True, dtype=np.int8)
            in_scale = state_scale = in_q.scale
        else:
            wWz_scale = in_qs[names['w_2_z_w']].scale
            wWr_scale = in_qs[names['w_2_r_w']].scale
            wWh_scale = in_qs[names['w_2_h_w']].scale
            rWz_scale = in_qs[names['r_2_z_w']].scale
            rWr_scale = in_qs[names['r_2_r_w']].scale
            rWh_scale = in_qs[names['r_2_h_w']].scale
            in_scale = in_qs[0].scale
            in_q = in_qs[0]
            state_q = QType(bits=8, q=7, signed=True, dtype=np.int8)
            state_scale = state_q.scale
        i_2_z_WR_q = MultMulBiasScaleQType(
            scale=(wWz_scale * in_scale)/(rWz_scale * state_scale))
        i_2_r_WR_q = MultMulBiasScaleQType(
            scale=(wWr_scale * in_scale)/(rWr_scale * state_scale))
        i_2_h_WR_q = MultMulBiasScaleQType(
            scale=(wWh_scale * in_scale)/(rWh_scale * state_scale))

        i_qtype = QType(bits=32, q=12, signed=True, dtype=np.int32)
        h_WR_2_int_q = MultMulBiasScaleQType(
            scale=(rWh_scale * state_scale)/i_qtype.scale)
        r_WR_2_int_q = MultMulBiasScaleQType(
            scale=(rWr_scale * state_scale)/i_qtype.scale)
        z_WR_2_int_q = MultMulBiasScaleQType(
            scale=(rWz_scale * state_scale)/i_qtype.scale)

        if not params.rnn_states_as_inputs:
            in_qs[names['h_state']].scale = state_q.scale
        in_qs[0].scale = in_scale
        o_q.scale = state_scale

        in_qs[names['z_b']] = QType(scale=in_scale * rWz_scale, dtype=BIAS_DTYPE)
        in_qs[names['r_b']] = QType(scale=in_scale * rWr_scale, dtype=BIAS_DTYPE)
        in_qs[names['w_h_b']] = QType(scale=in_scale * wWh_scale, dtype=BIAS_DTYPE)
        in_qs[names['r_h_b']] = QType(scale=in_scale * rWh_scale, dtype=BIAS_DTYPE)

        in_qs[names['w_2_z_w']].scale = wWz_scale
        in_qs[names['w_2_r_w']].scale = wWr_scale
        in_qs[names['w_2_h_w']].scale = wWh_scale
        in_qs[names['r_2_z_w']].scale = rWz_scale
        in_qs[names['r_2_r_w']].scale = rWr_scale
        in_qs[names['r_2_h_w']].scale = rWh_scale

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=[o_q],
            i_2_z_WR_q=i_2_z_WR_q,
            i_2_r_WR_q=i_2_r_WR_q,
            i_2_h_WR_q=i_2_h_WR_q,
            h_WR_2_int_q=h_WR_2_int_q,
            r_WR_2_int_q=r_WR_2_int_q,
            z_WR_2_int_q=z_WR_2_int_q,
            i_qtype=i_qtype,
            scales={
                'w_2_z_w': wWz_scale,
                'w_2_r_w': wWr_scale,
                'w_2_h_w': wWh_scale,
                'r_2_z_w': rWz_scale,
                'r_2_r_w': rWr_scale,
                'r_2_h_w': rWh_scale,
                'in': [in_scale],
                'state': state_scale,
                'out': [state_scale]
            }
        )
