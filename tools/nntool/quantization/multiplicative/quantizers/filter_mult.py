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

import numpy as np
from graph.types import Conv2DParameters, FcParameters
from graph.types.activations import (HSigmoidActivationParameters,
                                     HSwishActivationParameters,
                                     SigmoidActivationParameters,
                                     TanHActivationParameters)
from quantization.multiplicative.mult_quantization import \
    MultScalableFilterQuantizationRecord
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.qtype import QType
from quantization.unified_quantization_handler import options, params_type
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler


@options(
    {
        'name': 'quantized_dimension',
        'type': str,
        'choices': ['tensor', 'channel'],
        'help': 'scales filter weights by channel or tensor',
        'default': 'channel'
    },
    {
        'name': 'narrow_weights',
        'type': bool,
        'help': 'scales weights with a representation of both 1 and -1 (i.e. -127 - 127 in 8 bits)',
        'default': True
    })
@params_type(FcParameters, Conv2DParameters)
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
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        G = kwargs['G']
        weights_node = cls.get_weights_node(G, fusion if fusion else params)
        min_val, max_val = None, None
        weights_q = QType.from_array_sq(arr=weights_node.dqvalue,
                                        quantized_dimension=cls.get_quantized_dimension(
                                            params, opts),
                                        dtype=np.int8, narrow_range=opts['narrow_weights'])
        if fusion and fusion.fusion_type in ['conv_active_pool', 'conv_active']:
            stats = kwargs['all_stats'][NodeId(fusion, fusion.contained_nodes()[0])]

            if isinstance(fusion.contained_nodes()[1],
                          (SigmoidActivationParameters, TanHActivationParameters, HSwishActivationParameters)):
                stats = kwargs['all_stats'][NodeId(fusion, fusion.contained_nodes()[0])]
            elif fusion and isinstance(fusion.contained_nodes()[1], HSigmoidActivationParameters):
                # Hard sigmoid implements a RELU, be sure 6 can be representable
                min_val, max_val = 0, 6
            else:
                # Take stats from activation after the convolution
                stats = kwargs['all_stats'][NodeId(fusion, fusion.contained_nodes()[1])]

        if min_val is None or max_val is None:
            min_val, max_val = stats['range_out'][0]['min'], stats['range_out'][0]['max']

        if force_out_q:
            o_q = force_out_q
        else:
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=out_dtype)
        biases_q = QType(dtype=np.int32, scale=weights_q.scale * in_qs[0].scale)
        mul_biases_q = MultMulBiasScaleQType.from_filter(in_qs[0], weights_q, o_q, params)
        # returning the new weights and biases qs will force backprop
        # TODO - ACC_Q LOOKS WRONG AFTER THIS
        return MultScalableFilterQuantizationRecord(in_qs=[in_qs[0], weights_q, biases_q],
                                                    out_qs=[o_q],
                                                    acc_q=biases_q,
                                                    calc_q=biases_q,
                                                    mul_biases_q=mul_biases_q)
