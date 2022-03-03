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

from functools import reduce
import logging
import math
from copy import deepcopy

import numpy as np
from graph.types.rnn import GRUParameters
from quantization.multiplicative.quantizers.rnn_mult_ne16 import (
    calculatate_weight_q, limit_input_precision, roundup)
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantizer_options import *
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
from utils.stats_funcs import calc_bits

from .rnn_mult_ne16 import NE16RNNMultQuantizionHandler, calc_bias_offset, calc_weight_q

LOG = logging.getLogger('nntool.' + __name__)


def get_maxq_val(stats, scale):
    return np.ceil(np.maximum(np.abs(stats['min']), np.abs(stats['max'])) / scale)


def get_max(stat):
    return np.maximum(np.abs(stat['min']), np.abs(stat['max']))


def get_max_or_one(stat):
    gate_max = np.maximum(np.abs(stat['min']), np.abs(stat['max']))
    return np.where(gate_max == 0, 1.0, gate_max)

def combine_stats(stats, *keys):
    stats = [stats[k] for k in keys if k in stats]
    if not stats:
        return None
    def reduction(state, item):
        return {'min': min(state['min'], item['min']), 'max': max(state['max'], item['max'])}
    return reduce(reduction, stats)

def combine_qtype_ranges(qtypes, *indexes):
    qtypes = [qtypes[i] for i in indexes if qtypes[i] is not None]
    if not qtypes:
        return None
    def reduction(state, item):
        if state is None:
            return {'min': item.min_val, 'max': item.max_val}
        return {'min': np.min(np.minimum(state['min'], item.min_val)), 'max': np.max(np.maximum(state['max'], item.max_val))}
    return reduce(reduction, qtypes, None)

@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION,
    MAX_PRECISION_LIMIT_OPTION
)
class GRUMultMultNE16Base(NE16RNNMultQuantizionHandler):

    @classmethod
    def _quantize_gru(cls, params, in_qs, stats, input_bits, **kwargs):
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

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')

        in_edges = G.indexed_in_edges(params.name)

        names = {val: idx for idx, val in enumerate(
            GRUParameters.INPUT_NAMES)}

        # o_q = in_qs[names['h_state']] = QType.from_min_max_sq(
        #     min_val=-1,
        #     max_val=1,
        #     dtype=in_out_dtype,
        #     narrow_range=opts['narrow_state'])

        o_q = in_qs[names['h_state']] = QType(
            q=15 if input_bits == 16 else 7,
            zero_point=in_out_dtype(math.pow(2, input_bits-1)),
            min_val=-1,
            max_val=1,
            dtype=in_out_dtype)

        # set weight qtypes
        int_num_inp = roundup(params.n_inputs, input_bits == 16)
        int_num_states = roundup(params.n_states, input_bits == 16)

        for gate in ['z', 'r', 'h']:
            i_idx = names[f'w_2_{gate}_w']
            r_idx = names[f'r_2_{gate}_w']

            in_qs[i_idx] = calc_weight_q(in_edges[i_idx].from_node, (params.n_states, params.n_inputs),
                (params.n_states, int_num_inp),
                opts['weight_bits'],
                opts.get('narrow_weights'))

            in_qs[r_idx] = calc_weight_q(in_edges[r_idx].from_node, (params.n_states, params.n_states),
                (params.n_states, int_num_states),
                opts['weight_bits'],
                opts.get('narrow_weights'))

        # check for overflow
        in_q = limit_input_precision(
            params,
            input_bits,
            in_q,
            int_num_inp,
            opts['narrow_weights'],
            opts['weight_bits'],
            opts.get('max_precision_limit', MAX_PRECISION_LIMIT_OPTION['default']),
            w_qs=[in_qs[names[f'w_2_{gate}_w']] for gate in ['z', 'r']],
            out_ranges=[stats.get(f'range_{gate}_gate_inp') for gate in ['z', 'r']])

        # The state out is not limited but include this to print warnings
        o_q = limit_input_precision(
            params,
            input_bits,
            o_q,
            int_num_states,
            opts['narrow_weights'],
            opts['weight_bits'],
            0,
            w_qs=[in_qs[names[f'r_2_{gate}_w']] for gate in ['z', 'r', 'h']],
            out_ranges=[stats.get(f'range_{gate}_gate_state') for gate in ['z', 'r', 'h']])

        # setup zero offset bias adjustment
        woffs = {}
        for gate in ['z', 'r', 'h']:
            i_idx = names[f'w_2_{gate}_w']
            r_idx = names[f'r_2_{gate}_w']

            woffs[gate] = [
                calc_bias_offset(in_edges[i_idx].from_node, in_qs[i_idx], in_q.zero_point),
                calc_bias_offset(in_edges[r_idx].from_node, in_qs[r_idx], o_q.zero_point),
            ]

        # get weight scales
        scale_pairs = {chan: ('w_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['z', 'r', 'h']}
        w_scales = [(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                    for k, (namei, namer) in scale_pairs.items()]

        gate_sum_max = [
            (get_max_or_one(stats[f'range_{gate}_gate_inp']),
             get_max_or_one(stats[f'range_{gate}_gate_state']))
            for gate in ['z', 'r', 'h']
        ]

        gate_sum_max_bits = [
            (np.ceil(np.log2(gsm_i / (in_qs[0].scale * i_w))),
                np.ceil(np.log2(gsm_r / (o_q.scale * r_w))))
            for (gsm_i, gsm_r), (i_w, r_w) in zip(gate_sum_max, w_scales)]

        for gate, (max_i, max_r) in zip(['z', 'r', 'h'], gate_sum_max_bits):
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
        for gate, w_scale, max_bits in zip(['z', 'r', 'h'], w_scales, gate_sum_max_bits):
            weight_scale_ratio = w_scale[0]/w_scale[1]
            # TODO - decide to scale weights equal

            i_pscales[gate] = w_scale[0] * in_q.scale
            r_pscales[gate] = w_scale[1] * o_q.scale
            # h gate input is added manually to state in Q12
            if input_bits == 16 or gate == 'h':
                scale_qtypes[f"w_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                    scale=i_pscales[gate] / int_scale
                )
            else:
                scale_qtypes[f"w_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                    scale=i_pscales[gate] / r_pscales[gate]
                )
            if input_bits == 16:
                i_zp_b = woffs[gate][0]
                if gate == "h":
                    in_qs[names['w_h_b']] = QType(
                        dtype=np.int32,
                        scale=i_pscales[gate],
                        offset=i_zp_b,
                        quantized_dimension=0
                    )
            else:
                i_zp_b = woffs[gate][0] * qscale.qbiases.astype(
                    np.int32) + (1 << (qscale.qnorms.astype(np.int32) - 1))
                if gate == "h":
                    in_qs[names['w_h_b']] = QType(
                        dtype=np.int32,
                        scale=i_pscales[gate] / qscale.qbiases,
                        offset=i_zp_b,
                        quantized_dimension=0
                    )

            scale_qtypes[f"r_2_{gate}_q"] = qscale = MultMulBiasScaleQType(
                scale=r_pscales[gate] / int_scale
            )

            if gate == 'h':
                bias_name = 'r_h_b'
                interleaved_values = None
            else:
                bias_name = f'{gate}_b'
                interleaved_values = [i_zp_b]
            if input_bits == 16:
                r_zp_b = woffs[gate][1]
                in_qs[names[bias_name]] = QType(
                    dtype=np.int32,
                    scale=r_pscales[gate],
                    offset=r_zp_b,
                    interleaved_values=interleaved_values,
                    quantized_dimension=0
                )
            else:
                r_zp_b = woffs[gate][1] * qscale.qbiases.astype(
                    np.int32) + (1 << (qscale.qnorms.astype(np.int32) - 1))
                in_qs[names[bias_name]] = QType(
                    dtype=np.int32,
                    scale=r_pscales[gate] / qscale.qbiases,
                    offset=r_zp_b,
                    interleaved_values=interleaved_values,
                    quantized_dimension=0
                )

        # NOTE - for 16 bit pre-normalize the scales to give us room but make sure it isn't negative
        if input_bits == 16:
            gate_prenorm = min(np.min([
                np.min(scale_qtypes[f"{inp}_2_{gate}_q"].qnorms) for gate in ['z', 'r', 'h'] for inp in ['w', 'r']
            ]), 8)
            for gate in ['z', 'r', 'h']:
                for inp in ['w', 'r']:
                    scale_qtypes[f"{inp}_2_{gate}_q"].pre_normalization = gate_prenorm
        else:
            gate_prenorm = 0

        scales = {
            'i': i_pscales,
            'r': r_pscales,
            'state': o_q.scale,
            'in': in_q.scale,
            'act_in': int_scale,
            'act_out': out_tanh_sig_scale,
            'act_in_q': act_in_q,
            'act_out_q': act_out_q   
        }
        scale_qtypes['i_qtype'] = QType(q=act_in_q, dtype=np.int32)

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=[o_q],
            ne16=True,
            gate_prenorm=gate_prenorm,
            scales=scales,
            **scale_qtypes,
        )


@params_type(GRUParameters)
@in_qs_constraint({'dtype': np.uint8})
@out_qs_constraint({'dtype': np.uint8})
@option_constraint(force_external_size={8, None}, use_ne16=True)
class GRUMultMultNE16UInt8(GRUMultMultNE16Base):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_gru(params, in_qs, stats, 8, **kwargs)


@params_type(GRUParameters)
@in_qs_constraint({'dtype': np.uint16})
@out_qs_constraint({'dtype': np.uint16})
@option_constraint(force_external_size=16, use_ne16=True)
class GRUMultMultNE16UInt16(GRUMultMultNE16Base):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_gru(params, in_qs, stats, 16, **kwargs)
