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
from quantization.qtype import QType
from quantization.quantizer_options import SQBITS_OPTION_DEFAULT_8

from ..unified_quantization_handler import QuantizionHandler, options, scheme


#pylint: disable=abstract-method
@options(
    SQBITS_OPTION_DEFAULT_8
)
@scheme('SQ8')
class MultQuantizionHandler(QuantizionHandler):
    BITS_TO_DTYPE = {
        8: np.int8,
        16: np.int16
    }
    DEFAULT_DTYPE = np.int8

    @classmethod
    def get_mult_opts(cls, **kwargs):
        force_out_qs = kwargs.get('force_out_qs', None)
        opts = kwargs.get('opts', {})
        bits = opts.get('sq_bits', 8)
        return force_out_qs, cls.BITS_TO_DTYPE[bits]

    @classmethod
    def force_symmetric(cls, in_qs, idx=None, dtype=None):
        res_qs = []
        for in_q_idx, in_q in enumerate(in_qs):
            if in_qs is None or idx is not None and idx != in_q_idx:
                res_qs.append(in_q)
                continue
            update = False
            if in_q.asymmetric:
                # you need to change scale to change zero point
                if in_q.forced_zero_point or in_q.forced_scale:
                    return None
                update = True
            if dtype is not None and dtype != in_q.dtype:
                if in_q.forced_dtype:
                    return None
                update = True
            if update:
                this_dtype = in_q.dtype if dtype is None else dtype
                in_q = QType.from_min_max_sq(in_q.min_val, in_q.max_val,
                                             dtype=this_dtype)
                in_q.set_forced('zero_point')
            res_qs.append(in_q)
        return res_qs

    @classmethod
    def force_symmetric_and_dtype(cls, in_qs, dtype=None, idx=None):
        return cls.force_symmetric(in_qs, idx=idx, dtype=dtype)

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=cls.DEFAULT_DTYPE)
                if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]

    @classmethod
    def can_handle_asymmetric_input(cls, params, **kwargs):
        return False
