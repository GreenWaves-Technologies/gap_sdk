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
from quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (out_qs_constraint,
                                                       params_type)


@params_type(QuantizeParameters)
@out_qs_constraint({'dtype': set([np.float32, np.float16, bfloat16])})
class HandleQuantize(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        params = cast(QuantizeParameters, params)
        force_out_qs, dtype = cls.get_float_opts(**kwargs)
        if force_out_qs:
            o_q = deepcopy(force_out_qs[0])
        else:
            o_q = QType(
                dtype=dtype, min_val=in_qs[0].min_val, max_val=in_qs[0].max_val)

        params.from_qtype = in_qs[0]
        params.to_qtype = o_q
        return QRec.float(in_qs=in_qs, out_qs=[o_q], float_dtype=dtype)
