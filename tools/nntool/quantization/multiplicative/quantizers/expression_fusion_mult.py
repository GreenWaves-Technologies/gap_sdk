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

from graph.types import (ExpressionFusionParameters, FusionInputParameters,
                         FusionOutputParameters)
from quantization.multiplicative.mult_quantization import \
    MultExpressionQuantizationRecord
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(ExpressionFusionParameters)
class ExpressionFusionMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        o_qs = [SymmetricMultQType.from_min_max(min_val=orange['min'],
                                                max_val=orange['max'],
                                                dtype=out_dtype)
                for orange in stats['range_out']]
        fusion_inputs = sorted([n for n in params.subgraph.inputs()
                                if isinstance(n, FusionInputParameters)],
                               key=lambda x: x.idx)
        fusion_outputs = sorted([n for n in params.subgraph.outputs()
                                 if isinstance(n, FusionOutputParameters)],
                                key=lambda x: x.idx)

        node_scale_map = {fnode: in_qs[idx].scale
                          for idx, fnode in enumerate(fusion_inputs)}
        for idx, fnode in enumerate(fusion_outputs):
            node_scale_map[fnode] = o_qs[idx].scale
        inp, outp, expr = params.decompose(node_scale_map=node_scale_map)

        return MultExpressionQuantizationRecord(in_qs=in_qs,
                                                out_qs=o_qs,
                                                inputs=inp,
                                                output_exprs=outp,
                                                intermediate_exprs=expr)
