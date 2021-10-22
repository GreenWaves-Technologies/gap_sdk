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
from graph.types import GRUParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantizer_options import (FORCE_EXTERNAL_SIZE_OPTION,
                                            NARROW_STATE_OPTION,
                                            NARROW_WEIGHTS_OPTION,
                                            NE16_WEIGHT_BITS_OPTION,
                                            USE_NE16_OPTION)
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)

BIAS_DTYPE = np.int32
WEIGHTS_DTYPE = np.int8

# pylint: disable=invalid-name


@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION,
    NARROW_STATE_OPTION
)
@params_type(GRUParameters)
@in_qs_constraint({'dtype': np.int16})
@out_qs_constraint({'dtype': np.int16})
@option_constraint(force_external_size=16, use_ne16={None, False})
class GRUMult16x8(RescaleConstantMixin, MultQuantizionHandler):
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

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')

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

        wWz_scale = in_qs[names['w_2_z_w']].scale
        wWr_scale = in_qs[names['w_2_r_w']].scale
        wWh_scale = in_qs[names['w_2_h_w']].scale
        rWz_scale = in_qs[names['r_2_z_w']].scale
        rWr_scale = in_qs[names['r_2_r_w']].scale
        rWh_scale = in_qs[names['r_2_h_w']].scale
        in_scale = in_qs[0].scale
        state_q_bits = 14 if opts.get('narrow_state', False) else 15
        state_q = QType(bits=16, q=state_q_bits, signed=True, dtype=np.int16)
        state_scale = state_q.scale
        i_qtype = QType(bits=32, q=12, signed=True, dtype=np.int32)
        int_scale = i_qtype.scale
        act_qtype = QType(bits=32, q=15, signed=True, dtype=np.int32)

        input_z_w_internal = MultMulBiasScaleQType(
            scale=(wWz_scale * in_scale)/int_scale)
        input_r_w_internal = MultMulBiasScaleQType(
            scale=(wWr_scale * in_scale)/int_scale)
        input_h_w_internal = MultMulBiasScaleQType(
            scale=(wWh_scale * in_scale)/int_scale)

        state_h_w_internal = MultMulBiasScaleQType(
            scale=(rWh_scale * state_scale)/int_scale)
        state_r_w_internal = MultMulBiasScaleQType(
            scale=(rWr_scale * state_scale)/int_scale)
        state_z_w_internal = MultMulBiasScaleQType(
            scale=(rWz_scale * state_scale)/int_scale)

        in_qs[names['h_state']] = state_q
        o_q = state_q

        in_qs[names['z_b']].scale = int_scale
        in_qs[names['z_b']].dtype = BIAS_DTYPE
        in_qs[names['r_b']].scale = int_scale
        in_qs[names['r_b']].dtype = BIAS_DTYPE
        in_qs[names['w_h_b']].scale = in_scale * wWh_scale
        in_qs[names['w_h_b']].dtype = BIAS_DTYPE
        in_qs[names['r_h_b']].scale = state_scale * rWh_scale
        in_qs[names['r_h_b']].dtype = BIAS_DTYPE

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=[o_q],
            input_z_w_internal=input_z_w_internal,
            input_r_w_internal=input_r_w_internal,
            input_h_w_internal=input_h_w_internal,
            state_h_w_internal=state_h_w_internal,
            state_r_w_internal=state_r_w_internal,
            state_z_w_internal=state_z_w_internal,
            i_qtype=i_qtype,
            act_qtype=act_qtype,
            scales={
                'w_2_z_w': wWz_scale,
                'w_2_r_w': wWr_scale,
                'w_2_h_w': wWh_scale,
                'r_2_z_w': rWz_scale,
                'r_2_r_w': rWr_scale,
                'r_2_h_w': rWh_scale,
                'in': [in_scale],
                'state': state_scale,
                'out': [state_scale],
                'act': math.pow(2, -15)
            }
        )
