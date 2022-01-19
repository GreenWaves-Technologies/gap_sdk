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
from graph.types import MatMulOpParameters
from graph.types.activations import ReluActivationParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)
from utils.node_id import NodeId

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(MatMulOpParameters)
@in_qs_constraint({'dtype': set([np.int8, np.int16]), 'dtype': set([np.int8, np.int16])})
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
class MatMultPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, params_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        fusion = kwargs.get('fusion', None)

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        if fusion:
            activation = fusion.contained_nodes()[1]
            if isinstance(activation, ReluActivationParameters):
                # Take stats from activation after the convolution
                range_out = kwargs['all_stats'][NodeId(
                    fusion, activation)]['range_out'][0]
                out_dtype = np.int32
        else:
            out_dtype = params_dtype
            range_out = stats['range_out'][0]

        in_q1 = deepcopy(in_qs[0]).scale_to_pow2()
        in_q2 = deepcopy(in_qs[1]).scale_to_pow2()
        biases_q = QType.Pow2(32, in_q1.q + in_q2.q, True)

        if force_out_q:
            o_q = force_out_q
        else:
            o_q = QType.from_min_max_pow2(range_out['min'],
                                          range_out['max'],
                                          dtype=out_dtype)
        if len(in_qs) == 3:
            return QRec.symmetric(in_qs=[in_q1, in_q2, biases_q], out_qs=[o_q])
        return QRec.symmetric(in_qs=[in_q1, in_q2], out_qs=[o_q])
