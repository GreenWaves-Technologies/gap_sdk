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
from graph.types import SoftMaxParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       needs_stats,
                                                       out_qs_constraint,
                                                       params_type)

from ..pow2_quantization_handler import Pow2QuantizionHandler


@params_type(SoftMaxParameters)
@in_qs_constraint({'dtype': set([np.int8, np.int16])})
@out_qs_constraint({'dtype': np.int16})
@needs_stats(False)
class SoftmaxPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = deepcopy(in_qs[0]).scale_to_pow2()
        in_q.set_forced()
        out_q = QType.Pow2(16, 15, True, forced=True)
        if force_out_q and force_out_q != out_q:
            return None
        return QRec.symmetric(in_qs=[in_q], out_qs=[out_q])
