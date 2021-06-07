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
from graph.types import ConstantInputParameters
from quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (out_qs_constraint,
                                                       params_type)


@params_type(ConstantInputParameters)
class ConstantInputFloat(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, dtype = cls.get_float_opts(**kwargs)

        if force_out_qs:
            o_q = force_out_qs[0]
        else:
            o_q = QType(dtype=dtype)

        o_q.is_constant = True

        return QRec.float(in_qs=None, out_qs=[o_q], float_dtype=dtype)
