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
from typing import cast

from graph.types import QuantizeParameters

from quantization.new_qrec import QRec
from quantization.unified_quantization_handler import params_type

from ..unified_quantization_handler import QuantizionHandler


@params_type(QuantizeParameters)
class HandleQuantize(QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        params = cast(QuantizeParameters, params)
        force_out_qs = kwargs.get('force_out_qs', None)
        force_out_q = force_out_qs and force_out_qs[0]
        backwards = kwargs.get('backwards')
        # keep the in_qtype for informational purposes
        params.from_qtype = in_qs[0]
        # if we are going backwards
        if backwards:
            # if output must be forced
            assert force_out_q, f'going backwards at {params.name} but output is not forced'
            # change our output to match the force
            params.to_qtype = deepcopy(force_out_q)
            return QRec(in_qs=deepcopy(in_qs), out_qs=[deepcopy(force_out_q)])

        # copy the out_qtype into the qrec
        return QRec.symmetric(in_qs=in_qs, out_qs=[deepcopy(params.to_qtype)])
