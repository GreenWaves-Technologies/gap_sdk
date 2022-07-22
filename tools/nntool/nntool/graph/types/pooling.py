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

from ..dim import Dim, PoolFilterDim
from .base import FilterLikeNodeBase, SensitiveToOrderMixin, cls_op_name

LOG = logging.getLogger(__name__)


class PoolingNodeBase(FilterLikeNodeBase, SensitiveToOrderMixin):
    POOL_TYPE = None

    def __new__(cls, *args, pool_type="max", **kwargs):
        if pool_type == 'max':
            instance = super(PoolingNodeBase, cls).__new__(MaxPoolNode)
        else:
            instance = super(PoolingNodeBase, cls).__new__(AveragePoolNode)
        instance.__init__(*args, **kwargs)
        return instance

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name: str, filter_dim=None, pool_type="max", **kwargs):
        super(PoolingNodeBase, self).__init__(name, **kwargs)
        if isinstance(filter_dim, (tuple, list)):
            filter_dim = PoolFilterDim(*filter_dim)
        self.filter_dim = filter_dim
        LOG.debug("created POOL %s", str(self))

    def __deepcopy__(self, memo):
        duplicate = PoolingNodeBase.__new__(
            PoolingNodeBase,
            self.__dict__['_name'],
            pool_type=self.pool_type)
        for k, v in self.__dict__.items():
            setattr(duplicate, k, deepcopy(v, memo=memo))
        return duplicate

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs['filter_dim'] = self.filter_dim
        return super_attrs

    @property
    def graph_label(self):
        return [self.name, self.pool_type, str(self.filter_dim)]

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

        if self.filter_dim is None:
            self.filter_dim = PoolFilterDim(in_dims.h, in_dims.w)

        if self.padding.is_same:
            self.padding.calculate_same(in_dims, self.filter_dim, self.stride)

        pad = self.padding.height_width()

        h = ((in_dims.h - self.filter_dim.h + pad.h)//self.stride.h) + 1
        w = ((in_dims.w - self.filter_dim.w + pad.w)//self.stride.w) + 1
        if h < 0 or w < 0:
            raise ValueError(f'{self.name}dimension calculation invalid {h}, {w}')

        out_dim = Dim.named_ordered(c=in_dims.c, h=h, w=w)
        out_dim.impose_order(in_dims.order)

        return [out_dim]

    def compute_load(self):
        if self.pool_type == "max":
            return self.out_dims[0].size() * self.filter_dim.h * self.filter_dim.w
        else:
            return (self.out_dims[0].size() * self.filter_dim.h *
                    self.filter_dim.w) + self.out_dims[0].size()

    def __str__(self):
        return "T {} F {} S {} P {} {} {}".format(
            self.pool_type,
            self.filter_dim,
            self.stride,
            self.padding,
            self.pad_type,
            self.at_options
        )


@cls_op_name('max_pool')
class MaxPoolNode(PoolingNodeBase):
    POOL_TYPE = 'max'


@cls_op_name('average_pool')
class AveragePoolNode(PoolingNodeBase):
    POOL_TYPE = 'average'
