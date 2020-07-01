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
from .base import MultiplicativeBiasParameters, SingleInputAndOutput

LOG = logging.getLogger("nntool." + __name__)

class FcParameters(MultiplicativeBiasParameters, SingleInputAndOutput):
    op_name = "linear"
    def __init__(self, *args, **kwargs):

        super(FcParameters, self).__init__(*args, **kwargs)
        LOG.debug("created LINEAR %s", str(self))

    def get_parameter_size(self):
        return self.filter.size() + (self.has_bias and self.filter.out_c or 0)

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        self.in_dims = in_dims

        out_dim = Dim.named(c=self.filter.out_c, order=True)
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def clone(self, name, groupn=None):
        return FcParameters(name, filt=self.filter.clone(), has_bias=self.has_bias)

    def compute_load(self):
        return self.in_dims[0].size() * self.out_dims[0].c

    def __str__(self):
        return "F {} {}".format(self.filter, self.at_options or "")
