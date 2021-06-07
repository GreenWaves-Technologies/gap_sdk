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
from graph.types import SigmoidActivationParameters, TanHActivationParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(SigmoidActivationParameters, TanHActivationParameters)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
class SigmoidTanHMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            return None
        in_qs = [QType.from_min_max_sq(-8, 8, dtype=np.int8, forced=True)]
        o_q = QType.from_min_max_sq(min_val=-1.0,
                                    max_val=1.0,
                                    dtype=out_dtype,
                                    forced=True)
        return QRec.scaled(in_qs=in_qs, out_qs=[o_q])
