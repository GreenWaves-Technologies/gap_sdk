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

import numpy as np
from graph.types import ConstantInputParameters, LSTMParameters
from quantization.multiplicative.mult_quantization import \
    MultScalableLstmQuantizationRecord
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import \
    MultMulBiasScaleQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.qtype import QType
from quantization.quantization_handler import params_type
from utils.node_id import NodeId
from utils.stats_funcs import calc_bits

from ..mult_quantization_handler import MultQuantizionHandler
from .rescale_constant_mixin import RescaleConstantMixin

LOG = logging.getLogger('nntool.' + __name__)


@params_type(LSTMParameters)
class LSTMMultMult(RescaleConstantMixin, MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        qrecs = kwargs['qrecs']
        G = kwargs['G']

        o_q = SymmetricMultQType.from_min_max(min_val=stats['range_out'][0]['min'],
                                              max_val=stats['range_out'][0]['max'],
                                              dtype=out_dtype)
        input_nodes = {LSTMParameters.INPUT_NAMES[edge.to_idx]: edge.from_node
                       for edge in G.in_edges(params.name)
                       if isinstance(edge.from_node, ConstantInputParameters)}
        names = {val: idx for idx, val in enumerate(LSTMParameters.INPUT_NAMES)}
        if params.cell_clip:
            cell_max = params.cell_clip
        else:
            cell_max = max(abs(stats['range_cell'][var]) for var in ['min', 'max'])

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
            out_tanh_sig_scale = math.pow(2, -15)  # output of LUT activations are always Q15
        int_scale = math.pow(2, -int_q)

        if np.isclose(in_qs[0].scale, o_q.scale, atol=1e-2):
            LOG.info("node %s has similar input and i_state scales --> "
                     "will be generated the same_scale kernel with better performances", params.name)
            params.rnn_same_inout_scale = True
            G.node_options[NodeId(params)] = params.at_options

        if params.rnn_same_inout_scale:
            if not np.isclose(in_qs[0].scale, o_q.scale, atol=1e-2):
                LOG.warning(
                    "node %s has different input and i_state scales consider using the "
                    "LSTM kernel with rnn_same_inout_scale=False (better accuracy)", params.name)
            # in and out and state are all in the same scale
            in_and_out_scale = np.maximum(in_qs[0].scale, o_q.scale)
            i_state_scale = in_scale = np.maximum(in_and_out_scale, in_qs[names['i_state']].scale)
            in_qs[0].scale = in_scale
            o_q.scale = in_scale
            if not params.rnn_states_as_inputs:
                cls.rescale_constant(input_nodes['i_state'], i_state_scale, qrecs)
        else:
            in_scale = in_qs[0].scale
            i_state_scale = np.maximum(o_q.scale, in_qs[names['i_state']].scale)
            o_q.scale = i_state_scale
        scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['i', 'o', 'c', 'f']}
        scales = {k: np.maximum(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                  for k, (namei, namer) in scale_pairs.items()}
        for k, (namei, namer) in scale_pairs.items():
            cls.rescale_constant(input_nodes[namei], scales[k], qrecs)
            cls.rescale_constant(input_nodes[namer], scales[k], qrecs)
        # compute scales for perceptrons
        pscales = {k: scales[k] * i_state_scale for k in ['i', 'o', 'c', 'f']}
        scale_qtypes = {"r_2_%s_q" % k: MultMulBiasScaleQType(
            scale=pscale/int_scale) for k, pscale in pscales.items()}

        # if input and i_state have different scales -> scale the inputs before sum
        # otherwise do nothing and these scales will be ignored
        scale_qtypes.update({"i_2_%s_q" % k: MultMulBiasScaleQType(
            scale=in_scale/i_state_scale) for k in ['i', 'o', 'c', 'f']})

        if params.hard_act:
            cell_in_scale = in_qs[names['c_state']].scale/int_scale
            cell_out_scale = int2_scale/in_qs[names['c_state']].scale
            state_out_scale = int3_scale/i_state_scale
        else:
            cell_in_scale = in_qs[names['c_state']].scale*out_tanh_sig_scale/int_scale
            cell_out_scale = int_scale/in_qs[names['c_state']].scale
            state_out_scale = out_tanh_sig_scale/i_state_scale

        scale_qtypes['cell_in_q'] = MultMulBiasScaleQType(scale=cell_in_scale)
        # TODO - Check cell clip here
        scale_qtypes['cell_out_q'] = MultMulBiasScaleQType(scale=cell_out_scale)
        scale_qtypes['state_out_q'] = MultMulBiasScaleQType(scale=state_out_scale)
        # set internal scale
        scale_qtypes['i_qtype'] = QType(q=int_q, bits=32, signed=True)
        # set biases to output of perceptron
        for gate in ['i', 'o', 'c', 'f']:
            cls.rescale_constant(input_nodes["%s_b" % gate], pscales[gate], qrecs, dtype=np.int32)
        if params.lstm_output_c_state:
            out_qs = [o_q, in_qs[names['c_state']]]
        else:
            out_qs = [o_q]
        return MultScalableLstmQuantizationRecord(
            in_qs=in_qs,
            out_qs=out_qs,
            **scale_qtypes,
        )
