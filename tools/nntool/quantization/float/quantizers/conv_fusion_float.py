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

import numpy as np
from bfloat16 import bfloat16
from graph.types import ConvFusionParameters, LinearFusionParameters
from quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from quantization.new_qrec import QRec
from quantization.unified_quantization_handler import (fusion_handler,
                                                       in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)


@params_type(ConvFusionParameters, LinearFusionParameters)
@in_qs_constraint({'dtype': set([np.float16, np.float32, bfloat16])})
@out_qs_constraint({'dtype': set([np.float16, np.float32, bfloat16])})
@fusion_handler
class FilterFusionFloat(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        _, dtype = cls.get_float_opts(**kwargs)
        out_qs = [deepcopy(in_qs[0])]
        return QRec.float(in_qs=in_qs, out_qs=out_qs, float_dtype=dtype)
