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
from nntool.graph.types.lstm import LSTMNode
from nntool.quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)


@params_type(LSTMNode)
@in_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@out_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
class LSTMFloat(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        _, dtype = cls.get_float_opts(**kwargs)
        names = {val: idx for idx, val in enumerate(LSTMNode.INPUT_NAMES)}
        edges = kwargs['G'].indexed_in_edges(params.name)
        in_qs = deepcopy(in_qs)
        scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                       for chan in ['i', 'o', 'c', 'f']}
        for scale_pair in scale_pairs.values():
            w_q = in_qs[names[scale_pair[0]]]
            in_qs[names[scale_pair[0]]] = QType(
                min_val=w_q.min_val,
                max_val=w_q.max_val,
                dtype=dtype,
                dont_generate_value=True)
            w_q = in_qs[names[scale_pair[1]]]
            in_qs[names[scale_pair[1]]] = QType(
                min_val=w_q.min_val,
                max_val=w_q.max_val,
                dtype=dtype,
                concatenated_nodes=[edges[names[scale_pair[0]]].from_node.name])

        if params.output_c_state:
            out_qs = [QType(dtype=dtype), QType(dtype=dtype)]
        else:
            out_qs = [QType(dtype=dtype)]

        return QRec.float(in_qs=in_qs, out_qs=out_qs, float_dtype=dtype)
