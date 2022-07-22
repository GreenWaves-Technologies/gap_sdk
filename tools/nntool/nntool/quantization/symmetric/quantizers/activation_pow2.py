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
from nntool.graph.types import ActivationNodeBase
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.unified_quantization_handler import (can_dequantize,
                                                       in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)
from nntool.utils.stats_funcs import calc_bits

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(ActivationNodeBase)
@can_dequantize(True)
@in_qs_constraint({'dtype': set([np.int8, np.int16, np.int32])})
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
class ActivationPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        fusion = kwargs.get('fusion', None)
        if not fusion and in_qs[0].dtype == np.int32:
            return None

        if params.activation == "relu6":
            int_bits = calc_bits(6)
        elif params.activation == "relun":
            relun = params.activation_params
            if isinstance(relun, list):
                relun = max(relun)
            int_bits = calc_bits(relun)
        elif params.activation in ["relu", "hswish", "hsigmoid", "leaky", "htanh"]:
            # cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            min_val, max_val = stats.get_range_out(0, dtype=out_dtype)
            int_bits = calc_bits(max_val, min_val)
        elif params.activation == "sigmoid" or params.activation == "tanh":
            if force_out_q is None:
                q = 7 if out_dtype == np.int8 else 15
                return QRec.symmetric(in_qs=[in_qs[0]], out_qs=[QType(q=q, dtype=out_dtype)])
            else:
                q = 7 if force_out_q.dtype == np.int8 else 15
                if force_out_q.q != q:
                    return None
                return QRec.symmetric(in_qs=[in_qs[0]], out_qs=[force_out_q])
        else:
            LOG.error(f'no support for activation {params.activation} in POW2 quantizer')
            return None

        in_q = in_qs[0]
        if force_out_q is None:
            q = max(cls.get_pow2_bits(**kwargs) - int_bits, 0)
            out_q = QType(q=q, dtype=out_dtype)
        else:
            if force_out_q.bits - force_out_q.q < int_bits:
                return None
            out_q = force_out_q
        return QRec.symmetric(in_qs=[in_q], out_qs=[out_q])
