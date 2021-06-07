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
from quantization.qtype import QType
from quantization.new_qrec import QRec


LOG = logging.getLogger('nntool.' + __name__)


class NoChangeMixin():
    @classmethod
    def _handle(cls, params, in_qs, _, ktype, **kwargs):
        force_out_qs = kwargs.get('force_out_qs')
        force_out_q = force_out_qs and force_out_qs[0]
        forced_in_qs = [in_q for in_q in in_qs if in_q.forced]
        # two inputs cannot be forced to different values
        if forced_in_qs and not QType.forced_equal(*forced_in_qs):
            LOG.debug(
                'two input qtypes of %s are forced to different qtypes - rejecting', params.name)
            return None
        # input cannot be forced to different value than output
        if force_out_q and not force_out_q.can_force(force_out_q, *in_qs):
            LOG.debug(
                'output and input of %s are forced to different qtypes - rejecting', params.name)
            return None

        backwards = kwargs.get('backwards')
        if backwards:
            if force_out_q:
                in_qs = [deepcopy(force_out_q) for _ in in_qs]
                return QRec(in_qs=in_qs, out_qs=[deepcopy(force_out_q)], ktype=ktype)
        elif force_out_q and not all(in_q == force_out_q for in_q in in_qs):
            # if going forwards and our output is forced and does not match input then
            # we cannot satisfy
            LOG.debug(
                "output of %s is forced and inputs don't match - rejecting", params.name)
            return None

        return QRec(in_qs=in_qs, out_qs=[deepcopy(in_qs[0])], ktype=ktype)
