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
from nntool.quantization.qtype import QType
from nntool.quantization.quantizer_options import BITS_OPTION_DEFAULT_16

from ..unified_quantization_handler import QuantizionHandler, options, scheme


#pylint: disable=abstract-method
@options(
    BITS_OPTION_DEFAULT_16
)
@scheme('POW2')
class Pow2QuantizionHandler(QuantizionHandler):
    BITS_TO_DTYPE = {
        8: np.int8,
        16: np.int16
    }

    @classmethod
    def get_pow2_opts(cls, **kwargs):
        force_out_qs = kwargs.get('force_out_qs', None)
        opts = kwargs.get('opts', {})
        bits = opts.get('bits', 16)
        return force_out_qs, cls.BITS_TO_DTYPE[bits]

    @classmethod
    def get_pow2_bits(cls, **kwargs):
        opts = kwargs.get('opts', {})
        bits = opts.get('bits', 16)
        return bits

    @classmethod
    def force_pow2(cls, in_qs, idx=None):
        return [QType.from_min_max_pow2(in_q.min_val, in_q.max_val,
                                        dtype=in_q.dtype, forced=True)
                if in_q and (idx is None or idx == in_q_idx) and not in_q.is_pow2 else in_q
                for in_q_idx, in_q in enumerate(in_qs)]

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        dtype = np.int16 if cls.get_pow2_bits(**kwargs) == 16 else np.int8
        return [dtype if dim is not None else None for dim in params.in_dims]

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return [QType.from_min_max_pow2(*stats.get_range_in(idx),
                                        dtype=dtype)
                if dtype is not None else None
                for idx, dtype in enumerate(cls.get_prefered_input_dtypes(params, **kwargs))]
