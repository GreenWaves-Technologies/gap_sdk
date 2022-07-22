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
from nntool.graph.types import SplitNode
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.quantizers.split_mixin import SplitMixin
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       needs_stats,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(SplitNode)
@in_qs_constraint({'dtype': set([np.int8, np.uint8])})
@out_qs_constraint(MatchAll({'dtype': set([np.int8, np.uint8])}))
@needs_stats(False)
class SplitMult(MultQuantizionHandler, SplitMixin):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._handle(params, in_qs, stats, 'scaled', **kwargs)
