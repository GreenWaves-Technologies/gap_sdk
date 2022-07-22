# Copyright (C) 2021  GreenWaves Technologies, SAS

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

import numpy as np

from ..dim import Dim, FcFilterDim
from .base import (MultiplicativeBiasNodeBase, SingleInputAndOutputMixin,
                   cls_op_name, nargs)

LOG = logging.getLogger(__name__)


@cls_op_name('linear')
@nargs({2, 3})
class LinearNode(MultiplicativeBiasNodeBase, SingleInputAndOutputMixin):
    def __init__(self, *args, filter_dim=None, batch_size=1, keep_dims=False, has_bias=True,
                 _batch_minor=False,
                 ker_in_order=None, ker_out_order=None, **kwargs):
        if isinstance(filter_dim, (list, tuple)):
            filter_dim = FcFilterDim(*filter_dim)
        if ker_in_order is None:
            ker_in_order = [['c'], ['out_c', 'in_c'], ['out_c']]
        if ker_out_order is None:
            ker_out_order = [['c']]
        super(LinearNode, self).__init__(
            *args,
            filter_dim=filter_dim, has_bias=True,
            ker_in_order=ker_in_order, ker_out_order=ker_out_order,
            **kwargs)
        self._keep_dims = keep_dims
        self._batch_size = batch_size
        self._batch_minor = _batch_minor

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.batch_size != 1:
            super_attrs['batch_size'] = self.batch_size
        super_attrs.update({k: getattr(self, k) for k in [
            "batch_minor", "keep_dims"
        ] if getattr(self, k)})
        return super_attrs

    @property
    def graph_anon_label(self):
        return ['Filt']

    @property
    def graph_label(self):
        if self.batch_size > 1:
            batch = [
                f'Batches {self.batch_size} {"minor" if self._batch_minor else "major"}']
        else:
            batch = []
        return [self.name, f'Filter {self.filter_dim}'] + batch

    @property
    def keep_dims(self):
        return self._keep_dims

    @property
    def batch_size(self):
        return self._batch_size

    @property
    def batch_minor(self):
        return self._batch_minor

    @batch_minor.setter
    def batch_minor(self, val):
        self._batch_minor = val

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        if self._batch_size > 1:
            if self._batch_minor:
                out_dim = Dim.named(c=self.filter_dim.out_c,
                                    n=self._batch_size, order=True)
            else:
                out_dim = Dim.named(n=self._batch_size,
                                    c=self.filter_dim.out_c, order=True)
        else:
            out_dim = Dim.named(c=self.filter_dim.out_c, order=True)
        return [out_dim]

    @property
    def can_equalize(self):
        return True

    def compute_load(self):
        return self.in_dims[0].size() * self.out_dims[0].c

    def __str__(self):
        return "F {} B {} {}".format(self.filter_dim, self.batch_size, self.at_options or "")
