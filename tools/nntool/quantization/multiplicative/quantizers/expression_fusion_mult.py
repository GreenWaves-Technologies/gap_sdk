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
from expressions.symbolic.q15_quantization.q15_scaled_quantization import \
    Q15ScaledQuantization
from expressions.symbolic.symbol import SymbolStats
from graph.types import ExpressionFusionParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.qtype_constraint import MatchAll
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)
from bfloat16 import bfloat16
from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

@params_type(ExpressionFusionParameters)
@in_qs_constraint(MatchAll({'dtype': {np.int8, np.uint8, np.int16, np.uint16, np.float16, bfloat16}}))
@out_qs_constraint(MatchAll({'dtype': {np.int8, np.uint8, np.int16, np.uint16, np.float16, bfloat16}}))
class ExpressionFusionMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)

        if stats is None or 'expression' not in stats:
            raise ValueError(
                f'no valid range information is present for {params.name}')

        # # expressions need a symmetric input
        # in_qs = cls.force_symmetric(in_qs)

        # if in_qs is None:
        #     LOG.info('expression quantizer for {params.name} was not able to force input symmetric')
        #     return None

        symbol_control = SymbolStats(stats['expression'])
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
                cls.check_valid_ranges(params, stats, idx=idx, dirs='out')
                o_q = QType.from_min_max_sq(stats['range_out'][idx]['min'],
                                            stats['range_out'][idx]['max'],
                                            dtype=np.int8)
            prequant[sym_name] = o_q
            o_qs.append(o_q)

        qfunc_col = params.func_col.quantize(Q15ScaledQuantization,
                                             symbol_control,
                                             quantize_inputs=False,
                                             qtypes=prequant)

        return QRec.scaled(in_qs=in_qs,
                           out_qs=o_qs,
                           qfunc_col=qfunc_col)
