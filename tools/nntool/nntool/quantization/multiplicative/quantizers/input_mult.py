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
from nntool.graph.types import InputNode
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.quantizer_options import ALLOW_ASYMMETRIC_OUT_OPTION_DEFAULT_FALSE, QTYPE_IND_OPTION
from nntool.quantization.unified_quantization_handler import (options,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler


@options(
    ALLOW_ASYMMETRIC_OUT_OPTION_DEFAULT_FALSE,
    QTYPE_IND_OPTION
)
@params_type(InputNode)
@out_qs_constraint({'dtype': set([np.int8, np.uint8, np.int16, np.uint16])})
class InputMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        if in_qs and in_qs[0].dtype in (np.int8, np.uint8, np.int16, np.uint16):
            out_dtype = in_qs[0].dtype
        force_out_q = force_out_qs and force_out_qs[0]
        opts = kwargs['opts']
        o_q_ind = opts.get('qtype_ind')
        if force_out_q:
            o_q = deepcopy(force_out_q)
        elif o_q_ind:
            o_q = deepcopy(o_q_ind)
        else:
            min_val, max_val = stats.get_range_out(0, dtype=out_dtype)
            asymmetric = opts.get('allow_asymmetric_out') or min_val == 0
            o_q = QType.from_min_max_sq(min_val, max_val,
                                        dtype=out_dtype,
                                        asymmetric=asymmetric)

        return QRec.scaled(in_qs=in_qs, out_qs=[o_q])
