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
from graph.types import ExpressionFusionParameters
from quantization.multiplicative.mult_quantization import \
    MultExpressionQuantizationRecord
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type
from utils.symbolic.q15_quantization.q15_scaled_quantization import \
    Q15ScaledQuantization
from utils.symbolic.symbol import SymbolStats

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(ExpressionFusionParameters)
class ExpressionFusionMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        if force_out_qs and any(force_q is not None for force_q in force_out_qs):
            return None
        symbol_control = SymbolStats(stats['expression'])
        qfunc_col = params.func_col.quantize(Q15ScaledQuantization,
                                             symbol_control,
                                             quantize_inputs=False)

        o_qs = [QType(scale=qfunc_col.qrecs[sym_name].scaledq, dtype=np.int8)
                for sym_name in params.output_symbols]
        return MultExpressionQuantizationRecord(in_qs=in_qs,
                                                out_qs=o_qs,
                                                qfunc_col=qfunc_col)
