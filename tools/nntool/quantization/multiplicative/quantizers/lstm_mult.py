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
from graph.types import LSTMParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)
from utils.node_id import NodeId
from utils.stats_funcs import calc_bits

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)


@options(
    {
        'name': 'weight_bits',
        'type': int,
        'help': 'how many bits to use in weights',
        'choices': list(range(2, 9)),
        'default': 8
    }
)
@params_type(LSTMParameters)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
class LSTMMultMult(RescaleConstantMixin, MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        in_qs = cls.force_symmetric_and_dtype(in_qs, idx=0)
        if in_qs is None:
            return None
        in_qs = deepcopy(in_qs)
        G = kwargs['G']
        opts = kwargs['opts']

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        o_q = QType.from_min_max_sq(min_val=stats['range_out'][0]['min'],
                                    max_val=stats['range_out'][0]['max'],
                                    dtype=out_dtype)
        if force_out_qs and force_out_qs[0]:
            LOG.warning('on node %s output is being forced from scale %s -> %s',
                        params.name,
                        o_q.scale, force_out_qs[0].scale)
            o_q = force_out_qs[0]

        names = {val: idx for idx, val in enumerate(
            LSTMParameters.INPUT_NAMES)}
        cell_range = stats.get('range_cell')
        if cell_range is None:
            ValueError(f'cell range not present in stats for {params.name}')
        cell_stat = max(abs(cell_range[var])
                        for var in ['min', 'max'])
        if params.cell_clip and not params.quant_c_state_with_stat:
            cell_max = params.cell_clip
            ratio_c = cell_max / cell_stat
            if not (ratio_c > 0.9 and ratio_c < 1.1):
                LOG.warning(
                    f"C state is forced to a range [-{cell_max}:{cell_max}] different to the one calulated "
                    f"from the inference statistic [-{cell_stat}:{cell_stat}], consider using nodeoption {params.name} "
                    "QUANT_C_STATE_WITH_STAT 1 to force it to be the one calculated")
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

        scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['i', 'o', 'c', 'f']}
        for weight_name in [weight_name for scale_pair in scale_pairs.values() for weight_name in scale_pair]:
            in_qs[names[weight_name]] = deepcopy(in_qs[names[weight_name]])
            in_qs[names[weight_name]].dtype = np.int8
            in_qs[names[weight_name]].bits = opts['weight_bits']

        w_scales = [(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                    for k, (namei, namer) in scale_pairs.items()]
        if (abs(1 - in_qs[0].scale / o_q.scale) < 0.1) and \
                all([(abs(1 - w_scale[0] / w_scale[1]) < 0.2) for w_scale in w_scales]):
            LOG.info("node %s has similar input and i_state scales --> "
                     "will be generated the same_scale kernel with better performances", params.name)
            params.rnn_same_inout_scale = True
            G.node_options[NodeId(params)] = params.at_options

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

        i_pscales = {k: in_qs[names["i_2_%s_w" % k]
                              ].scale * in_scale for k in ['i', 'o', 'c', 'f']}
        # if input and i_state have different scales -> scale the inputs before sum
        # otherwise do nothing and these scales will be ignored
        scale_qtypes.update({"i_2_%s_q" % k: MultMulBiasScaleQType(
            scale=i_pscale/r_pscale) for (k, i_pscale), r_pscale in zip(i_pscales.items(), r_pscales.values())})

        if params.hard_act:
            cell_in_scale = in_qs[names['c_state']].scale/int_scale
            cell_out_scale = int2_scale/in_qs[names['c_state']].scale
            state_out_scale = int3_scale/i_state_scale
        else:
            cell_in_scale = in_qs[names['c_state']
                                  ].scale*out_tanh_sig_scale/int_scale
            cell_out_scale = int_scale/in_qs[names['c_state']].scale
            state_out_scale = out_tanh_sig_scale/i_state_scale

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
            in_qs[names[f"{gate}_b"]].scale = r_pscales[gate]
            in_qs[names[f"{gate}_b"]].dtype = np.int32
        if params.lstm_output_c_state:
            out_qs = [o_q, in_qs[names['c_state']]]
        else:
            out_qs = [o_q]
        return QRec.scaled(
            in_qs=in_qs,
            out_qs=out_qs,
            **scale_qtypes,
        )
