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

from ..dim import PoolFilterDim
from .base import FilterLikeParameters, Transposable, cls_op_name

LOG = logging.getLogger("nntool." + __name__)


class PoolingParameters(FilterLikeParameters, Transposable):
    POOL_TYPE = None

    def __new__(cls, *args, pool_type="max", **kwargs):
        if pool_type == 'max':
            return super(PoolingParameters, cls).__new__(MaxPoolParameters)
        return super(PoolingParameters, cls).__new__(AveragePoolParameters)

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name: str, filt=None, pool_type="max", **kwargs):
        super(PoolingParameters, self).__init__(name, **kwargs)

        self.filter = filt
        LOG.debug("created POOL %s", str(self))

    def __deepcopy__(self, memo):
        duplicate = PoolingParameters.__new__(PoolingParameters, pool_type=self.pool_type)
        for k, v in self.__dict__.items():
            setattr(duplicate, k, deepcopy(v, memo=memo))
        return duplicate

    @property
    def pool_type(self):
        return self.POOL_TYPE

    @property
    def op_name(self):
        return "{}_pool".format(self.pool_type)

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        in_dims = in_dims[0]

        if self.transpose_in and self.transpose_in[0]:
            in_dims = in_dims.calc_transpose(self.transpose_in[0])

        if self.filter is None:
            self.filter = PoolFilterDim(in_dims.h, in_dims.w)

        if self.padding.is_same:
            self.padding.calculate_same(in_dims, self.filter, self.stride)

        pad = self.padding.height_width()
        out_dim = ((in_dims - self.filter + pad)//self.stride) + 1

        out_dim.c = in_dims.c
        out_dim.impose_order(in_dims.order)
        if self.transpose_out and self.transpose_out[0]:
            out_dim = out_dim.calc_transpose(self.transpose_out[0])

        return [out_dim]

    def compute_load(self):
        if self.pool_type == "max":
            return self.out_dims[0].size() * self.filter.h * self.filter.w
        else:
            return (self.out_dims[0].size() * self.filter.h *
                    self.filter.w) + self.out_dims[0].size()

    def __str__(self):
        return "T {} F {} S {} P {} {} {} {}".format(
            self.pool_type,
            self.filter,
            self.stride,
            self.padding,
            self.pad_type,
            Transposable.__str__(self),
            self.at_options
        )


@cls_op_name('max_pool')
class MaxPoolParameters(PoolingParameters):
    POOL_TYPE = 'max'


@cls_op_name('average_pool')
class AveragePoolParameters(PoolingParameters):
    POOL_TYPE = 'average'
