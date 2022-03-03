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

from graph.types import ConstantInputParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType, DTYPES
from quantization.quantizer_options import QTYPE_IND_OPTION, OUTPUT_SIZE_OPTION
from quantization.unified_quantization_handler import (needs_stats, options,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler


@options(
    QTYPE_IND_OPTION,
    OUTPUT_SIZE_OPTION
)
@params_type(ConstantInputParameters)
@needs_stats(False)
class ConstantInputMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        # if forced set what we are forced to
        if force_out_q:
            o_q = deepcopy(force_out_q)
        # if value is already quantized then keep the same quantization
        elif params.qtype:
            o_q = deepcopy(params.qtype)
        # derive quantization from statistics
        else:
            opts = kwargs.get('opts', {})
            output_size = opts.get('output_size')
            if output_size and output_size[0]:
                cur_bits = DTYPES[out_dtype][0]
                bits = min(output_size[0], cur_bits)
            else:
                bits = None
            o_q = QType.from_array_sq(params.value, dtype=out_dtype, bits=bits)
        o_q.is_constant = True
        return QRec.scaled(in_qs=[o_q], out_qs=[o_q])
