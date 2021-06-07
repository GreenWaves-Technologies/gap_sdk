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
from quantization.qtype_constraint import MatchAll
from quantization.quantizers.concat_mixin import ConcatMixin
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(ConcatParameters)
@in_qs_constraint(MatchAll({'dtype': set([np.int8, np.uint8])}))
@out_qs_constraint({'dtype': set([np.int8, np.uint8])})
class ConcatMult(MultQuantizionHandler, ConcatMixin):

    KTYPE = 'scaled'

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._handle(params, in_qs, stats, **kwargs)

    @classmethod
    def _get_common_q(cls, in_qs):
        # if all the inputs are not the same then force all of them to the maximum input size with a Q that
        # fits the most int bits
        # TODO - Need to handle unsigned / signed
        # take maximum amount of inputs of a type to force that type
        max_scale_idx = max(
            [(idx, in_q.scale) for idx, in_q in enumerate(in_qs)], key=lambda x: x[1])[0]
        return in_qs[max_scale_idx]
