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
from graph.types import MatrixAddParameters, MatrixSubParameters
from quantization.multiplicative.mult_quantization import \
    MultAddQuantizationRecord
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(MatrixAddParameters, MatrixSubParameters)
class AddSubMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        # NOTE: The autotiler kernel scales and clips after the operation and before the
        # activation so there is no change if this is in a fusion or not
        scaled_idx = params.force_quantized_index if isinstance(params, MatrixAddParameters) else None

        if force_out_q:
            o_q = deepcopy(force_out_q)
        else:
            o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                        stats['range_out'][0]['max'],
                                        dtype=out_dtype)
        return MultAddQuantizationRecord(in_qs=in_qs, out_qs=[o_q], scaled_idx=scaled_idx)
