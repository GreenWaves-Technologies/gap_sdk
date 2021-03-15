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


from graph.types import SoftMaxParameters
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type
from quantization.symmetric.symmetric_quantization import \
    SymmetricQuantizationRecord

from ..pow2_quantization_handler import Pow2QuantizionHandler


@params_type(SoftMaxParameters)
class SoftmaxPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        out_q = QType.Pow2(16, 15, True)
        if force_out_q and force_out_q != out_q:
            return None
        return SymmetricQuantizationRecord(in_qs=in_qs, out_qs=[QType.Pow2(16, 15, True)])
