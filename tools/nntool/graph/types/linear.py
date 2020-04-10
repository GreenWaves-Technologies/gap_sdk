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

from ..dim import Dim
from .base import FilterParameters, SingleInputAndOutput

LOG = logging.getLogger("nntool." + __name__)

class FcParameters(FilterParameters, SingleInputAndOutput):
    op_name = "linear"
    def __init__(self, name, **kwargs):

        super(FcParameters, self).__init__(name,
                                           **kwargs)
        LOG.debug("created LINEAR %s", str(self))

    def get_parameter_size(self):
        return self.filter.size() + (self.has_bias and self.filter.out_c or 0)

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        self.in_dims = in_dims
        in_dims = in_dims[0]

        for key in ['h', 'w', 'in_c']:
            in_key = 'c' if key == 'in_c' else key
            if self.filter.has_key(key):
                assert getattr(self.filter, key) == getattr(in_dims, in_key)
            else:
                setattr(self.filter, key, getattr(in_dims, in_key))

        out_dim = Dim.named(c=self.filter.out_c, w=1, h=1, order=in_dims.order)
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def clone(self, name, groupn=None):
        return FcParameters(name, filt=self.filter.clone(), has_bias=self.has_bias)

    def compute_load(self):
        return self.in_dims[0].size() * self.filter.size()

    def __str__(self):
        return "F {} {}".format(self.filter, self.at_options or "")
