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
from nntool.graph.types import LSTMNode
from nntool.quantization.multiplicative.quantizers.rnn_mult_ne16 import (
    calculatate_weight_q, limit_input_precision, roundup)
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.quantizer_options import *
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from nntool.utils.stats_funcs import calc_bits

from .rnn_mult_ne16 import NE16RNNMultQuantizionHandler, calc_bias_offset, calc_weight_q

LOG = logging.getLogger('nntool.' + __name__)


def get_maxq_val(stats, scale):
    return np.ceil(np.maximum(np.abs(stats['min']), np.abs(stats['max'])) / scale)


def get_max(stat):
    return np.maximum(np.abs(stat['min']), np.abs(stat['max']))


def get_max_or_one(stat):
    gate_max = np.maximum(np.abs(stat['min']), np.abs(stat['max']))
    return np.where(gate_max == 0, 1.0, gate_max)


@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION,
    NARROW_STATE_OPTION,
    FORCE_RNN_1_MINUS_1_OUT,
    MAX_PRECISION_LIMIT_OPTION
)
class LSTMMultMultNE16Base(NE16RNNMultQuantizionHandler):

    @classmethod
    def _quantize_lstm(cls, params, in_qs, stats, input_bits, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_qs and any(force_out_q is not None for force_out_q in force_out_qs):
            return None

        opts = kwargs.get('opts', {})

        if input_bits == 16:
            in_out_dtype = np.uint16
        else:
            in_out_dtype = np.uint8

        if in_qs is None:
            return None
        in_qs = deepcopy(in_qs)
        G = kwargs['G']

        in_q = in_qs[0]

        # if in_q.dtype != in_out_dtype:
        #     cls.check_valid_ranges(params, stats, idx=0, dirs='in')
        #     in_q = in_qs[0] = QType.from_min_max_sq(
        #         min_val=stats['range_in'][0]['min'],
        #         max_val=stats['range_in'][0]['max'],
        #         dtype=in_out_dtype,
        #         asymmetric=True)

        # cls.check_valid_ranges(params, stats, idx=0, dirs='out')

        in_edges = G.indexed_in_edges(params.name)

        names = {val: idx for idx, val in enumerate(
            LSTMNode.INPUT_NAMES)}

        if opts.get('force_rnn_1_minus_1_out'):
            o_q = QType.from_min_max_sq(
                -1.0,
                1.0,
                narrow_range=True,
                dtype=in_out_dtype)
        else:
            o_q = in_qs[names['i_state']] = QType.from_min_max_sq(
                *stats.get_range_out(0, dtype=in_out_dtype),
                dtype=in_out_dtype,
                narrow_range=opts['narrow_state'])

        cell_range = stats.get_stats_key('range_cell')
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
        # cell stays signed since it is used in a haddamard with the int32 streamout
        # in NE16
        in_qs[names['c_state']] = QType.from_min_max_sq(
            -cell_max, cell_max, dtype=np.int16 if input_bits == 16 else np.int8)

        LOG.debug("cell bits %d max %d cell range %d",
                  cell_int_bits,
                  cell_max,
                  in_qs[names['c_state']].range)

        # set weight qtypes
        int_num_inp = roundup(params.n_inputs, input_bits == 16)
        int_num_states = roundup(params.n_states, input_bits == 16)

        for gate in ['i', 'o', 'c', 'f']:
            i_idx = names[f'i_2_{gate}_w']
            r_idx = names[f'r_2_{gate}_w']

            in_qs[i_idx] = calc_weight_q(
                in_edges[i_idx].from_node,
                (params.n_states, params.n_inputs),
                (params.n_states, int_num_inp),
                opts['weight_bits'],
                opts.get('narrow_weights'))

            in_qs[r_idx] = calc_weight_q(
                in_edges[r_idx].from_node,
                (params.n_states, params.n_states),
                (params.n_states, int_num_states),
                opts['weight_bits'],
                opts.get('narrow_weights'))

        in_q = limit_input_precision(
            params,
            input_bits,
            in_q,
            int_num_inp,
            opts['narrow_weights'],
            opts['weight_bits'],
            opts.get('max_precision_limit', MAX_PRECISION_LIMIT_OPTION['default']),
            w_qs=[in_qs[names[f'i_2_{gate}_w']] for gate in ['i', 'o', 'c', 'f']],
            out_ranges=[stats.get_stats_key(f'range_{gate}_gate_i') for gate in ['i', 'o', 'c', 'f']])

        o_q = limit_input_precision(
            params,
            input_bits,
            o_q,
            int_num_states,
            opts['narrow_weights'],
            opts['weight_bits'],
            opts.get('max_precision_limit',
                     MAX_PRECISION_LIMIT_OPTION['default']),
            extra_correction=-1 if opts.get('narrow_state') else 0,
            w_qs=[in_qs[names[f'r_2_{gate}_w']] for gate in ['i', 'o', 'c', 'f']],
            out_ranges=[stats.get_stats_key(f'range_{gate}_gate_r') for gate in ['i', 'o', 'c', 'f']])

        # setup zero offset bias adjustment
        woffs = {}
        for gate in ['i', 'o', 'c', 'f']:
            i_idx = names[f'i_2_{gate}_w']
            r_idx = names[f'r_2_{gate}_w']

            woffs[gate] = [
                calc_bias_offset(in_edges[i_idx].from_node, in_qs[i_idx], in_q.zero_point),
                calc_bias_offset(in_edges[r_idx].from_node, in_qs[r_idx], o_q.zero_point),
            ]


        # get weight scales
        scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['i', 'o', 'c', 'f']}
        w_scales = [(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                    for k, (namei, namer) in scale_pairs.items()]

        gate_sum_max = [
            (get_max_or_one(stats.get_stats_key(f'range_{gate}_gate_i')),
             get_max_or_one(stats.get_stats_key(f'range_{gate}_gate_r')))
            for gate in ['i', 'o', 'c', 'f']
        ]

        gate_sum_max_bits = [
            (np.ceil(np.log2(gsm_i / (in_qs[0].scale * i_w))),
                np.ceil(np.log2(gsm_r / (o_q.scale * r_w))))
            for (gsm_i, gsm_r), (i_w, r_w) in zip(gate_sum_max, w_scales)]

        for gate, (max_i, max_r) in zip(['i', 'o', 'c', 'f'], gate_sum_max_bits):
            if np.max(max_i) > 30:
                LOG.warning(
                    'max bits in accumulation input %s gate %s - there may be errors',
                    max_i, gate)
            if np.max(max_r) > 30:
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

            i_pscales[gate] = w_scale[0] * in_q.scale
            r_pscales[gate] = w_scale[1] * o_q.scale
            if input_bits == 16:
                scale_qtypes[f"i_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                    scale=i_pscales[gate] / int_scale
                )
            else:
                scale_qtypes[f"i_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                    scale=i_pscales[gate] / r_pscales[gate]
                )
            if input_bits == 16:
                i_zp_b = woffs[gate][0]
            else:
                i_zp_b = woffs[gate][0] * qscale.qbiases.astype(
                    np.int32) + (1 << (qscale.qnorms.astype(np.int32) - 1))

            scale_qtypes[f"r_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                scale=r_pscales[gate] / int_scale
            )
            if input_bits == 16:
                r_zp_b = woffs[gate][1]
                in_qs[names[f'{gate}_b']] = QType(
                    dtype=np.int32,
                    scale=r_pscales[gate],
                    offset=r_zp_b,
                    interleaved_values=[i_zp_b],
                    quantized_dimension=0
                )
            else:
                r_zp_b = woffs[gate][1] * qscale.qbiases.astype(
                    np.int32) + (1 << (qscale.qnorms.astype(np.int32) - 1))
                in_qs[names[f'{gate}_b']] = QType(
                    dtype=np.int32,
                    scale=r_pscales[gate] / qscale.qbiases,
                    offset=r_zp_b,
                    interleaved_values=[i_zp_b],
                    quantized_dimension=0
                )

        # NOTE - for 16 bit pre-normalize the scales to give us room but make sure it isn't negative
        if input_bits == 16:
            gate_prenorm = min(np.min([
                np.min(scale_qtypes[f"{inp}_2_{gate}_q"].qnorms) for gate in ['i', 'o', 'c', 'f'] for inp in ['i', 'r']
            ]), 8)
            for gate in ['i', 'o', 'c', 'f']:
                for inp in ['i', 'r']:
                    scale_qtypes[f"{inp}_2_{gate}_q"].pre_normalization = gate_prenorm
        else:
            gate_prenorm = 0

        r_pscales['state_out_scale'] = o_q.scale
        r_pscales['int_scale'] = int_scale

        # ct = c_in * f + c * i
        # c * i = Q15 * Q15 -> Q30 -> norm(18) -> Q12
        # scale(c_in * f) = Qcell * Q15 (prenorm if 16bit) and scale -> Q12
        # ((c_in * f) + (c * i)) in Q12
        # scale -> cell_out
        # tan(ct) -> Q15
        # o * tan(ct) -> Q30
        # prenorm and scale

        # scale result of c_state_1 * f_gate -> Q15
        cell_in_scale = (in_qs[names['c_state']].scale *
                         out_tanh_sig_scale / out_tanh_sig_scale)

        # cell_out from Q15 -> Q7/Q15 scaled
        cell_out_scale = out_tanh_sig_scale / in_qs[names['c_state']].scale

        state_out_scale = out_tanh_sig_scale / o_q.scale

        r_pscales['act_out_scale'] = out_tanh_sig_scale
        r_pscales['c_before_scale'] = int_scale

        scale_qtypes['cell_in_q'] = MultMulBiasScaleQType(scale=cell_in_scale)
        # NOTE - for 16 bit pre-normalize the scales to give us room
        if input_bits == 16:
            scale_qtypes['cell_in_q'].pre_normalization = 8
        scale_qtypes['cell_out_q'] = MultMulBiasScaleQType(
            scale=cell_out_scale)
        scale_qtypes['state_out_q'] = MultMulBiasScaleQType(
            scale=state_out_scale)
        scale_qtypes['i_qtype'] = QType(q=act_in_q, dtype=np.int32)

        out_qs = [o_q]
        if params.output_h_state:
            out_qs.append(in_qs[names['i_state']])
        if params.output_c_state:
            out_qs.append(in_qs[names['c_state']])

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=out_qs,
            ne16=True,
            gate_prenorm=gate_prenorm,
            **scale_qtypes,
        )


@params_type(LSTMNode)
@in_qs_constraint({'dtype': np.uint8})
@out_qs_constraint({'dtype': np.uint8})
@option_constraint(force_external_size={8, None}, use_ne16=True)
class LSTMMultMultNE16UInt8(LSTMMultMultNE16Base):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_lstm(params, in_qs, stats, 8, **kwargs)


@params_type(LSTMNode)
@in_qs_constraint({'dtype': np.uint16})
@out_qs_constraint({'dtype': np.uint16})
@option_constraint(force_external_size=16, use_ne16=True)
class LSTMMultMultNE16UInt16(LSTMMultMultNE16Base):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_lstm(params, in_qs, stats, 16, **kwargs)
