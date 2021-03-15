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
from graph.types.activations import TanHActivationParameters
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.qtype import \
    QType
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(SoftMaxParameters, TanHActivationParameters)
class SoftmaxTanHMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            return None
        # force the input to be POW2 scaled
        in_q = deepcopy(in_qs[0])
        # in_q.scale_to_pow2()
        o_q = QType(min_val=-1, max_val=1, dtype=np.int16, scale=2**(-15))
        return MultQuantizationRecord(in_qs=[in_q], out_qs=[o_q])
