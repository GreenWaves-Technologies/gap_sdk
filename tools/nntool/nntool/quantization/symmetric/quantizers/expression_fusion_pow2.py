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
from nntool.expressions.symbolic.q15_quantization.q15_scaled_quantization import \
    Q15ScaledQuantization
from nntool.expressions.symbolic.symbol import SymbolStats
from nntool.graph.types import ExpressionFusionNode
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(ExpressionFusionNode)
@in_qs_constraint(MatchAll({'dtype': np.int16}))
@out_qs_constraint(MatchAll({'dtype': np.int16}))
class ExpressionFusionPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_pow2_opts(**kwargs)


        expression_stats = stats.get_stats_key('expression')
        if expression_stats is None:
            raise ValueError(
                f'no valid range information is present for {params.name}')

        # expressions need a symmetric input
        # this is done on the mult8 version but probably isn't necessary here
        # in_qs = cls.force_symmetric(in_qs)

        symbol_control = SymbolStats(expression_stats)
        # preload the input and output quantization
        # This will force variables to the right scales in the expression quantizer
        # first the input
        prequant = {params.input_symbols[idx]: in_q
                    for idx, in_q in enumerate(in_qs)}
        # now the output
        o_qs = []
        for idx, sym_name in enumerate(params.output_symbols):
            if force_out_qs and force_out_qs[idx]:
                o_q = force_out_qs[idx]
            else:
                o_q = QType.from_min_max_pow2(*stats.get_range_out(idx, dtype=out_dtype),
                                              dtype=out_dtype)
            prequant[sym_name] = o_q
            o_qs.append(o_q)

        qfunc_col = params.func_col.quantize(Q15ScaledQuantization,
                                             symbol_control,
                                             quantize_inputs=False,
                                             qtypes=prequant)

        return QRec.symmetric(in_qs=in_qs, out_qs=o_qs, qfunc_col=qfunc_col)

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        # only works in 16 bit mode
        return [np.int16 for _ in params.in_dims]
