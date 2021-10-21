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

from copy import deepcopy

import numpy as np
from bfloat16 import bfloat16
from graph.types.rnn import RNNParameters
from quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.qtype_constraint import MatchAll
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)


@params_type(RNNParameters)
@in_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@out_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
class RNNFloat(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        _, dtype = cls.get_float_opts(**kwargs)
        names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
        edges = kwargs['G'].indexed_in_edges(params.name)
        in_qs = deepcopy(in_qs)
        w_q = in_qs[names['i_2_i_w']]
        in_qs[names['i_2_i_w']] = QType.from_min_max_sq(
            w_q.min_val, w_q.max_val,
            dtype=dtype,
            dont_generate_value = True)

        w_q = in_qs[names['r_2_i_w']]
        in_qs[names['r_2_i_w']] = QType.from_min_max_sq(
            w_q.min_val, w_q.max_val,
            dtype=dtype,
            concatenated_nodes=[edges[names['i_2_i_w']].from_node.name])

        return QRec.float(in_qs=in_qs, out_qs=[QType(dtype=dtype)], float_dtype=dtype)
