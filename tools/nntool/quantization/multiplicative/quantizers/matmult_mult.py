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
from graph.types import (ConstantInputParameters, HSigmoidActivationParameters,
                         MatMulOpParameters, ReluActivationParameters,
                         SigmoidActivationParameters, TransposeParameters)
from graph.types.activations import (HSwishActivationParameters,
                                     TanHActivationParameters)
from graph.types.base import NNEdge
from graph.types.tensor_arithmetic import MatMulTransposedParameters
from quantization.multiplicative.quantizers.filter_mult import \
    check_filter_options
from quantization.multiplicative.quantizers.rnn_mult_ne16 import (
    limit_input_precision, roundup)
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantizer_options import (FORCE_INPUT_SIZE_OPTION,
                                            FORCE_OUTPUT_SIZE_OPTION,
                                            MAX_PRECISION_LIMIT_OPTION,
                                            NE16_WEIGHT_BITS_OPTION,
                                            USE_NE16_OPTION)
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       option_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from utils.graph import GraphView
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@options(
    NE16_WEIGHT_BITS_OPTION,
    USE_NE16_OPTION,
    FORCE_INPUT_SIZE_OPTION,
    FORCE_OUTPUT_SIZE_OPTION,
    MAX_PRECISION_LIMIT_OPTION
)
class MatMultMultBase(MultQuantizionHandler):
    @classmethod
    def move_constant(cls, G: GraphView, params, in_qs):
        # looks for a constant on one of the inputs
        # if there is one we can scale by the second dimension of the second
        # tensor. If the constant is on the first tensor then move to the second
        # and transpose the operation
        in_edges = G.indexed_in_edges(params.name)
        in1_node = in_edges[0].from_node
        in2_node = in_edges[1].from_node

        if isinstance(in2_node, ConstantInputParameters):
            return in2_node, in_qs
        elif isinstance(in1_node, ConstantInputParameters):
            if len(params.in_dims) > 2:
                # check if the bias has the correct length to move constant
                # it must have a length equal to the second tensors second dimension after transpose
                bias_size = params.in_dims[2].size()
                in1_shape = params.in_dims[0].shape
                if in1_shape[1] != bias_size:
                    return None, in_qs
            for edge in in_edges[:2:]:
                G.remove_edge(edge)
            to_idx = 1
            # swap edges to move constant onto input 2
            for edge in in_edges[:2:]:
                new_edge = NNEdge(from_node=edge.from_node, to_node=edge.to_node,
                                  from_idx=edge.from_idx, to_idx=to_idx)
                G.add_edge(new_edge)
                to_idx = 1 - to_idx
            # use A.B = (BT.AT)T identity
            tin1 = TransposeParameters(G.unique_name(
                f'{params.name}_tin1'), transpose=(1, 0))
            tin2 = TransposeParameters(G.unique_name(
                f'{params.name}_tin2'), transpose=(1, 0))
            tout = TransposeParameters(G.unique_name(
                f'{params.name}_tout'), transpose=(1, 0))
            G.insert_node_before(tin1, params)
            G.insert_node_before(tin2, params, to_idx=1)
            G.insert_node_after(params, tout)
            LOG.warning(
                'transposes inserted on %s - rerun adjust', params.name)
            return in1_node, [in_qs[1], in_qs[0]] + in_qs[2::]
        else:
            return None, in_qs

    @classmethod
    def get_min_max(cls, fusion, stats, all_stats, params):
        min_val, max_val = None, None
        if fusion:
            fnodes = fusion.contained_nodes()
            if len(fnodes) > 1:
                act_node = fnodes[1]
                if isinstance(act_node, (HSigmoidActivationParameters, HSwishActivationParameters)):
                    # Hard sigmoid/swish have parameters that need to be representable
                    cls.check_valid_ranges(params, stats, idx=0, dirs='out')
                    min_val, max_val = stats['range_out'][0]['min'], stats['range_out'][0]['max']
                    max_val = np.maximum(max_val, max(
                        params.upper_bound, params.offset))
                elif isinstance(act_node, (SigmoidActivationParameters, TanHActivationParameters)):
                    # Guarantee Q12 input to lut based sigmoid and tan
                    min_val, max_val = -8, 8
                elif isinstance(act_node, ReluActivationParameters):
                    # Take stats from activation after the convolution
                    stats = all_stats.get(NodeId(fusion, act_node))

        if min_val is None or max_val is None:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            min_val, max_val = stats['range_out'][0]['min'], stats['range_out'][0]['max']
        return min_val, max_val


@params_type(MatMulOpParameters)
@in_qs_constraint({'dtype': set([np.int8])})
@out_qs_constraint({'dtype': set([np.int8, np.int32])})
@option_constraint(check_filter_options(False, input_size={8, None}, output_size={8, None}))
class MatMultMultSW8(MatMultMultBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)

        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        G = kwargs['G']
        # only attempt channel scaling if the second input is constant
        # if len(in_qs) > 2:
        in2_node, in_qs = cls.move_constant(
            G, fusion if fusion else params, in_qs)
        if in2_node:
            kwargs['graph_update']['requires_adjust'] = True
            in_q2 = QType.from_array_sq(
                arr=in2_node.dqvalue,
                quantized_dimension=(len(in2_node.dqvalue.shape) -
                                     (2 if isinstance(params, MatMulTransposedParameters) else 1)),
                dtype=np.int8,
                narrow_range=True,
                bits=8)
        else:
            in_q2 = in_qs[1].make_symmetric_signed()

        in_q1 = in_qs[0].make_symmetric_signed()

        # min_val, max_val = cls.get_min_max(
        #     fusion, stats, kwargs['all_stats'], params)

        if force_out_q:
            o_q = force_out_q
            # can't be forced to something not np.int8
            if (o_q.dtype != np.int8 and o_q.dtype != np.int32) or o_q.asymmetric:
                return None
            LOG.warning(f'node {params.name} output forced to range {o_q.min}/{o_q.max} '
                        f'{"asymmetric" if o_q.asymmetric else "symmetric"}')
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            min_val, max_val = stats['range_out'][0]['min'], stats['range_out'][0]['max']
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=out_dtype)
        if len(in_qs) == 3:
            bias_scale = in_q1.scale * in_q2.scale
            quantized_dimension = None if len(bias_scale) == 1 else 0
            biases_q = QType(
                dtype=np.int32,
                scale=in_q1.scale * in_q2.scale,
                quantized_dimension=quantized_dimension)
            out_in_qs = [in_q1, in_q2, biases_q]
        else:
            out_in_qs = [in_q1, in_q2]

        mul_biases_q = MultMulBiasScaleQType(
            scale=(in_q1.scale*in_q2.scale)/o_q.scale)

        return QRec.scaled(in_qs=out_in_qs,
                           out_qs=[o_q],
                           mul_biases_q=mul_biases_q)


class MatMultMultNE16Base(MatMultMultBase):
    @classmethod
    def _quantize_ne16(cls, params, in_qs, stats, input_dtype, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        input_bits = 16 if input_dtype in (np.uint16, np.int16) else 8

        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        G = kwargs['G']
        # only attempt channel scaling if we have a bias
        in2_node, in_qs = cls.move_constant(
            G, fusion if fusion else params, in_qs)
        if not in2_node:
            raise ValueError(
                f"Not supported in NE16 this matmul {params.name}")

        w1, h1 = params.in_dims[0].shape[0], params.in_dims[0].shape[1]
        h2, w2 = params.in_dims[1].shape[0], params.in_dims[1].shape[1]
        h2_padded = roundup(h2, input_bits == 16)
        kwargs['graph_update']['requires_adjust'] = True
        in_q2 = QType.from_array_sq(
            arr=in2_node.dqvalue,
            quantized_dimension=0,
            dtype=np.uint8,
            narrow_range=True,
            bit_pack=opts['weight_bits'],
            no_compression=True,
            bits=opts['weight_bits'],
            resize=((h2, w2), (h2_padded, w2))
        )

        in_q1 = QType.from_min_max_sq(in_qs[0].min_val, in_qs[0].max_val,
                                      dtype=input_dtype,
                                      asymmetric=True)
        in_q1 = limit_input_precision(
            params, input_bits, in_q1, w1, False, opts['weight_bits'],
            opts.get('max_precision_limit',
                     MAX_PRECISION_LIMIT_OPTION['default']),
            out_ranges=stats.get('range_out'),
            w_qs=[in_q2])

        # min_val, max_val = cls.get_min_max(
        #     fusion, stats, kwargs['all_stats'], params)

        if force_out_q:
            o_q = force_out_q
            LOG.warning(f'node {params.name} output forced to range {o_q.min}/{o_q.max} '
                        f'{"asymmetric" if o_q.asymmetric else "symmetric"}')
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            min_val, max_val = stats['range_out'][0]['min'], stats['range_out'][0]['max']
            force_output_size = opts.get('force_output_size', 8)
            out_dtype = np.uint8 if force_output_size == 8 else np.uint16
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dont_copy_attr=['ne16'],
                                        asymmetric=True,
                                        dtype=out_dtype)
        if len(in_qs) == 3:
            biases_q = QType(
                dtype=np.int32, scale=in_q1.scale * in_q2.scale, ne16_biases=(input_bits != 16),
                quantized_dimension=0)
            # calculate bias offset - this will be added to the bias in the kernel
            # it is already in quantized form
            bias_offset = np.zeros(
                (in2_node.dqvalue.shape[0], ), dtype=np.int32)
            if in_q1.zero_point != 0:
                # input zero correction is sum(W * Zin) by out_c if weights are channel scaled
                bias_offset -= np.sum(np.multiply(in_q1.zero_point,
                                                  in2_node.value_as(in_q2).astype(
                                                      np.int32) - in_q2.zero_point,
                                                  dtype=np.int32),
                                      dtype=np.int32,
                                      axis=1)
            if o_q.zero_point != 0:
                # output zero correction is So/(Si * Sw) * ZPo by out_c if weights are channel scaled
                scale = o_q.scale / (in_q1.scale * in_q2.scale)
                bias_offset += np.floor((o_q.zero_point *
                                         scale) + 0.5).astype(np.int32)
            if not np.all(bias_offset == 0):
                biases_q.offset = bias_offset
            out_in_qs = [in_q1, in_q2, biases_q]
        else:
            out_in_qs = [in_q1, in_q2]

        mul_biases_q = MultMulBiasScaleQType()
        mul_biases_q.scale = in_q1.scale * in_q2.scale / o_q.scale
        o_q.attr.ne16 = True

        if input_bits == 16:
            prenorm = min(np.min(np.min(mul_biases_q.qnorms)), 8)
        else:
            prenorm = 0
        mul_biases_q.pre_normalization = prenorm

        return QRec.scaled(in_qs=out_in_qs,
                           out_qs=[o_q],
                           mul_biases_q=mul_biases_q,
                           ne16=True)


@params_type(MatMulOpParameters)
@in_qs_constraint({'dtype': set([np.uint8])})
@out_qs_constraint({'dtype': set([np.uint8, np.int8, np.uint16, np.int16, np.int32])})
@option_constraint(check_filter_options(True, input_size={8, None}, output_size={8, 16, None}))
class MatMultMultNE16_8v8(MatMultMultNE16Base):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_ne16(params, in_qs, stats, np.uint8, **kwargs)


@params_type(MatMulOpParameters)
@in_qs_constraint({'dtype': set([np.uint16])})
@out_qs_constraint({'dtype': set([np.uint8, np.int8, np.uint16, np.int16, np.int32])})
@option_constraint(check_filter_options(True, input_size={16, None}, output_size={8, 16, None}))
class MatMultMultNE16_16v8(MatMultMultNE16Base):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_ne16(params, in_qs, stats, np.uint16, **kwargs)
