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
from nntool.expressions.symbolic.float_quantization import \
    FloatQuantization
from nntool.expressions.symbolic.symbol import SymbolStats
from nntool.graph.types import ExpressionFusionNode
from nntool.quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

# Fusion handler attribute not set since expressions are handled only by this handler
@params_type(ExpressionFusionNode)
@in_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16, np.uint16, np.int16, np.uint8, np.int8])}))
@out_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16, np.uint16, np.int16, np.uint8, np.int8])}))
class ExpressionFusionFloat(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        _, dtype = cls.get_float_opts(**kwargs)
        force_out_qs = kwargs.get('force_out_qs')

        expression_stats = stats and stats.get_stats_key('expression')
        if not expression_stats:
            raise ValueError(
                f'no valid range information is present for {params.name}')

        symbol_control = SymbolStats(expression_stats)
        # preload the input and output quantization
        # This will force variables to the right scales in the expression quantizer
        # first the input
        prequant = {params.input_symbols[idx]: in_q for idx, in_q in enumerate(in_qs)}
        # now the output
        o_qs = []
        for idx, sym_name in enumerate(params.output_symbols):
            if force_out_qs and force_out_qs[idx]:
                o_q = force_out_qs[idx]
            else:
                # cls.check_valid_ranges(params, stats, idx=idx, dirs='out')
                o_q = QType(dtype=dtype)
            prequant[sym_name] = o_q
            o_qs.append(o_q)

        qfunc_col = params.func_col.quantize(FloatQuantization,
                                             symbol_control,
                                             quantize_inputs=False,
                                             qtypes=prequant)

        return QRec.float(in_qs=in_qs, out_qs=o_qs, float_dtype=dtype, qfunc_col=qfunc_col)
