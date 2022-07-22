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
from numpy.core.shape_base import _concatenate_shapes
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

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@options(
    NE16_WEIGHT_BITS_OPTION,
    FORCE_EXTERNAL_SIZE_OPTION,
    NARROW_WEIGHTS_OPTION,
    USE_NE16_OPTION,
    FORCE_RNN_1_MINUS_1_OUT
)
@params_type(LSTMNode)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
@option_constraint(force_external_size={8, None}, use_ne16={None, False})
class LSTMMultMult8x8(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params: LSTMNode, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        in_qs = cls.force_symmetric_and_dtype(in_qs, idx=0)
        if in_qs is None:
            return None
        in_qs = deepcopy(in_qs)
        G = kwargs['G']
        opts = kwargs.get('opts', {})

        if opts.get('force_rnn_1_minus_1_out'):
            o_q = QType.from_min_max_sq(
                -1.0,
                1.0,
                narrow_range=True,
                dtype=out_dtype)
        else:
            o_q = QType.from_min_max_sq(*stats.get_range_out(0, dtype=out_dtype),
                                        dtype=out_dtype)
            if force_out_qs and force_out_qs[0]:
                LOG.warning('on node %s output is being forced from scale %s -> %s',
                            params.name,
                            o_q.scale, force_out_qs[0].scale)
                o_q = force_out_qs[0]

        names = {val: idx for idx, val in enumerate(
            LSTMNode.INPUT_NAMES)}
        cell_range = stats.get_stats_key('range_cell')
        if cell_range is None:
            LOG.warning(
                f'cell range not present in stats for {params.name} - using cell clip {params.cell_clip}')
            cell_stat = params.cell_clip
        else:
            cell_stat = max(abs(cell_range[var])
                            for var in ['min', 'max'])
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

        cell_int_bits = calc_bits(cell_max)

        in_qs[names['c_state']].recalculate_scale(-cell_max,
                                                  cell_max)
        LOG.debug("cell bits %d max %d cell range %d",
                  cell_int_bits,
                  cell_max,
                  in_qs[names['c_state']].range)
        int2_scale = int3_scale = out_tanh_sig_scale = None
        if params.hard_act:
            # worst case is (internal_q * 3) + 2 = 32 (1 for 1 and 1 for sign) i.e. 10
            # but also (internal_q * 2) + cell_bits = 32
            int_q = min((16-cell_int_bits), 10)
            int2_scale = math.pow(2, -(int_q*2))
            int3_scale = math.pow(2, -(int_q*3))
        else:
            int_q = 12
            # output of LUT activations are always Q15
            out_tanh_sig_scale = math.pow(2, -15)
        int_scale = math.pow(2, -int_q)

        edges = G.indexed_in_edges(params.name)
        scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['i', 'o', 'c', 'f']}
        for scale_pair in scale_pairs.values():
            in_q = in_qs[names[scale_pair[0]]]
            in_qs[names[scale_pair[0]]] = QType.from_min_max_sq(
                in_q.min_val,
                in_q.max_val,
                dtype=np.int8,
                narrow_range=opts.get('narrow_weights'),
                bits = opts['weight_bits'],
                dont_generate_value=True)
            # in_qs[names[scale_pair[0]]].bits = opts['weight_bits']
            in_q = in_qs[names[scale_pair[1]]]
            in_qs[names[scale_pair[1]]] = QType.from_min_max_sq(
                in_q.min_val,
                in_q.max_val,
                dtype=np.int8,
                bits = opts['weight_bits'],
                narrow_range=opts.get('narrow_weights'),
                concatenated_nodes=[edges[names[scale_pair[0]]].from_node.name])
            # in_qs[names[scale_pair[1]]].bits = opts['weight_bits']

        w_scales = [(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                    for k, (namei, namer) in scale_pairs.items()]
        if (abs(1 - in_qs[0].scale / o_q.scale) < 0.1) and \
                all([(abs(1 - w_scale[0] / w_scale[1]) < 0.2) for w_scale in w_scales]):
            LOG.info("node %s has similar input and i_state scales --> "
                     "will be generated the same_scale kernel with better performances", params.name)
            params.rnn_same_inout_scale = True
            G.node_options[params.name] = params.at_options

        if params.rnn_same_inout_scale:
            if not (abs(1 - in_qs[0].scale / o_q.scale) < 0.1) and \
               not all([(abs(1 - w_scale[0] / w_scale[1]) < 0.1) for w_scale in w_scales]):
                LOG.warning(
                    "node %s has different input and i_state scales consider using the "
                    "LSTM kernel with rnn_same_inout_scale=False (better accuracy)", params.name)
            # in and out and state are all in the same scale
            in_and_out_scale = np.maximum(in_qs[0].scale, o_q.scale)
            # i_state scale may be 1 since the value is 0
            # np.maximum(in_and_out_scale, in_qs[names['i_state']].scale)
            i_state_scale = in_scale = in_and_out_scale
            in_qs[0].scale = in_scale
            o_q.scale = in_scale
            scales = {k: np.maximum(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                      for k, (namei, namer) in scale_pairs.items()}
            for k, (namei, namer) in scale_pairs.items():
                in_qs[names[namei]].scale = scales[k]
                in_qs[names[namer]].scale = scales[k]
        else:
            in_scale = in_qs[0].scale
            i_state_scale = o_q.scale
            o_q.scale = i_state_scale

        if not params.rnn_states_as_inputs:
            in_qs[names['i_state']].scale = i_state_scale

        # compute scales for perceptrons
        r_pscales = {k: in_qs[names["r_2_%s_w" % k]].scale *
                     i_state_scale for k in ['i', 'o', 'c', 'f']}
        scale_qtypes = {"r_2_%s_q" % k: MultMulBiasScaleQType(
            scale=r_pscale/int_scale) for k, r_pscale in r_pscales.items()}
        scale_qtypes['r_pscales'] = r_pscales
        r_pscales['int_scale'] = int_scale

        i_pscales = {k: in_qs[names["i_2_%s_w" % k]
                              ].scale * in_scale for k in ['i', 'o', 'c', 'f']}
        scale_qtypes['i_pscales'] = i_pscales
        # if input and i_state have different scales -> scale the inputs before sum
        # otherwise do nothing and these scales will be ignored
        scale_qtypes.update({"i_2_%s_q" % k: MultMulBiasScaleQType(
            scale=i_pscale/r_pscale) for (k, i_pscale), r_pscale in zip(i_pscales.items(), r_pscales.values())})

        if params.hard_act:
            # int_scale/int_scale because scaled after multiplied by forget
            cell_in_scale = (
                in_qs[names['c_state']].scale * int_scale)/int2_scale
            cell_out_scale = int2_scale/in_qs[names['c_state']].scale
            state_out_scale = int3_scale/i_state_scale
            r_pscales['act_out_scale'] = int_scale
            r_pscales['c_before_scale'] = int2_scale
        else:
            cell_in_scale = in_qs[names['c_state']
                                  ].scale*out_tanh_sig_scale/int_scale
            cell_out_scale = int_scale/in_qs[names['c_state']].scale
            state_out_scale = out_tanh_sig_scale/i_state_scale
            r_pscales['act_out_scale'] = out_tanh_sig_scale
            r_pscales['c_before_scale'] = int_scale

        scale_qtypes['cell_in_q'] = MultMulBiasScaleQType(scale=cell_in_scale)
        # TODO - Check cell clip here
        scale_qtypes['cell_out_q'] = MultMulBiasScaleQType(
            scale=cell_out_scale)
        scale_qtypes['state_out_q'] = MultMulBiasScaleQType(
            scale=state_out_scale)
        # set internal scale
        scale_qtypes['i_qtype'] = QType(q=int_q, bits=32, signed=True)
        # set biases to output of perceptron
        for gate in ['i', 'o', 'c', 'f']:
            in_qs[names[f"{gate}_b"]] = QType(scale=r_pscales[gate], dtype=np.int32)

        out_qs = [o_q]
        if params.output_h_state:
            out_qs.append(in_qs[names['i_state']])
        if params.output_c_state:
            out_qs.append(in_qs[names['c_state']])
        return QRec.scaled(
            in_qs=in_qs,
            out_qs=out_qs,
            **scale_qtypes,
        )
