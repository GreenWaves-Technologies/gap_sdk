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


from graph.types import ConstantInputParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type

from ..pow2_quantization_handler import Pow2QuantizionHandler


@params_type(ConstantInputParameters)
class ConstantInputPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        if force_out_q:
            o_q = force_out_q
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            o_q = QType.from_min_max_pow2(stats['range_out'][0]['min'],
                                          stats['range_out'][0]['max'],
                                          dtype=out_dtype)
        o_q.is_constant = True
        return QRec.symmetric(in_qs=None, out_qs=[o_q])
