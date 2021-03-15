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
from graph.types.fusions import FusionInputParameters
from graph.types import (GlobalPoolParameters, MatrixAddParameters,
                         MatrixBroadcastedLinearOpParameters,
                         MatrixSubParameters, MatScaleFusionParameters, InputParameters)
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type
from quantization.symmetric.symmetric_quantization import \
    SymmetricQuantizationRecord

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(MatrixAddParameters, MatrixSubParameters,
             MatrixBroadcastedLinearOpParameters,
             MatScaleFusionParameters, GlobalPoolParameters,
             InputParameters, FusionInputParameters)
class FromStatsMultiInputPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        o_q = QType.from_min_max_pow2(stats['range_out'][0]['min'],
                                      stats['range_out'][0]['max'],
                                      dtype=out_dtype)
        if force_out_q:
            if force_out_q.bits - force_out_q.q < o_q.bits - o_q.q:
                LOG.warning('%s is being forced to output in Q%s and may clip',
                            params.name, force_out_q.q)
            o_q = force_out_q

        return SymmetricQuantizationRecord(in_qs=in_qs, out_qs=[o_q])
