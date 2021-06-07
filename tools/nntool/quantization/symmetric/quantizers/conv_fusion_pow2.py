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
from graph.types import ConvFusionParameters
from quantization.new_qrec import QRec
from quantization.unified_quantization_handler import (fusion_handler,
                                                       in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..pow2_quantization_handler import Pow2QuantizionHandler


@params_type(ConvFusionParameters)
# only constain input. The others are constant so will always be changeable
@in_qs_constraint({'dtype': set([np.int8, np.int16])})
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
@fusion_handler
class GenericFusionPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        out_qs = kwargs['out_qs']
        return QRec.symmetric(in_qs=in_qs, out_qs=out_qs)
