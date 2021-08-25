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
from graph.types import LSTMParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from utils.stats_funcs import calc_bits

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)


def get_maxq_val(stats, scale):
    return np.ceil(np.maximum(np.abs(stats['min']), np.abs(stats['max'])) / scale)


def get_max(stat):
    return np.maximum(np.abs(stat['min']), np.abs(stat['max']))


@options(
    {
        'name': 'weight_bits',
        'type': int,
        'help': 'how many bits to use in weights',
        'choices': list(range(2, 9)),
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
@params_type(LSTMParameters)
@in_qs_constraint({'dtype': np.int16})
@out_qs_constraint({'dtype': np.int16})
@option_constraint(force_external_size=16)
class LSTMMultMult16x8(RescaleConstantMixin, MultQuantizionHandler):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_qs = cls.force_symmetric_and_dtype(in_qs, idx=0, dtype=np.int16)
        if in_qs is None:
            return None
        in_qs = deepcopy(in_qs)
        G = kwargs['G']
        opts = kwargs.get('opts', {})

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')

        names = {val: idx for idx, val in enumerate(
            LSTMParameters.INPUT_NAMES)}

        o_q = in_qs[names['i_state']] = QType.from_min_max_sq(
            min_val=stats['range_out'][0]['min'],
            max_val=stats['range_out'][0]['max'],
            dtype=np.int16)
        if force_out_q:
            if force_out_q.zero_point != 0:
                return None
            LOG.warning('on node %s output is being forced from scale %s -> %s',
                        params.name,
                        o_q.scale, force_out_qs[0].scale)
            o_q = force_out_qs[0]

        cell_range = stats.get('range_cell')
        if cell_range is None:
            raise ValueError(
                f'cell range not present in stats for {params.name}')

        # cell range in minimum 1.0
        cell_stat = max(1.0, *[abs(cell_range[var])
                               for var in ['min', 'max']])
        if params.cell_clip and not params.quant_c_state_with_stat:
            cell_max = params.cell_clip
            ratio_c = cell_max / cell_stat
            if not (ratio_c > 0.9 and ratio_c < 1.1):
                msg = (f"C state is forced to a range [-{cell_max}:{cell_max}] different to the one calulated "
                       f"from the inference statistic [-{cell_stat}:{cell_stat}], consider using nodeoption {params.name} "
                       "QUANT_C_STATE_WITH_STAT 1 to force it to be the one calculated")
                LOG.warning('%s', msg)
        else:
            cell_max = cell_stat

        # this limit is driven by the c_in * f + c * i calculation
        # c * i will be in Q24 and we want c_in * f to be scaled to the same
        # abs(f) will be <=1 so the cell int bits cannot exceed 31 - 1 (overflow) - 24 = 6
        cell_limit = pow(2, 6)
        if cell_max > cell_limit:
            LOG.warning(
                'Cell state exceeds %s and will be clipped', cell_limit)
            cell_max = cell_limit

        cell_int_bits = calc_bits(cell_max)
        in_qs[names['c_state']] = QType.from_min_max_sq(
            -cell_max, cell_max, dtype=np.int16)

        LOG.debug("cell bits %d max %d cell range %d",
                  cell_int_bits,
                  cell_max,
                  in_qs[names['c_state']].range)

        # set weight qtypes
        scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['i', 'o', 'c', 'f']}
        for weight_name in [weight_name for scale_pair in scale_pairs.values() for weight_name in scale_pair]:
            w_q = in_qs[names[weight_name]]
            in_qs[names[weight_name]] = QType.from_min_max_sq(
                w_q.min_val,
                w_q.max_val,
                dtype=np.int8,
                bits=opts['weight_bits'],
                narrow_range=opts.get('narrow_weights'))

        # get weight scales
        w_scales = [(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                    for k, (namei, namer) in scale_pairs.items()]

        gate_sum_max = [
            (get_max(stats[f'range_{gate}_gate_i']),
             get_max(stats[f'range_{gate}_gate_r']))
            for gate in ['i', 'o', 'c', 'f']
        ]

        gate_sum_max_bits = [
            (np.ceil(np.log2(gsm_i / (in_qs[0].scale * i_w))),
                np.ceil(np.log2(gsm_r / (o_q.scale * r_w))))
            for (gsm_i, gsm_r), (i_w, r_w) in zip(gate_sum_max, w_scales)]
        
        for gate, (max_i, max_r) in zip(['i', 'o', 'c', 'f'], gate_sum_max_bits):
            if max_i > 30:
                LOG.warning(
                    'max bits in accumulation input %s gate %s - there may be errors',
                    max_i, gate)
            if max_r > 30:
                LOG.warning(
                    'max bits in accumulation state %s gate %s - there may be errors',
                    max_i, gate)

        # LUT activations Q12 -> Q15
        act_in_q = 12
        act_out_q = 15
        int_scale = math.pow(2, -act_in_q)
        out_tanh_sig_scale = math.pow(2, -act_out_q)

        scale_qtypes = {}
        r_pscales = {}
        i_pscales = {}
        scale_qtypes['r_pscales'] = r_pscales
        scale_qtypes['i_pscales'] = i_pscales
        for gate, w_scale, max_bits in zip(['i', 'o', 'c', 'f'], w_scales, gate_sum_max_bits):
            weight_scale_ratio = w_scale[0]/w_scale[1]
            # TODO - decide to scale weights equal
            in_qs[names[f"{gate}_b"]] = QType(scale=int_scale, dtype=np.int32)
            i_pscales[gate] = w_scale[0] * in_qs[0].scale
            scale_qtypes[f"i_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                scale=i_pscales[gate] / int_scale
            )
            qscale.pre_normalization = int(max(8 - (31 - max_bits[0]), 0))
            r_pscales[gate] = w_scale[1] * o_q.scale
            scale_qtypes[f"r_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                scale=r_pscales[gate] / int_scale
            )
            qscale.pre_normalization = int(max(8 - (31 - max_bits[1]), 0))

        r_pscales['state_out_scale'] = o_q.scale
        r_pscales['int_scale'] = int_scale

        # ct = c_in * f + c * i
        # c * i = Q15 * Q15 -> Q30 -> norm(18) -> Q12
        # scale(c_in * f) = Q15 * Q15 prenorm 8 and scale -> Q12
        # ((c_in * f) + (c * i)) in Q12
        # scale -> cell_out
        # tan(ct) -> Q15
        # o * tan(ct) -> Q30
        # prenorm and scale

        # cell in to Q12
        cell_in_scale = (in_qs[names['c_state']].scale *
                         out_tanh_sig_scale / int_scale)
        # cell_out from Q12
        cell_out_scale = int_scale / in_qs[names['c_state']].scale
        # state out from Q30
        state_out_scale = math.pow(2, -(2 * act_out_q)) / o_q.scale

        r_pscales['act_out_scale'] = out_tanh_sig_scale
        r_pscales['c_before_scale'] = int_scale

        scale_qtypes['cell_in_q'] = MultMulBiasScaleQType(scale=cell_in_scale)
        # for 16 bit pre-normalize the scales to give us room
        scale_qtypes['cell_in_q'].pre_normalization = 8
        scale_qtypes['cell_out_q'] = MultMulBiasScaleQType(
            scale=cell_out_scale)
        scale_qtypes['state_out_q'] = MultMulBiasScaleQType(
            scale=state_out_scale)
        scale_qtypes['state_out_q'].pre_normalization = 8
        scale_qtypes['i_qtype'] = QType(q=act_in_q, dtype=np.int32)
        if params.lstm_output_c_state:
            out_qs = [o_q, in_qs[names['c_state']]]
        else:
            out_qs = [o_q]

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=out_qs,
            **scale_qtypes,
        )
