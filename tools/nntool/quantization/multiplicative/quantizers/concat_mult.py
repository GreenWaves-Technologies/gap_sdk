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

from graph.types import ConcatParameters
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(ConcatParameters)
class ConcatMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        backwards = kwargs.get('backwards')
        # if we are going backwards
        if backwards:
            # if output must be forced
            assert force_out_q, f'going backwards at {params.name} but output is not forced'
            return MultQuantizationRecord(in_qs=[force_out_qs[0]] * len(in_qs), out_qs=[deepcopy(force_out_qs[0])])

        # if going forwards and our output is forced and does not match input then
        # we cannot satisfy
        if force_out_q and not all(in_q == force_out_q for in_q in in_qs):
            return None

        # if all the inputs are the same qtype then we output that qtype
        if all(in_qs[0] == in_q for in_q in in_qs[1::]):
            return MultQuantizationRecord(in_qs=in_qs, out_qs=[deepcopy(in_qs[0])])

        # our output cannot be forced at this point
        # if all the inputs are not the same then force all of them to the maximum input size with a Q that
        # fits the most int bits
        max_scale_idx = max([(idx, in_q.scale) for idx, in_q in enumerate(in_qs)], key=lambda x: x[1])[0]
        max_scale_q = in_qs[max_scale_idx]

        return MultQuantizationRecord(in_qs=[max_scale_q] * len(in_qs), out_qs=[deepcopy(max_scale_q)])
