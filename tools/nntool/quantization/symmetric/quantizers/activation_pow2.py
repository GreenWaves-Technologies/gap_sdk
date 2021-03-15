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

from graph.types import ActivationParameters
from quantization.qtype import QType
from quantization.unified_quantization_handler import can_dequantize, params_type
from quantization.symmetric.symmetric_quantization import \
    SymmetricQuantizationRecord
from utils.stats_funcs import bits, calc_bits

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(ActivationParameters)
@can_dequantize(True)
class ActivationPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]            

        if params.activation == "relu6":
            int_bits = calc_bits(6)
        elif params.activation == "relun":
            relun = params.activation_params
            if isinstance(relun, list):
                relun = max(relun)
            int_bits = calc_bits(relun)
        elif params.activation == "relu" or params.activation == "hswish" or params.activation == "hsigmoid" or params.activation == "leaky":
            int_bits = bits(stats['range_out'][0]['max'], stats['range_out'][0]['min'])
        else:
            raise ValueError(f'no support for activation {params.activation} in POW2 quantizer')

        in_q = in_qs[0]
        if force_out_q is None:
            q = max(cls.get_pow2_bits(**kwargs) - int_bits, 0)
            out_q = QType(q=q, dtype=out_dtype)
        else:
            if force_out_q.bits - force_out_q.q < int_bits:
                LOG.warning('quantization is forcing node %s to have an output that may clip', params.name)
            out_q = force_out_q
        return SymmetricQuantizationRecord(in_qs=[in_q], out_qs=[out_q])
