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
from typing import cast

import numpy as np
from bfloat16 import bfloat16
from graph.types import QuantizeParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler


def is_signed_to_unsigned(in_q, out_q):
    if in_q.dtype not in set([np.int8, np.uint8]):
        return False
    if out_q.dtype not in set([np.int8, np.uint8]):
        return False
    if in_q.dtype == out_q.dtype:
        return False
    return True


def match_zero_point_and_scale(from_q, forced):
    if from_q.dtype == np.uint8:
        zero_point = from_q.zero_point - 128
        dtype = np.int8
    else:
        zero_point = from_q.zero_point + 128
        dtype = np.uint8

    return QType(scale=from_q.scale, min_val=from_q.min_val,
                 max_val=from_q.max_val, dtype=dtype,
                 zero_point=zero_point, forced=forced)


@params_type(QuantizeParameters)
@out_qs_constraint({'dtype': set([np.uint8, np.int8])})
class HandleQuantizeMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        params = cast(QuantizeParameters, params)
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = in_qs[0]
        if force_out_q:
            o_q = deepcopy(force_out_q)
            if params.to_qtype.dtype == np.uint8 and in_q.dtype == np.int8:
                if np.all(in_q.zero_point == -128):
                    cls.check_valid_ranges(params, stats, idx=0, dirs='out')
                    o_q = QType.from_min_max_sq(0,
                                                in_q.max_val,
                                                dtype=np.uint8,
                                                asymmetric=True)
                else:
                    in_zero_point = o_q.zero_point - 128
                    if in_q.forced_zero_point and in_q.zero_point != in_zero_point:
                        return None
                    in_q = QType(scale=o_q.scale, min_val=o_q.min_val, max_val=o_q.max_val,
                                 dtype=in_q.dtype, zero_point=in_zero_point)
            elif params.to_qtype.dtype == np.int8 and in_q.dtype == np.uint8:
                if np.all(in_q.zero_point == 0):
                    cls.check_valid_ranges(params, stats, idx=0, dirs='out')
                    o_q = QType.from_min_max_sq(0,
                                                in_q.max_val,
                                                dtype=np.int8,
                                                asymmetric=True)
                else:
                    in_zero_point = o_q.zero_point + 128
                    if in_q.forced_zero_point and in_q.zero_point != in_zero_point:
                        return None
                    in_q = QType(scale=o_q.scale, min_val=o_q.min_val, max_val=o_q.max_val,
                                dtype=in_q.dtype, zero_point=in_zero_point)
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            if params.to_qtype is None and 'output_dtypes' in kwargs:
                o_dtype = kwargs['output_dtypes'][0]
                if isinstance(o_dtype, set):
                    o_dtype = next(o_dtype.__iter__())
            else:
                o_dtype = params.to_qtype.dtype if params.to_qtype is not None else None

            # moving to NE16
            if o_dtype == np.uint8 and in_q.dtype == np.int8:
                if in_q.forced_zero_point:
                    # if input is forced then match input zero point so that the scale also matches
                    in_zero_point = in_q.zero_point if in_q.zero_point is not None else np.array([
                                                                                                 0])
                    out_zero_point = in_zero_point + 128
                    o_q = QType(min_val=stats['range_out'][0]['min'],
                                max_val=stats['range_out'][0]['max'],
                                scale=in_q.scale,
                                zero_point=out_zero_point,
                                dtype=np.uint8)
                else:
                    # try to produce something asymmetric
                    o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                                stats['range_out'][0]['max'],
                                                dtype=np.uint8,
                                                asymmetric=True)
                    in_zero_point = o_q.zero_point - 128
                    in_q = QType(scale=o_q.scale, min_val=o_q.min_val, max_val=o_q.max_val,
                                 dtype=in_q.dtype, zero_point=in_zero_point)
            elif o_dtype == np.int8 and in_q.dtype == np.uint8:
                if in_q.forced_zero_point:
                    # if input is forced then match input zero point so that the scale also matches
                    out_zero_point = in_q.zero_point - 128
                    o_q = QType(min_val=stats['range_out'][0]['min'],
                                max_val=stats['range_out'][0]['max'],
                                scale=in_q.scale,
                                zero_point=out_zero_point,
                                dtype=np.int8)
                else:
                    # try to produce something asymmetric
                    o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                                stats['range_out'][0]['max'],
                                                dtype=np.int8,
                                                asymmetric=True)
                    in_zero_point = o_q.zero_point + 128
                    in_q = QType(scale=o_q.scale, min_val=o_q.min_val, max_val=o_q.max_val,
                                 dtype=in_q.dtype, zero_point=in_zero_point)
            elif o_dtype in [np.float32, np.float16, bfloat16]:
                o_q = QType(dtype=o_dtype, max_val=stats['range_out'][0]['max'], min_val=stats['range_out'][0]['min'])
            else:
                o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                            stats['range_out'][0]['max'],
                                            dtype=o_dtype,
                                            asymmetric=params.to_qtype and params.to_qtype.asymmetric)

        params.from_qtype = in_q
        params.to_qtype = o_q
        return QRec.scaled(in_qs=[in_q], out_qs=[o_q])
