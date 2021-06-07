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
from copy import deepcopy

from quantization.new_qrec import QRec
from quantization.qtype import QType

LOG = logging.getLogger('nntool.' + __name__)


class SplitMixin():
    @classmethod
    def _handle(cls, params, in_qs, stats, ktype, **kwargs):
        forced_out_qs = kwargs.get('force_out_qs')
        if forced_out_qs:
            # some could not be forced
            forced_out_qs = [
                qtype for qtype in forced_out_qs if qtype is not None]
        forced_in_qs = [in_q for in_q in in_qs if in_q.forced]
        forced_in_q = forced_in_qs[0] if forced_in_qs else None
        # two inputs cannot be forced to different values
        if forced_out_qs and not QType.forced_equal(*forced_out_qs):
            LOG.info(
                'two output qtypes of split %s are forced to different qtypes', params.name)
            return None
        # input cannot be forced to different value than output
        if forced_in_q and forced_out_qs and not forced_in_q.can_force(*forced_out_qs):
            LOG.error(
                'output and input of split %s are forced to different qtypes', params.name)
            return None

        # now if forced we are compatible with the split
        forced_out_q = forced_out_qs[0] if forced_out_qs else None

        if forced_in_q:
            out_qs = [deepcopy(forced_in_q) for _ in range(params.num_splits)]
            return QRec(ktype=ktype, in_qs=[deepcopy(forced_in_q)], out_qs=out_qs)

        if forced_out_q:
            out_qs = [deepcopy(forced_out_q) for _ in range(params.num_splits)]
            return QRec(ktype=ktype, in_qs=[deepcopy(forced_out_q)], out_qs=out_qs)

        out_qs = [deepcopy(in_qs[0]) for _ in range(params.num_splits)]
        return QRec(ktype=ktype, in_qs=[deepcopy(in_qs[0])], out_qs=out_qs)
