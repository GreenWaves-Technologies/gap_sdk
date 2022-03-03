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
from bfloat16 import bfloat16
from quantization.qtype import QType

from ..unified_quantization_handler import (QuantizionHandler, needs_stats,
                                            options, scheme)

FLOAT_DTYPES = {
    'bfloat16': bfloat16,
    'float16': np.float16,
    'ieee16': np.float16,
    'float32': np.float32,
    'ieee32': np.float32
}

#pylint: disable=abstract-method


@options(
    {
        'name': 'float_type',
        'type': str,
        'choices': list(FLOAT_DTYPES),
        'help': 'float type to use for quantization',
        'default': 'float32'
    }
)
@scheme('FLOAT')
@needs_stats(False)
class FloatQuantizionHandler(QuantizionHandler):
    @classmethod
    def get_float_opts(cls, **kwargs):
        force_out_qs = kwargs.get('force_out_qs', None)
        float_type = kwargs['opts']['float_type']
        dtype = FLOAT_DTYPES.get(float_type)
        if dtype is None:
            raise ValueError(f'invalid float_type {float_type}')
        return force_out_qs, dtype

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        float_type = kwargs['opts']['float_type']
        dtype = FLOAT_DTYPES.get(float_type)
        if dtype is None:
            raise ValueError(f'invalid float_type {float_type}')
        if stats:
            return [QType(min_val=stats['range_in'][idx]['min'],
                        max_val=stats['range_in'][idx]['max'],
                        dtype=dtype) if dim is not None else None
                    for idx, dim in enumerate(params.in_dims)]
        return [QType(dtype=dtype) if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]

    @classmethod
    def get_min_max(cls, stats, idx=0, direction='out'):
        if stats:
            return (stats[f'range_{direction}'][idx]['min'],
            stats[f'range_{direction}'][idx]['max'])
        return None, None
