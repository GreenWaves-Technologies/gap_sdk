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


from graph.types import ImageFormatParameters
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.qtype import \
    QType
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(ImageFormatParameters)
class ImageFormatterMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            return None

        out_dtype = params.output_dtype
        in_dtype = params.input_dtype
        in_q = QType(scale=1, dtype=in_dtype)
        out_q = QType.from_min_max_sq(-1, 1, dtype=out_dtype, narrow_range=True)

        return MultQuantizationRecord(in_qs=[in_q], out_qs=[out_q])
