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
from graph.types import OutputParameters
from quantization.new_qrec import QRec
from quantization.quantizer_options import QTYPE_IND_OPTION
from quantization.unified_quantization_handler import (out_qs_constraint, options,
                                                       params_type, needs_stats)

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(OutputParameters)
@out_qs_constraint({'dtype': set([np.int8, np.uint8, np.int16])})
@needs_stats(False)
@options(QTYPE_IND_OPTION)
class OutputMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        opts = kwargs['opts']
        in_q_ind = opts.get('qtype_ind')
        if in_q_ind:
            in_q = deepcopy(in_q_ind)
        else:
            in_q = deepcopy(in_qs[0])
        return QRec.scaled(in_qs=[in_q], out_qs=[in_q])
