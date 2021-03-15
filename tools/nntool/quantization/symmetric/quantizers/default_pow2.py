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

from quantization.symmetric.symmetric_quantization import \
    SymmetricQuantizationRecord
from quantization.unified_quantization_handler import params_type

from ..pow2_quantization_handler import Pow2QuantizionHandler


@params_type('__default__')
class NoChangePow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        backwards = kwargs.get('backwards')
        # if we are going backwards
        if backwards:
            # if output must be forced
            assert force_out_q, f'going backwards at {params.name} but output is not forced'
            return SymmetricQuantizationRecord(in_qs=[force_out_q] * len(in_qs), out_qs=[deepcopy(force_out_q)])

        # if going forwards and our output is forced and does not match input then
        # we cannot satisfy
        if force_out_q and not all(in_q == force_out_q for in_q in in_qs):
            return None

        return SymmetricQuantizationRecord(in_qs=in_qs, out_qs=[deepcopy(in_qs[0])])
