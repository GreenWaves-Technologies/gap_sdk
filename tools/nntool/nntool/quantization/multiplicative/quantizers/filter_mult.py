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
from nntool.graph.types import (Conv2DNode, LinearNode, FusionInputNode,
                         HSigmoidNode,
                         ReluNode, SigmoidNode)
from nntool.graph.types.constant_input import ConstantInputNode
from nntool.graph.types.tensor_arithmetic import (MatMulTransposedNode)
from nntool.quantization.clipping import get_clip
from nntool.quantization.multiplicative.quantizers.rnn_mult_ne16 import \
    limit_input_precision
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

AT_SW_KER_IN_ORDER = [['c', 'h', 'w'], ['out_c', 'in_c', 'h', 'w'], ['out_c']]
AT_SW_KER_OUT_ORDER = [['c', 'h', 'w']]
AT_SW_KER_HWC_IN_ORDER = [['h', 'w', 'c'], [
    'out_c', 'h', 'w', 'in_c'], ['out_c']]
AT_SW_KER_HWC_OUT_ORDER = [['h', 'w', 'c']]
AT_NE16_KER_IN_ORDER = [['h', 'w', 'c'], [
    'out_c', 'in_c', 'h', 'w'], ['out_c']]
AT_NE16_KER_OUT_ORDER = [['h', 'w', 'c']]


def can_ne16(params, G=None, cur_G=None, fusion=None, **kwargs):
    if not isinstance(params, (Conv2DNode, LinearNode, MatMulTransposedNode)):
        return False
    if isinstance(params, Conv2DNode):
        if (params.is_depthwise_conv() and (params.filter_dim.w != 3 or params.filter_dim.h != 3)):
            return False
    elif isinstance(params, MatMulTransposedNode):
        in_nodes = [edge.from_node for edge in cur_G.indexed_in_edges(params)]
        if fusion:
            fusion_in_nodes = [
                edge.from_node for edge in G.indexed_in_edges(fusion)]
            in_nodes = [fusion_in_nodes[node.idx] if isinstance(node, FusionInputNode) else node
                        for node in in_nodes]
        if not isinstance(in_nodes[1], ConstantInputNode):
            return False
    return True


def check_option(option, val):
    if isinstance(option, set):
        return val in option
    return val == option


def check_filter_options(is_ne16, input_size, output_size):
    def check_options(params, opts=None, **kwargs):
        if not check_option(input_size, opts.get('force_input_size')):
            return False
        if not check_option(output_size, opts.get('force_output_size')):
            return False
        if opts.get('force_ne16'):
            return is_ne16
        if not opts.get('use_ne16'):
            return not is_ne16
        return is_ne16 == can_ne16(params, **kwargs)
    return check_options


@options(
    QUANTIZED_DIMENSION_OPTION,
    NARROW_WEIGHTS_OPTION,
    NE16_WEIGHT_BITS_OPTION,
    USE_NE16_OPTION,
    FORCE_NE16_OPTION,
    ALLOW_ASYMMETRIC_OUT_OPTION,
    FORCE_INPUT_SIZE_OPTION,
    FORCE_OUTPUT_SIZE_OPTION,
    HWC_OPTION,
    MAX_PRECISION_LIMIT_OPTION,
)
# pylint: disable=abstract-method
class FilterMultBase(MultQuantizionHandler):
    @classmethod
    def get_quantized_dimension(cls, params, opts):
        if opts['quantized_dimension'] == 'tensor':
            return None
        elif opts['quantized_dimension'] == 'channel':
            return params.filter_dim.get_order_idx('out_c')
        return None

    @classmethod
    def get_weights_node(cls, G, params):
        edges = G.indexed_in_edges(params.name)
        if len(edges) != 3:
            raise ValueError(f"didn't find 3 input edges on {params.name}")
        return edges[1].from_node

    @classmethod
    def get_biases_node(cls, G, params):
        edges = G.indexed_in_edges(params.name)
        if len(edges) != 3:
            raise ValueError(f"didn't find 3 input edges on {params.name}")
        return edges[2].from_node

    @staticmethod
    def calculate_bias_offset(params, in_q, weights_node, weights_q, o_q):
        # calculate bias offset - this will be added to the bias in the kernel
        # it is already in quantized form
        axis = tuple([idx for idx in range(len(params.filter_dim.actual_shape)) if idx !=
                      params.filter_dim.get_order_idx('out_c')])
        bias_offset = np.zeros((params.filter_dim.out_c, ), dtype=np.int32)

        if in_q.zero_point != 0:
            # input zero correction is sum(W * Zin) by out_c if weights are channel scaled
            bias_offset -= np.sum(np.multiply(in_q.zero_point,
                                              weights_node.value_as(weights_q).astype(
                                                  np.int32) - weights_q.zero_point,
                                              dtype=np.int32),
                                  dtype=np.int32,
                                  axis=axis)
        if o_q.zero_point != 0:
            # output zero correction is So/(Si * Sw) * ZPo by out_c if weights are channel scaled
            scale = o_q.scale / (in_q.scale * weights_q.scale)
            bias_offset += np.floor((o_q.zero_point *
                                     scale) + 0.5).astype(np.int32)
        if np.all(bias_offset == 0):
            return None
        return bias_offset

    # @classmethod
    # def get_min_max(cls, fusion, stats, all_stats, params):
    #     min_val, max_val = None, None
    #     if fusion:
    #         # activation directly after filter
    #         if fusion.fusion_type in ['conv_active_pool', 'conv_active']:
    #             act_node = fusion.contained_nodes()[1]
    #         # activation after pool layer - if max will be swapped to conv_active_pool
    #         elif fusion.fusion_type == 'conv_pool_active' and fusion.contained_nodes()[1].pool_type == 'max':
    #             act_node = fusion.contained_nodes()[2]
    #         else:
    #             act_node = None
    #         if act_node:
    #             if isinstance(act_node, HSigmoidNode):
    #                 # Hard sigmoid implements a RELU, be sure 6 can be represented
    #                 min_val, max_val = 0, 6
    #             elif isinstance(act_node, SigmoidNode):
    #                 # Guarantee Q12 input to sigmoid
    #                 min_val, max_val = -8, 8
    #             elif isinstance(act_node, ReluNode):
    #                 # Take stats from activation after the convolution
    #                 stats = all_stats.get((fusion.name, act_node.name))

    #     if min_val is None or max_val is None:
    #         min_val, max_val = stats.get_range_out(0)
    #     return min_val, max_val


class FilterSWMultBase(FilterMultBase):
    @classmethod
    def _quantize_sw(cls, params, in_qs, stats, in_out_dtype, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs.get('opts', {})
        fusion = kwargs.get('fusion', None)
        LOG.debug('selecting SQ8 software kernel filter quantizer')
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        G = kwargs['G']
        in_q = in_qs[0]

        if not in_q.symmetric and (not opts['allow_asymmetric_out'] or ((isinstance(params, Conv2DNode) and params.padding.has_padding))):
            in_q = QType.from_min_max_sq(
                *stats.get_range_in(0),
                dtype=in_out_dtype)
        # if not forced we can try asymmetric
        # elif (opts['allow_asymmetric_out'] and isinstance(params, Conv2DNode) and not in_q.forced and
        #       not in_q.symmetric and not params.padding.has_padding):
        #     in_q = QType.from_min_max_sq(
        #         *stats.get_range_in(0),
        #         dtype=in_out_dtype,
        #         asymmetric=True)

        if opts['weight_bits'] != 8:
            LOG.warning(
                'sub byte weights quantization requested but NE16 kernel not selected')
        weights_node = cls.get_weights_node(G, fusion if fusion else params)
        weights_q = QType.from_array_sq(arr=weights_node.dqvalue,
                                        quantized_dimension=cls.get_quantized_dimension(
                                            params, opts),
                                        dtype=np.int8, narrow_range=opts['narrow_weights'])

        # min_val, max_val = cls.get_min_max(
        #     fusion, stats, kwargs['all_stats'], params)

        if force_out_q:
            o_q = force_out_q
            # can't be forced to something not in_out_dtype or int32
            if o_q.dtype != in_out_dtype and o_q.dtype != np.int32:
                return None
            LOG.debug(f'node {params.name} output forced to range {o_q.min}/{o_q.max} '
                      f'{"asymmetric" if o_q.asymmetric else "symmetric"}')
        else:
            out_bits = 8 if in_out_dtype == np.int8 else 16
            min_val, max_val = stats.get_range_out(0, bits=out_bits)
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=in_out_dtype,
                                        asymmetric=opts['allow_asymmetric_out'])
        biases_node = cls.get_biases_node(G, fusion if fusion else params)
        biases_scale = weights_q.scale * in_q.scale
        max_int32 = np.iinfo(np.int32).max
        bias_overflow = np.abs(biases_node.dqvalue / biases_scale) > max_int32
        if np.any(bias_overflow):
            weights_scales = weights_q.scale
            weights_scales[bias_overflow] = 1
            weights_q.scale = weights_scales
            biases_scale = weights_q.scale * in_q.scale
        biases_q = QType(
            dtype=np.int32, scale=weights_q.scale * in_q.scale, quantized_dimension=0)
        biases_bits = np.ceil(np.log2(np.abs(biases_q.quantize(biases_node.dqvalue))))
        max_accum_bits = np.maximum((np.ceil(np.log2(params.filter_dim.sz)) + 7 + 7), biases_bits)
        available_bits = 31 - max_accum_bits
        mul_biases_q = MultMulBiasScaleQType(
            dtype=np.uint8,
            available_bits=available_bits,
            scale=in_q.scale * weights_q.scale / o_q.scale
        )

        # returning the new weights and biases qs will force backprop

        # calculate bias offset - this will be added to the bias in the kernel
        # it is already in quantized form
        biases_q.offset = FilterSWMultBase.calculate_bias_offset(
            params, in_q, weights_node, weights_q, o_q)

        if not (opts['allow_asymmetric_out'] or force_out_q or biases_q.offset is None):
            raise ValueError(
                f'bias offset is set but asymmetric is disallowed in {params.name}')

        # o_q.set_forced(flags=['dtype'])
        # in_q.set_forced(flags=['dtype'])
        if isinstance(params, Conv2DNode) and params.padding.has_padding:
            in_q.set_forced(flags=['zero_point'])

        if opts['hwc']:
            cls.check_order(params, AT_SW_KER_HWC_IN_ORDER,
                            AT_SW_KER_HWC_OUT_ORDER)
        else:
            cls.check_order(params, AT_SW_KER_IN_ORDER, AT_SW_KER_OUT_ORDER)
        return QRec.scaled(in_qs=[in_q, weights_q, biases_q],
                           out_qs=[o_q],
                           acc_q=biases_q,
                           calc_q=biases_q,
                           mul_biases_q=mul_biases_q)

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return [QType.from_min_max_sq(
            *stats.get_range_in(idx),
            dtype=np.int8,
            asymmetric=in_qs and in_qs[idx].asymmetric and cls.can_handle_asymmetric_input(params, **kwargs))
            if dim is not None else None
            for idx, dim in enumerate(params.in_dims)]

    @classmethod
    def can_handle_asymmetric_input(cls, params, **kwargs):
        if isinstance(params, LinearNode):
            return True
        return not params.padding.has_padding


@params_type(LinearNode, Conv2DNode)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': set([np.int8, np.int32])})
@option_constraint(check_filter_options(False, input_size={8, None}, output_size={8, None}))
class FilterSWMult8x8(FilterSWMultBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int8, **kwargs)


@params_type(LinearNode, Conv2DNode)
@in_qs_constraint({'dtype': np.int16})
@out_qs_constraint({'dtype': set([np.int8, np.int32])})
@option_constraint(check_filter_options(False, input_size={16, None}, output_size={16, None}))
class FilterSWMult16x8(FilterSWMultBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int16, **kwargs)


class FilterMultNE16Base(FilterMultBase):
    @classmethod
    def _quantize_ne16(cls, params, in_qs, stats, input_dtype, **kwargs):
        # copy in_qs because we may modify it
        in_qs = deepcopy(in_qs)
        input_bits = 16 if input_dtype in (np.uint16, np.int16) else 8
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        LOG.debug('selecting USQ8 NE16 kernel filter quantizer')
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        G = kwargs['G']
        weights_node = cls.get_weights_node(G, fusion if fusion else params)
        min_val, max_val = None, None
        wbits = (min(in_qs[1].bits, opts['weight_bits'])
                 if 'weight_bits' not in opts['set_on_node'] else opts['weight_bits'])
        weights_q = QType.from_array_sq(arr=weights_node.dqvalue,
                                        quantized_dimension=cls.get_quantized_dimension(
                                            params, opts),
                                        dtype=np.uint8,
                                        narrow_range=opts['narrow_weights'],
                                        bit_pack=wbits,
                                        no_compression=True,
                                        bits=wbits)

        in_q = in_qs[0]
        if input_bits > 8:
            in_q = limit_input_precision(
                params, input_bits, in_q, params.filter_dim.sz,
                opts['narrow_weights'], wbits,
                opts.get('max_precision_limit',
                         MAX_PRECISION_LIMIT_OPTION['default']),
                out_ranges=[stats.get_range_out(0)],
                w_qs=[weights_q])

        assert in_q.dtype == input_dtype

        if force_out_q:
            o_q = deepcopy(force_out_q)
            o_q.dont_copy_attr = ['ne16']
            LOG.debug(
                f'node {params.name} output forced to range {o_q.min}/{o_q.max}')
        else:
            # cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            force_output_size = opts.get('force_output_size', 8)
            output_dtype = np.uint8 if force_output_size == 8 else np.uint16
            min_val, max_val = stats.get_range_out(0, bits=force_output_size)
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=output_dtype,
                                        dont_copy_attr=['ne16'],
                                        asymmetric=opts["allow_asymmetric_out"])
        o_q.attr.ne16 = True
        biases_node = cls.get_biases_node(G, fusion if fusion else params)
        biases_scale = weights_q.scale * in_q.scale
        max_int32 = np.iinfo(np.int32).max
        bias_overflow = np.abs(biases_node.dqvalue / biases_scale) > max_int32
        if np.any(bias_overflow):
            weights_scales = weights_q.scale
            weights_scales[bias_overflow] = 1
            weights_q.scale = weights_scales
            biases_scale = weights_q.scale * in_q.scale
        biases_q = QType(
            dtype=np.int32, scale=weights_q.scale * in_q.scale,
            ne16_biases=(input_bits != 16), quantized_dimension=0)

        biases_bits = np.ceil(np.log2(np.abs(biases_q.quantize(biases_node.dqvalue))))
        max_accum_bits = np.maximum((np.ceil(np.log2(params.filter_dim.sz)) + 7 + 7), biases_bits)
        available_bits = 31 - max_accum_bits
        mul_biases_q = MultMulBiasScaleQType(
            dtype=np.uint8,
            available_bits=available_bits,
            scale=in_q.scale * weights_q.scale / o_q.scale
        )
        # mul_biases_q = MultMulBiasScaleQType.from_filter(
        #     in_q, weights_q, o_q, params)

        # calculate bias offset - this will be added to the bias in the kernel
        # it is already in quantized form
        biases_q.offset = FilterMultNE16Base.calculate_bias_offset(
            params, in_q, weights_node, weights_q, o_q)
        # returning the new weights and biases qs will force backprop

        cls.check_order(params, AT_NE16_KER_IN_ORDER, AT_NE16_KER_OUT_ORDER)

        if input_bits == 16:
            prenorm = min(np.min(np.min(mul_biases_q.qnorms)), 8)
        else:
            prenorm = 0
        mul_biases_q.pre_normalization = prenorm

        return QRec.scaled(in_qs=[in_q, weights_q, biases_q],
                           out_qs=[o_q],
                           acc_q=biases_q,
                           calc_q=biases_q,
                           mul_biases_q=mul_biases_q,
                           ne16=True)

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        opts = kwargs['opts']
        in_dtype = np.uint8 if opts.get(
            'force_input_size', 8) == 8 else np.uint16
        return [QType.from_min_max_sq(*stats.get_range_in(idx, dont_throw=idx==2),
                                      dtype=in_dtype,
                                      bits=None if idx != 1 or in_qs[1].is_floating else in_qs[1].bits,
                                      asymmetric=(idx==0 and opts["allow_asymmetric_out"]))
                if dim is not None and stats.get_range_in(idx, dont_throw=idx==2) else None
                for idx, dim in enumerate(params.in_dims)]


@params_type(LinearNode, Conv2DNode)
@in_qs_constraint({'dtype': np.uint8})
@out_qs_constraint({'dtype': set([np.uint8, np.int8, np.uint16, np.int16, np.int32])})
@option_constraint(check_filter_options(True, input_size=set([8, None]), output_size=set([8, 16, None])))
class FilterMultNE16Feat8x8(FilterMultNE16Base):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_ne16(params, in_qs, stats, np.uint8, **kwargs)


@params_type(LinearNode, Conv2DNode)
@in_qs_constraint({'dtype': np.uint16})
@out_qs_constraint({'dtype': set([np.uint8, np.int8, np.uint16, np.int16, np.int32])})
@option_constraint(check_filter_options(True, input_size=set([16]), output_size=set([8, 16, None])))
class FilterMultNE16Feat16x8(FilterMultNE16Base):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_ne16(params, in_qs, stats, np.uint16, **kwargs)
