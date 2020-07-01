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

from ..dim import PoolFilterDim
from .base import FilterLikeParameters, SensitiveToOrder

LOG = logging.getLogger("nntool." + __name__)

class PoolingParameters(FilterLikeParameters, SensitiveToOrder):

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name: str, filt=None, pool_type="max", **kwargs):
        super(PoolingParameters, self).__init__(name, **kwargs)

        self.filter = filt
        self.pool_type = pool_type
        LOG.debug("created POOL %s", str(self))

    @property
    def op_name(self):
        return "{}_pool".format(self.pool_type)

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        self.in_dims = in_dims
        in_dims = in_dims[0]

        if self.filter is None:
            self.filter = PoolFilterDim(in_dims.h, in_dims.w)

        if self.padding.is_same:
            self.padding.calculate_same(in_dims, self.filter, self.stride)

        pad = self.padding.height_width()
        out_dim = ((in_dims - self.filter + pad)//self.stride) + 1

        out_dim.c = in_dims.c
        out_dim.impose_order(in_dims.order)
        return [out_dim]

    def clone(self, name, groupn=None):
        return PoolingParameters(name, filt=self.filter.clone(), stride=self.stride.clone(),\
            padding=self.padding.clone(), pad_type=self.pad_type, pool_type=self.pool_type)

    def compute_load(self):
        if self.pool_type == "max":
            return self.out_dims[0].size() * self.filter.h * self.filter.w
        else:
            return (self.out_dims[0].size() * self.filter.h *\
                self.filter.w) + self.out_dims[0].size()

    def __str__(self):
        return "T {} F {} S {} P {} {} {}".format(
            self.pool_type,
            self.filter,
            self.stride,
            self.padding,
            self.pad_type,
            self.at_options
        )
