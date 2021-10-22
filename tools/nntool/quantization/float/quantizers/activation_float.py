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
from bfloat16 import bfloat16
from graph.types.activations import (SigmoidActivationParameters,
                                     TanHActivationParameters)
from quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.qtype_constraint import MatchAll
from quantization.quantizer_options import FLOAT_MATH_KERNEL_TYPE_OPTION
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)


@params_type(SigmoidActivationParameters, TanHActivationParameters)
@in_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@out_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@options(
    FLOAT_MATH_KERNEL_TYPE_OPTION
)
class FloatActivation(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, dtype = cls.get_float_opts(**kwargs)
        if force_out_qs and any(qtype.dtype != dtype for qtype in force_out_qs if qtype is not None):
            return None
        # use cur_G not G here since this may be called inside a fusion
        # cur_G == G or fusion subgraph if inside fusion
        G = kwargs['cur_G']
        in_len = len(G.indexed_in_edges(params.name))
        out_len = len(G.indexed_out_edges(params.name))
        opts = kwargs['opts']
        # all inputs and outputs are set to the required float type
        return QRec.float(in_qs=[QType(dtype=dtype)
                                 for _ in range(in_len)],
                          out_qs=[QType(dtype=dtype)
                                  for _ in range(out_len)],
                          float_dtype=dtype,
                          kernel_type=opts.get('kernel_type'))
