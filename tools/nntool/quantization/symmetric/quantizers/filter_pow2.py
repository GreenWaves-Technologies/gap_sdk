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
from graph.types import (Conv2DParameters, FcParameters,
                         MultiplicativeBiasParameters)
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type, options)
from utils.node_id import NodeId
from utils.stats_funcs import calc_bits

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

@options(
    {
        'name': 'pow2_biases',
        'type': int,
        'choices': [0, 8, 16, 32],
        'help': 'bits for filter biases - if set to 0 the same size as the output type will be used',
        'default': 0
    })
@params_type(FcParameters, Conv2DParameters)
# @can_dequantize(True)
@in_qs_constraint({'dtype': set([np.int8, np.int16])})
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
class FilterPow2(Pow2QuantizionHandler):
    @classmethod
    def get_weights_and_biases_nodes(cls, G, params):
        edges = G.indexed_in_edges(params.name)
        if len(edges) != 3:
            raise ValueError(f"didn't find 3 input edges on {params.name}")
        return edges[1].from_node, edges[2].from_node

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, params_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        fusion = kwargs.get('fusion', None)
        pow2_biases = kwargs.get('opts')['pow2_biases']
        G = kwargs['G']
        weights_node, biases_node = cls.get_weights_and_biases_nodes(
            G, fusion if fusion else params)

        range_acc = stats['range_acc']
        conv_active = fusion and fusion.fusion_type in [
            'conv_active_pool', 'conv_active']
        int_dtype = np.int32
        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        if conv_active:
            # Take stats from activation after the convolution
            range_out = kwargs['all_stats'][NodeId(
                fusion, fusion.contained_nodes()[1])]['range_out'][0]
            out_dtype = np.int32
        else:
            out_dtype = params_dtype
            range_out = stats['range_out'][0]

        in_q = deepcopy(in_qs[0]).scale_to_pow2()
        calc_width = 32

        if force_out_q:
            o_q = force_out_q
        else:
            o_q = QType.from_min_max_pow2(range_out['min'],
                                          range_out['max'],
                                          dtype=out_dtype)
        weights_q = QType.from_array_pow2(arr=weights_node.dqvalue,
                                          dtype=params_dtype)
        calc_q = in_q.q + weights_q.q

        acc_bits = calc_bits(range_acc['max'], range_acc['min'])
        act_bits = calc_bits(range_out['min'], range_out['max'])
        act_acc_bits = max(acc_bits, act_bits)

        calc_int_bits = calc_width - calc_q
        if calc_int_bits < act_acc_bits:
            # we don't have enough space for the integer portion so reduce the precision of
            # the weights
            missing_bits = act_acc_bits - calc_int_bits
            # TODO - This needs improving
            assert weights_q.q >= missing_bits, "no space in weights to reduce precision"
            LOG.warning(
                'reducing weight precision in %s to satisfy quantization constraints', params.name)
            weights_q.q = weights_q.q - missing_bits
            calc_q = in_q.q + weights_q.q
            calc_int_bits = calc_width - calc_q

        c_q = acc_q = QType(bits=calc_width, q=calc_q, signed=True)

        if conv_active:
            o_q = c_q

        if pow2_biases == 0:
            biases_dtype = params_dtype
        elif pow2_biases == 8:
            biases_dtype = np.int8
        elif pow2_biases == 16:
            biases_dtype = np.int16
        else:
            biases_dtype = np.int32

        biases_q = QType.from_array_pow2(arr=biases_node.dqvalue,
                                         dtype=biases_dtype)
        # make sure that the biases are not stored more precisily than the accumulator. It's pointless and will
        # cause a negative shift
        if biases_q.q > acc_q.q:
            biases_q.q = acc_q.q

        if isinstance(params, MultiplicativeBiasParameters) and params.has_mul_bias:
            mb_q = QType.from_array_pow2(arr=params.mul_biases,
                                         dtype=int_dtype)
        else:
            mb_q = None
        return QRec.symmetric(in_qs=[in_q, weights_q, biases_q], out_qs=[o_q], calc_q=c_q,
                              acc_q=acc_q,
                              mul_biases_q=mb_q)
