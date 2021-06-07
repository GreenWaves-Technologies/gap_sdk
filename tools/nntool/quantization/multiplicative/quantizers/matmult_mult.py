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
from graph.types import (HSigmoidActivationParameters, MatMulOpParameters,
                         ReluActivationParameters, SigmoidActivationParameters)
from graph.types.base import NNEdge
from graph.types.input_output import ConstantInputParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(MatMulOpParameters)
@in_qs_constraint({'dtype': set([np.int8])})
@out_qs_constraint({'dtype': set([np.int8])})
class MatMultMult(MultQuantizionHandler):
    @classmethod
    def move_constant(cls, G, params, in_qs):
        # looks for a constant on one of the inputs
        # if there is one we can scale by the first dimension of the first
        # tensor. If the constant is on the second tensor then move to the first
        # and transpose the operation
        in_edges = G.indexed_in_edges(params.name)
        in1_node = in_edges[0].from_node
        in2_node = in_edges[1].from_node

        if isinstance(in1_node, ConstantInputParameters):
            return in1_node, in_qs
        elif isinstance(in2_node, ConstantInputParameters):
            if len(params.in_dims) > 2:
                # check if the bias has the correct length to move constant
                # it must have a length equal to the first tensors first dimension after transpose
                bias_size = params.in_dims[2].size()
                in2_shape = params.in_dims[1].shape
                if params.transpose_in and params.transpose_in[1]:
                    in2_shape = [in2_shape[idx] for idx in params.transpose_in[1]]
                if in2_shape[1] != bias_size:
                    return None, in_qs
            for edge in in_edges[:2:]:
                G.remove_edge(edge)
            to_idx = 1
            # swap edges to move constant onto input 0
            for edge in in_edges[:2:]:
                new_edge = NNEdge(from_node=edge.from_node, to_node=edge.to_node,
                                  from_idx=edge.from_idx, to_idx=to_idx)
                G.add_edge(new_edge)
                to_idx = 1 - to_idx
            # use A.B = (BT.AT)T identity
            params.transpose_in = [(1, 0), (1, 0)] + \
                ([None] if len(in_qs) == 3 else [])
            params.transpose_out = [(1, 0)]
            LOG.warning(
                'transposes inserted on %s - rerun adjust', params.name)
            return in2_node, [in_qs[1], in_qs[0]] + in_qs[2::]
        else:
            return None, in_qs

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)

        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        G = kwargs['G']
        # only attempt channel scaling if we have a bias
        if len(in_qs) > 2:
            in1_node, in_qs = cls.move_constant(
                G, fusion if fusion else params, in_qs)
            if in1_node:
                kwargs['graph_update']['requires_adjust'] = True
                in_q1 = QType.from_array_sq(
                    arr=in1_node.dqvalue,
                    quantized_dimension=0,
                    dtype=np.int8,
                    narrow_range=True,
                    bits=8)
            else:
                in_q1 = in_qs[0].make_symmetric_signed()
        else:
            in_q1 = in_qs[0].make_symmetric_signed()

        in_q2 = in_qs[1].make_symmetric_signed()


        min_val, max_val = cls.get_min_max(
            fusion, stats, kwargs['all_stats'], params)

        if force_out_q:
            o_q = force_out_q
            # can't be forced to something not np.int8
            if o_q.dtype != np.int8 or o_q.is_asymmetric:
                return None
            LOG.warning('node %s output forced to range %s/%s - actual range %s/%s %s',
                        params.name, o_q.min, o_q.max, min_val, max_val,
                        "asymmetric" if o_q.is_asymmetric else "symmetric")
        else:
            o_q = QType.from_min_max_sq(min_val=min_val,
                                        max_val=max_val,
                                        dtype=out_dtype)
        if len(in_qs) == 3:
            biases_q = QType(
                dtype=np.int32, scale=in_q1.scale * in_q2.scale)
            out_in_qs = [in_q1, in_q2, biases_q]
        else:
            out_in_qs = [in_q1, in_q2]


        mul_biases_q = MultMulBiasScaleQType()
        mul_biases_q.scale = in_q1.scale * in_q2.scale / o_q.scale

        return QRec.scaled(in_qs=out_in_qs,
                           out_qs=[o_q],
                           mul_biases_q=mul_biases_q)

    @classmethod
    def get_min_max(cls, fusion, stats, all_stats, params):
        min_val, max_val = None, None
        if fusion:
            fnodes = fusion.contained_nodes()
            if len(fnodes) > 1:
                act_node = fnodes[1]
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
