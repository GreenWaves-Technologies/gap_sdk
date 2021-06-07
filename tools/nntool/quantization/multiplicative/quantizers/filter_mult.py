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

import numpy as np
from graph.types import (Conv2DParameters, FcParameters,
                         HSigmoidActivationParameters,
                         HSwishActivationParameters, PoolingParameters,
                         ReluActivationParameters, SigmoidActivationParameters,
                         HTanHActivationParameters)
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

AT_SW_KER_IN_ORDER = [['c', 'h', 'w'], ['out_c', 'in_c', 'h', 'w'], ['out_c']]
AT_SW_KER_OUT_ORDER = [['c', 'h', 'w']]
AT_NE16_KER_IN_ORDER = [['h', 'w', 'c'], [
    'out_c', 'h', 'w', 'in_c'], ['out_c']]
AT_NE16_KER_OUT_ORDER = [['h', 'w', 'c']]


@options(
    {
        'name': 'quantized_dimension',
        'shortcut': 'd',
        'type': str,
        'choices': ['tensor', 'channel'],
        'help': 'scales filter weights by channel or tensor',
        'default': 'channel'
    },
    {
        'name': 'narrow_weights',
        'shortcut': 'n',
        'type': bool,
        'help': 'scales filter weights with a representation of both 1 and -1 (i.e. -127 - 127 in 8 bits)',
        'default': True
    },
    {
        'name': 'weight_bits',
        'type': int,
        'help': 'how many bits to use in weights',
        'choices': list(range(2, 9)),
        'default': 8
    },
    {
        'name': 'use_ne16',
        'type': bool,
        'help': 'enable use of NE16 kernels (if supported) on this layer',
        'default': False
    },
    {
        'name': 'force_ne16',
        'type': bool,
        'help': 'force use of NE16 kernels on this layer - may not be supported for model generation',
        'default': False
    },
    {
        'name': 'allow_asymmetric',
        'type': bool,
        'help': 'EXPERIMENTAL - allow soft kernels to use asymmetric quantization where possible',
        'default': False
    }
)
@params_type(FcParameters, Conv2DParameters)
@in_qs_constraint({'dtype': set([np.int8, np.uint8])})
@out_qs_constraint({'dtype': set([np.int8, np.uint8])})
class FilterMult(MultQuantizionHandler):
    @classmethod
    def get_quantized_dimension(cls, params, opts):
        if opts['quantized_dimension'] == 'tensor':
            return None
        elif opts['quantized_dimension'] == 'channel':
            return params.filter.get_order_idx('out_c')
        return None

    @classmethod
    def get_weights_node(cls, G, params):
        edges = G.indexed_in_edges(params.name)
        if len(edges) != 3:
            raise ValueError(f"didn't find 3 input edges on {params.name}")
        return edges[1].from_node

    @classmethod
    def can_ne16(cls, params, opts, fusion):
        if opts.get('force_ne16'):
            return True
        if not opts.get('use_ne16'):
            return False
        if not isinstance(params, (Conv2DParameters, FcParameters)):
            return False
        if fusion:
            if fusion.fusion_type in ['conv_active_pool', 'conv_active']:
                if any(not isinstance(node, (Conv2DParameters, ReluActivationParameters, PoolingParameters))
                       for node in fusion.contained_nodes()):
                    return False
            else:
                return False
        if isinstance(params, Conv2DParameters):
            if (params.filter.w != params.filter.h or (params.filter.w != 1 and params.filter.w != 3)):
                return False
            if (params.stride.size() != 1 and params.stride.shape != [2, 2]):
                return False
        return True

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        if cls.can_ne16(params, opts, fusion):
            return [np.uint8, np.uint8, np.int32]
        return [np.int8, np.int8, np.int32]

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        if cls.can_ne16(params, opts, fusion):
            LOG.info('selecting USQ8 NE16 kernel filter quantizer')
            return cls.quantize_ne16(params, in_qs, stats, **kwargs)
        LOG.info('selecting SQ8 software kernel filter quantizer')
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        G = kwargs['G']
        in_q = in_qs[0]

        # check input quantization and int8 type
        # if not padded we can scale asymmetric
        if in_q.dtype == np.uint8:
            # handle NE16
            cls.check_valid_ranges(params, stats, idx=0, dirs='in')
            # allow asymmetric if not padded
            if isinstance(params, Conv2DParameters) and params.padding.has_padding:
                in_q = QType.from_min_max_sq(stats['range_in'][0]['min'], stats['range_in'][0]['max'],
                                             dtype=np.int8,
                                             forced=True)
            else:
                in_q = QType.from_min_max_sq(stats['range_in'][0]['min'], stats['range_in'][0]['max'],
                                             dtype=np.int8,
                                             zero_point=in_q.zero_point - 128)
        elif (isinstance(params, Conv2DParameters) and not in_q.is_symmetric and params.padding.has_padding):
            cls.check_valid_ranges(params, stats, idx=0, dirs='in')
            in_q = QType.from_min_max_sq(stats['range_in'][0]['min'], stats['range_in'][0]['max'],
                                         dtype=np.int8)
        # if not forced we can try asymmetric
        elif (opts['allow_asymmetric'] and isinstance(params, Conv2DParameters) and not in_q.forced and
              in_q.is_symmetric and not params.padding.has_padding):
            cls.check_valid_ranges(params, stats, idx=0, dirs='in')
            in_q = QType.from_min_max_sq(stats['range_in'][0]['min'], stats['range_in'][0]['max'],
                                         dtype=np.int8,
                                         asymmetric=True)

        if opts['weight_bits'] != 8:
            LOG.warning(
                'sub byte weights quantization requested but NE16 kernel not selected')
        weights_node = cls.get_weights_node(G, fusion if fusion else params)
        weights_q = QType.from_array_sq(arr=weights_node.dqvalue,
                                        quantized_dimension=cls.get_quantized_dimension(
                                            params, opts),
                                        dtype=np.int8, narrow_range=opts['narrow_weights'],
                                        bits=opts['weight_bits'])

        min_val, max_val = cls.get_min_max(
            fusion, stats, kwargs['all_stats'], params)

        if force_out_q:
            o_q = force_out_q
            # can't be forced to something not np.int8
            if o_q.dtype != np.int8:
                return None
            LOG.warning('node %s output forced to range %s/%s - actual range %s/%s %s',
                        params.name, o_q.min, o_q.max, min_val, max_val,
                        "asymmetric" if o_q.is_asymmetric else "symmetric")
        else:
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=out_dtype,
                                        asymmetric=opts['allow_asymmetric'])
        biases_q = QType(
            dtype=np.int32, scale=weights_q.scale * in_q.scale)
        mul_biases_q = MultMulBiasScaleQType.from_filter(
            in_q, weights_q, o_q, params)

        # returning the new weights and biases qs will force backprop

        # calculate bias offset - this will be added to the bias in the kernel
        # it is already in quantized form
        biases_q.offset = FilterMult.calculate_bias_offset(
            params, in_q, weights_node, weights_q, o_q)

        if not (opts['allow_asymmetric'] or force_out_q or biases_q.offset is None):
            raise ValueError(f'bias offset is set but asymmetric is disallowed in {params.name}')

        # o_q.set_forced(flags=['dtype'])
        # in_q.set_forced(flags=['dtype'])
        if isinstance(params, Conv2DParameters) and params.padding.has_padding:
            in_q.set_forced(flags=['zero_point'])

        cls.check_order(params, AT_SW_KER_IN_ORDER, AT_SW_KER_OUT_ORDER)
        return QRec.scaled(in_qs=[in_q, weights_q, biases_q],
                           out_qs=[o_q],
                           acc_q=biases_q,
                           calc_q=biases_q,
                           mul_biases_q=mul_biases_q)

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=np.uint8 if cls.can_ne16(params, opts, fusion) else np.int8,
                                      asymmetric=in_qs[idx].is_asymmetric and cls.can_handle_asymmetric_input(params, **kwargs))
                if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]

    @classmethod
    def can_handle_asymmetric_input(cls, params, **kwargs):
        if isinstance(params, FcParameters):
            return True
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        if cls.can_ne16(params, opts, fusion):
            return True
        return not params.padding.has_padding        

    @classmethod
    def get_min_max(cls, fusion, stats, all_stats, params):
        min_val, max_val = None, None
        if fusion:
            # activation directly after filter
            if fusion.fusion_type in ['conv_active_pool', 'conv_active']:
                act_node = fusion.contained_nodes()[1]
            # activation after pool layer - if max will be swapped to conv_active_pool
            elif fusion.fusion_type == 'conv_pool_active' and fusion.contained_nodes()[1].pool_type == 'max':
                act_node = fusion.contained_nodes()[2]
            else:
                act_node = None
            if act_node:
                if isinstance(act_node, HSigmoidActivationParameters):
                    # Hard sigmoid implements a RELU, be sure 6 can be represented
                    min_val, max_val = 0, 6
                elif isinstance(act_node, SigmoidActivationParameters):
                    # Guarantee Q12 input to sigmoid
                    min_val, max_val = -8, 8
                elif isinstance(act_node, ReluActivationParameters):
                    # Take stats from activation after the convolution
                    stats = all_stats.get(NodeId(fusion, act_node))

        if min_val is None or max_val is None:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            min_val, max_val = stats['range_out'][0]['min'], stats['range_out'][0]['max']
        return min_val, max_val

    @classmethod
    def quantize_ne16(cls, params, in_qs, stats, **kwargs):
        opts = kwargs['opts']
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        fusion = kwargs.get('fusion', None)
        G = kwargs['G']
        weights_node = cls.get_weights_node(G, fusion if fusion else params)
        min_val, max_val = None, None
        # note that weights are signed since the zero point of weights is
        # calculated by NE16. The zero point needs to be removed during
        # code gen
        weights_q = QType.from_array_sq(arr=weights_node.dqvalue,
                                        quantized_dimension=cls.get_quantized_dimension(
                                            params, opts),
                                        dtype=np.uint8,
                                        ne16_order=True,
                                        narrow_range=True,
                                        bits=opts['weight_bits'])

        in_q = in_qs[0]
        # check input quantization and scale asymmetric uint8
        if in_q.dtype != np.uint8:
            # I ignore a force here which is not very clean
            # if in_q.forced_dtype:
            #     return None
            cls.check_valid_ranges(params, stats, idx=0, dirs='in')
            in_q = QType.from_min_max_sq(stats['range_in'][0]['min'], stats['range_in'][0]['max'],
                                         dtype=np.uint8,
                                         asymmetric=True)

        min_val, max_val = cls.get_min_max(
            fusion, stats, kwargs['all_stats'], params)

        if force_out_q:
            o_q = force_out_q
            # can't be forced to something not np.uint8
            if o_q.dtype != np.uint8:
                return None
            LOG.warning('node %s output forced to range %s/%s - actual range %s/%s',
                        params.name, o_q.min, o_q.max, min_val, max_val)
        else:
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=np.uint8,
                                        asymmetric=True)
        biases_q = QType(
            dtype=np.int32, scale=weights_q.scale * in_q.scale, ne16_biases=True)

        mul_biases_q = MultMulBiasScaleQType.from_filter(
            in_q, weights_q, o_q, params)

        # calculate bias offset - this will be added to the bias in the kernel
        # it is already in quantized form
        biases_q.offset = FilterMult.calculate_bias_offset(
            params, in_q, weights_node, weights_q, o_q)
        cls.check_order(params, AT_NE16_KER_IN_ORDER, AT_NE16_KER_OUT_ORDER)
        # returning the new weights and biases qs will force backprop

        cls.check_order(params, AT_NE16_KER_IN_ORDER, AT_NE16_KER_OUT_ORDER)

        # o_q.set_forced(flags=['dtype'])
        # in_q.set_forced(flags=['dtype'])
        return QRec.scaled(in_qs=[in_q, weights_q, biases_q],
                           out_qs=[o_q],
                           acc_q=biases_q,
                           calc_q=biases_q,
                           mul_biases_q=mul_biases_q,
                           ne16=True)

    @staticmethod
    def calculate_bias_offset(params, in_q, weights_node, weights_q, o_q):
        # calculate bias offset - this will be added to the bias in the kernel
        # it is already in quantized form
        axis = tuple([idx for idx in range(len(params.filter.actual_shape)) if idx !=
                      params.filter.get_order_idx('out_c')])
        bias_offset = np.zeros((params.filter.out_c, ), dtype=np.int32)

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
