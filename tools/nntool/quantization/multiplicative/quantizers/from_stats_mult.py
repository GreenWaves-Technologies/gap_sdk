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

from graph.types import (GlobalPoolParameters, ImageFormatParameters,
                         InputParameters, MatrixBroadcastedLinearOpParameters,
                         MatScaleFusionParameters)
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(MatrixBroadcastedLinearOpParameters, InputParameters,
             MatScaleFusionParameters, GlobalPoolParameters, ImageFormatParameters)
class FromStatsMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            o_q = deepcopy(force_out_q)
        else:
            o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                        stats['range_out'][0]['max'],
                                        dtype=out_dtype)

        return MultQuantizationRecord(in_qs=in_qs, out_qs=[o_q])
