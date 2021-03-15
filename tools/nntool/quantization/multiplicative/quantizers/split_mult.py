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
import logging
from graph.types import SplitParameters
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.unified_quantization_handler import params_type

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(SplitParameters)
class SplitMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        o_q = in_qs[0]
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        first_forced_q = force_out_qs and next(
            iter(out_q for out_q in force_out_qs if out_q is not None), None)
        if first_forced_q and not all(out_q == first_forced_q
                                      for out_q in force_out_qs if out_q is not None):
            LOG.error('split %s is being forced to have different output qtypes', params.name)
            return None
        if first_forced_q:
            backwards = kwargs.get('backwards', None)
            if backwards:
                # if going backwards and forced then we force our input
                return MultQuantizationRecord(in_qs=[first_forced_q], out_qs=[deepcopy(first_forced_q)
                                                                            for _ in range(params.num_splits)])
            elif o_q != first_forced_q:
                LOG.error('split %s is being forced to have different output to input', params.name)
                return None
            # continue here if forced since o_q == forced_q

        return MultQuantizationRecord(in_qs=in_qs, out_qs=[deepcopy(o_q) for _ in range(params.num_splits)])
