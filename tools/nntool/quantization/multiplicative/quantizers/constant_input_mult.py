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

from graph.types import ConstantInputParameters
from quantization.multiplicative.mult_quantization import \
    MultConstantQuantizationRecord
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.quantization_handler import params_type, can_dequantize

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(ConstantInputParameters)
@can_dequantize(True)
class ConstantInputMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        o_q = SymmetricMultQType.from_min_max(min_val=stats['range_out'][0]['min'],
                                              max_val=stats['range_out'][0]['max'],
                                              dtype=out_dtype)
        if params.value_quantization:
            return MultConstantQuantizationRecord(out_qs=[params.value_quantization],
                                                  constants_are_quantized=True)
        else:
            return MultConstantQuantizationRecord(out_qs=[o_q],
                                                  constants_are_quantized=False)

    @classmethod
    def _dequantize(cls, params, qrec):
        params.value = qrec.out_q[0].dequantize(params.value)
