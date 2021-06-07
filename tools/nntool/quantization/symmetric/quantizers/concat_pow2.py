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
from graph.types import ConcatParameters
from quantization.qtype import QType
from quantization.qtype_constraint import MatchAll
from quantization.quantizers.concat_mixin import ConcatMixin
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(ConcatParameters)
@in_qs_constraint(MatchAll({'dtype': set([np.int8, np.int16])}))
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
class ConcatPow2(Pow2QuantizionHandler, ConcatMixin):

    KTYPE='symmetric'
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._handle(params, in_qs, stats, **kwargs)

    @classmethod
    def _get_common_q(cls, in_qs):
        max_int_bits_idx = max(in_q.bits - in_q.q for in_q in in_qs)
        max_bits = max([in_q.bits for in_q in in_qs])
        common_q = QType(bits=max_bits, q=max_bits -
                         max_int_bits_idx, signed=True)
        return common_q
