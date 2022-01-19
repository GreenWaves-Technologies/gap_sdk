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
from quantization.quantizer_options import *
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


def roundup(num, is_double):
    chans = 32 if is_double else 16
    return (num//chans) * chans + (chans if num % chans else 0)


def limit_input_precision(params,
                          input_bits,
                          in_q,
                          input_size,
                          narrow,
                          qw,
                          max_correction,
                          extra_correction=0,
                          accumulator_bits=31,
                          out_ranges=None,
                          w_qs=None):
    if out_ranges and all(stat is not None for stat in out_ranges) and w_qs:
        calc_bits = 0
        for stat, w_q in zip(out_ranges, w_qs):
            res_scale = np.min(w_q.scale * in_q.scale)
            max_out = max(abs(stat['min']), abs(stat['max']))
            if max_out == 0:
                continue
            calc_bits = max(calc_bits, math.ceil(math.log2(max_out/res_scale)))
        # add one safety bit if basing off stats
        calc_bits += 1
    else:
        calc_bits = (input_bits + qw - (1 if narrow else 0) + extra_correction +
                     math.ceil(math.log2(input_size)) + 1)
    reduce_by = max(calc_bits - accumulator_bits, 0)
    if reduce_by > max_correction:
        LOG.warning(f"{params.name}: too much dynamic: quantizer estimates {reduce_by} bit overflow. "
                    f"max_precision_limit set to {max_correction}")
    reduce_by = min(reduce_by, max_correction)
    if reduce_by:
        LOG.warning(
            f"{params.name}: too much dynamic: reducing input precision by {reduce_by} bits. ")
        in_q = in_q.reduce_precision(reduce_by)
    return in_q


def calc_weight_q(wnode,
                  real_dim,
                  padded_dim,
                  qw,
                  narrow):
    extra_attrs = {'bit_pack': qw} if qw < 8 else {}
    return QType.from_array_sq(
        wnode.dqvalue,
        dtype=np.uint8,
        bits=qw,
        narrow_range=narrow,
        quantized_dimension=0,
        resize=(
            real_dim,
            padded_dim
        ),
        ne16_decode={
            'type': 'RNN',
            'Ko': real_dim[0],
            'KiReal': real_dim[1],
            'Ki': padded_dim[1],
            'Qw': qw
        },
        no_compression=True,
        **extra_attrs)


def calc_bias_offset(wnode, w_q, in_zero_point):
    # since the weight zero offset is added by NE16 use signed value
    weight_val = wnode.value_as(w_q).astype(np.int32) - w_q.zero_point
    # return zero offset
    return np.sum(
        -in_zero_point.astype(np.int32) * weight_val,
        axis=1, dtype=np.int32)


def calculatate_weight_q(in_qs,
                         in_edges,
                         w_idx,
                         in_zero_point,
                         real_dim,
                         padded_dim,
                         qw,
                         narrow):
    # calculates weight qtype and zero offset bias correction
    in_qs[w_idx] = calc_weight_q(
        in_edges[w_idx].from_node, real_dim, padded_dim, qw, narrow)
    return calc_bias_offset(in_edges[w_idx].from_node, in_qs[w_idx], in_zero_point)


class NE16RNNMultQuantizionHandler(MultQuantizionHandler):

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        opts = kwargs['opts']
        in_dtype = np.uint8 if opts.get(
            'force_external_size', 8) == 8 else np.uint16
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=in_dtype if idx == 0 else np.uint8,
                                      asymmetric=(idx == 0))
                if dim is not None and stats['range_in'][idx] else None
                for idx, dim in enumerate(params.in_dims)]


@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION,
    NARROW_STATE_OPTION,
    MAX_PRECISION_LIMIT_OPTION
)
class RNNMultMultNE16Base(NE16RNNMultQuantizionHandler):
    @classmethod
    def _quantize_rnn(cls, params, in_qs, stats, input_bits, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        in_out_dtype = np.uint16 if input_bits == 16 else np.uint8
        if force_out_qs and any(force_out_q is not None for force_out_q in force_out_qs):
            return None
        in_qs = deepcopy(in_qs)
        if in_qs is None:
            return None
        in_q = in_qs[0]
        opts = kwargs['opts']
        # qrecs = kwargs['qrecs']
        G = kwargs['G']
        in_edges = G.indexed_in_edges(params.name)

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')

        assert in_q.dtype == in_out_dtype

        o_q = QType.from_min_max_sq(min_val=-1,
                                    max_val=1,
                                    dtype=in_out_dtype,
                                    narrow_range=opts['narrow_state'])

        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}

        in_qs[names['i_state']] = o_q

        woff = {}

        int_num_inp = roundup(params.n_inputs, input_bits == 16)
        int_num_states = roundup(params.n_states, input_bits == 16)

        in_qs[names['i_2_i_w']] = calc_weight_q(
            in_edges[names['i_2_i_w']].from_node,
            (params.n_states, params.n_inputs),
            (params.n_states, int_num_inp),
            opts['weight_bits'],
            opts.get('narrow_weights'))

        in_qs[names['r_2_i_w']] = calc_weight_q(
            in_edges[names['r_2_i_w']].from_node,
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
            opts.get('max_precision_limit',
                     MAX_PRECISION_LIMIT_OPTION['default']),
            w_qs=[in_qs[names['i_2_i_w']]],
            out_ranges=[stats.get('range_input_dot')])

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
            w_qs=[in_qs[names['r_2_i_w']]],
            out_ranges=[stats.get('range_state_dot')])

        woff['i_2_i_w'] = calc_bias_offset(
            in_edges[names['i_2_i_w']].from_node, in_qs[names['i_2_i_w']], in_q.zero_point)

        woff['r_2_i_w'] = calc_bias_offset(
            in_edges[names['r_2_i_w']].from_node, in_qs[names['r_2_i_w']], o_q.zero_point)

        i_state_scale = in_qs[names['i_state']].scale
        # rescale input * weight result to state * weight result so that they can be accumulated
        inp_before_scale = in_q.scale * in_qs[names['i_2_i_w']].scale
        state_w_scale = i_state_scale * in_qs[names['r_2_i_w']].scale

        # In 8 bit kernel input is rescaled to state scale
        # In 16 bit kernel input is scaled to LUT act input scale to avoid overflow
        # Bias zero correction is rescaled to state * state_w in both cases
        rescale = inp_before_scale / state_w_scale

        if input_bits == 8:
            i_2_s_q = MultMulBiasScaleQType(
                scale=rescale)
            # 8 bit mode biases are applied by NE16 so need to be multiplied by biases
            # and norm rounding added
            i_zp_b = woff['i_2_i_w'] * i_2_s_q.qbiases.astype(
                np.int32) + (1 << (i_2_s_q.qnorms.astype(np.int32) - 1))
            woff = woff['r_2_i_w']
        else:
            i_2_s_q = MultMulBiasScaleQType(
                scale=((in_q.scale * in_qs[names['i_2_i_w']].scale) /
                       math.pow(2, -12)))
            i_2_s_q.pre_normalization = min(
                opts['weight_bits'], np.min(i_2_s_q.qnorms))
            # in 16 bit mode biases are streamed in so zp corr already in right scale
            # and do not need norm rounding
            i_zp_b = woff['i_2_i_w']
            woff = woff['r_2_i_w']

        # hard activations are only implemented for 8 bit mode at present
        if input_bits == 8 and params.hard_act:
            act_input_scale = i_state_scale
            s_2_s_q = MultMulBiasScaleQType(
                scale=state_w_scale/i_state_scale)
            s_2_o_q = MultMulBiasScaleQType(scale=1.0)  # will be ignored
            act_output_scale = i_state_scale
            act_qtype = QType(dtype=np.int8, scale=act_input_scale,
                              narrow_range=opts.get('narrow_state'))
        else:
            act_input_scale = math.pow(2, -12)
            act_output_scale = math.pow(2, -15)
            act_qtype = None

            s_2_s_q = MultMulBiasScaleQType(
                scale=state_w_scale/act_input_scale)
            if input_bits == 16:
                s_2_s_q.pre_normalization = min(
                    opts['weight_bits'], np.min(s_2_s_q.qnorms))
            s_2_o_q = MultMulBiasScaleQType(
                scale=act_output_scale/o_q.scale)

        if input_bits == 8:
            in_qs[names['i_b']] = QType(
                scale=state_w_scale / s_2_s_q.qbiases, dtype=np.int32, quantized_dimension=0)
            in_qs[names['i_b']].offset = woff * s_2_s_q.qbiases.astype(
                np.int32) + (1 << (s_2_s_q.qnorms.astype(np.int32) - 1))
            if i_zp_b is not None:
                in_qs[names['i_b']].attr.interleaved_values = [i_zp_b]
        else:
            in_qs[names['i_b']] = QType(
                scale=state_w_scale, dtype=np.int32, quantized_dimension=0)
            in_qs[names['i_b']].offset = woff
            # Interleave input zero offset bias with state bias at generation time
            in_qs[names['i_b']].attr.interleaved_values = [i_zp_b]

        return QRec.scaled(
            in_qs=in_qs,
            out_qs=[o_q],
            s_2_s_q=s_2_s_q,
            i_2_s_q=i_2_s_q,
            s_2_o_q=s_2_o_q,
            act_qtype=act_qtype,
            scales={
                'int_scale': act_output_scale,
                'out_scale': o_q.scale,
                'act_input_scale': act_input_scale,
                'inp_after_scale': i_state_scale * in_qs[names['r_2_i_w']].scale,
                'inp_before_scale': inp_before_scale
            },
            ne16=True
        )


@params_type(RNNParameters)
@in_qs_constraint({'dtype': np.uint8})
@out_qs_constraint({'dtype': np.uint8})
@option_constraint(force_external_size={8, None}, use_ne16=True)
class RNNMultMult8x8NE16(RNNMultMultNE16Base):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_rnn(params, in_qs, stats, 8, **kwargs)


@params_type(RNNParameters)
@in_qs_constraint({'dtype': np.uint16})
@out_qs_constraint({'dtype': np.uint16})
@option_constraint(force_external_size=16, use_ne16=True)
class RNNMultMult16x8NE16(RNNMultMultNE16Base):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_rnn(params, in_qs, stats, 16, **kwargs)
