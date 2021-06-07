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

import logging

import numpy as np
from bfloat16 import bfloat16
from graph.types import ConcatParameters
from quantization.qtype_constraint import MatchAll
from quantization.quantizers.concat_mixin import ConcatMixin
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..float_quantization_handler import FloatQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(ConcatParameters)
@in_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@out_qs_constraint({'dtype': set([np.float32, np.float16, bfloat16])})
class ConcatFloat(FloatQuantizionHandler, ConcatMixin):

    KTYPE = 'float'

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._handle(params, in_qs, stats, **kwargs)

    @classmethod
    def _get_common_q(cls, in_qs):
        return in_qs[0]
