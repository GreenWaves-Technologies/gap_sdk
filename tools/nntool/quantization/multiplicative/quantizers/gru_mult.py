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
from graph.types import ConstantInputParameters, GRUParameters
from quantization.multiplicative.mult_quantization import \
    MultScalableGRUQuantizationRecord
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)

BIAS_DTYPE = np.int32
WEIGHTS_DTYPE = np.int8

# pylint: disable=invalid-name
@params_type(GRUParameters)
class RNNMultMult(RescaleConstantMixin, MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        if force_out_qs and any(force_out_q is not None for force_out_q in force_out_qs):
            return None
        in_qs = deepcopy(in_qs)
        # qrecs = kwargs['qrecs']
        G = kwargs['G']

        o_q = QType.from_min_max_sq(min_val=stats['range_out'][0]['min'],
                                    max_val=stats['range_out'][0]['max'],
                                    dtype=out_dtype)

        # input_nodes = {GRUParameters.INPUT_NAMES[edge.to_idx]: edge.from_node
        #                for edge in G.in_edges(params.name)
        #                if isinstance(edge.from_node, ConstantInputParameters)}

        names = {val: idx for idx, val in enumerate(GRUParameters.INPUT_NAMES)}

        # quantization_mode: extended, autotiler
        # state_width: 16bit or 8bit
        # if np.isclose(in_qs[0].scale, o_q.scale, atol=1e-2):
        #     LOG.info(
        #         "node %s has similar input and i_state scales --> "
        #         "will be generated the same_scale kernel with better performance", params.name)
        #     params.rnn_same_inout_scale = True
        #     G.node_options[NodeId(params)] = params.at_options

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
        i_2_z_WR_q = MultMulBiasScaleQType(scale=(wWz_scale * in_scale)/(rWz_scale * state_scale))
        i_2_r_WR_q = MultMulBiasScaleQType(scale=(wWr_scale * in_scale)/(rWr_scale * state_scale))
        i_2_h_WR_q = MultMulBiasScaleQType(scale=(wWh_scale * in_scale)/(rWh_scale * state_scale))

        i_qtype = QType(bits=32, q=12, signed=True, dtype=np.int32)
        h_WR_2_int_q = MultMulBiasScaleQType(scale=(rWh_scale * state_scale)/i_qtype.scale)
        r_WR_2_int_q = MultMulBiasScaleQType(scale=(rWr_scale * state_scale)/i_qtype.scale)
        z_WR_2_int_q = MultMulBiasScaleQType(scale=(rWz_scale * state_scale)/i_qtype.scale)

        if not params.rnn_states_as_inputs:
            in_qs[names['h_state']].scale = state_q.scale
            # cls.rescale_constant(input_nodes['h_state'], state_q.scale, qrecs)
        in_qs[0].scale = in_scale
        o_q.scale = state_scale

        in_qs[names['z_b']].scale = in_scale * rWz_scale
        in_qs[names['z_b']].dtype = BIAS_DTYPE
        # cls.rescale_constant(input_nodes['z_b'], in_scale * rWz_scale, qrecs, dtype=BIAS_DTYPE)
        in_qs[names['r_b']].scale = in_scale * rWr_scale
        in_qs[names['r_b']].dtype = BIAS_DTYPE
        # cls.rescale_constant(input_nodes['r_b'], in_scale * rWr_scale, qrecs, dtype=BIAS_DTYPE)
        in_qs[names['w_h_b']].scale = in_scale * wWh_scale
        in_qs[names['w_h_b']].dtype = BIAS_DTYPE
        # cls.rescale_constant(input_nodes['w_h_b'], in_scale * wWh_scale, qrecs, dtype=BIAS_DTYPE)
        in_qs[names['r_h_b']].scale = in_scale * rWh_scale
        in_qs[names['r_h_b']].dtype = BIAS_DTYPE
        # cls.rescale_constant(input_nodes['r_h_b'], state_scale * rWh_scale, qrecs, dtype=BIAS_DTYPE)

        in_qs[names['w_2_z_w']].scale = wWz_scale
        in_qs[names['w_2_z_w']].dtype = WEIGHTS_DTYPE
        # cls.rescale_constant(input_nodes['w_2_z_w'], wWz_scale, qrecs, dtype=WEIGHTS_DTYPE)
        in_qs[names['w_2_r_w']].scale = wWr_scale
        in_qs[names['w_2_r_w']].dtype = WEIGHTS_DTYPE
        # cls.rescale_constant(input_nodes['w_2_r_w'], wWr_scale, qrecs, dtype=WEIGHTS_DTYPE)
        in_qs[names['w_2_h_w']].scale = wWh_scale
        in_qs[names['w_2_h_w']].dtype = WEIGHTS_DTYPE
        # cls.rescale_constant(input_nodes['w_2_h_w'], wWh_scale, qrecs, dtype=WEIGHTS_DTYPE)
        in_qs[names['r_2_z_w']].scale = rWz_scale
        in_qs[names['r_2_z_w']].dtype = WEIGHTS_DTYPE
        # cls.rescale_constant(input_nodes['r_2_z_w'], rWz_scale, qrecs, dtype=WEIGHTS_DTYPE)
        in_qs[names['r_2_r_w']].scale = rWr_scale
        in_qs[names['r_2_r_w']].dtype = WEIGHTS_DTYPE
        # cls.rescale_constant(input_nodes['r_2_r_w'], rWr_scale, qrecs, dtype=WEIGHTS_DTYPE)
        in_qs[names['r_2_h_w']].scale = rWh_scale
        in_qs[names['r_2_h_w']].dtype = WEIGHTS_DTYPE
        # cls.rescale_constant(input_nodes['r_2_h_w'], rWh_scale, qrecs, dtype=WEIGHTS_DTYPE)

        return MultScalableGRUQuantizationRecord(
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
