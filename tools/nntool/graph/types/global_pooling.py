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

from ..dim import Dim
from .base import SingleInputAndOutput, Transposable, cls_op_name

LOG = logging.getLogger("nntool." + __name__)


class GlobalPoolingParameters(Transposable, SingleInputAndOutput):
    POOL_TYPE = None

    def __new__(cls, *args, pool_type="max", axis=None, keep_dims=None, **kwargs):
        if pool_type == 'max':
            return super(GlobalPoolingParameters, cls).__new__(GlobalMaxPoolParameters)
        if pool_type == 'max':
            return super(GlobalPoolingParameters, cls).__new__(GlobalSumPoolParameters)
        return super(GlobalPoolingParameters, cls).__new__(GlobalAveragePoolParameters)

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name: str, pool_type="max", axis=None, keep_dims=None, **kwargs):
        super(GlobalPoolingParameters, self).__init__(name, **kwargs)
        self._axis = axis
        self._keep_dims = keep_dims
        LOG.debug("created GLOBAL POOL %s", str(self))

    def __deepcopy__(self, memo):
        duplicate = GlobalPoolingParameters.__new__(
            GlobalPoolingParameters, pool_type=self.pool_type)
        for k, v in self.__dict__.items():
            setattr(duplicate, k, deepcopy(v, memo=memo))
        return duplicate

    @property
    def axis(self):
        return self._axis

    @axis.setter
    def axis(self, val):
        self._axis = val

    @property
    def keep_dims(self):
        return self._keep_dims

    @keep_dims.setter
    def keep_dims(self, val):
        self._keep_dims = val

    @property
    def pool_type(self):
        return self.POOL_TYPE

    @property
    def op_name(self):
        return "global_{}_pool".format(self.pool_type)

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        if self.transpose_in:
            out_dim.transpose(self.transpose_in[0])

        if self.keep_dims:
            names = out_dim.keys if out_dim.is_named else None
            out_dim = Dim(shape=[1 if idx in self._axis else dim
                                 for idx, dim in enumerate(out_dim.shape)],
                          names=names, is_ordered=True)
            if self.transpose_out:
                out_dim.transpose(self.transpose_out[0])
        else:
            out_dim = Dim(shape=[dim for idx, dim in enumerate(out_dim.shape)
                                 if idx not in self._axis],
                          is_ordered=True)

        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def compute_load(self):
        if self.pool_type == "max":
            return self.out_dims[0].size()
        else:
            return self.out_dims[0].size() + 1

    def __str__(self):
        return "{} A {}{} {} {}".format(
            self.pool_type,
            self._axis,
            " keep_dims " if self._keep_dims else "",
            Transposable.__str__(self),
            self.at_options
        )


@cls_op_name('global_max_pool')
class GlobalMaxPoolParameters(GlobalPoolingParameters):
    POOL_TYPE = 'max'


@cls_op_name('global_average_pool')
class GlobalAveragePoolParameters(GlobalPoolingParameters):
    POOL_TYPE = 'average'


@cls_op_name('global_sum_pool')
class GlobalSumPoolParameters(GlobalPoolingParameters):
    POOL_TYPE = 'sum'
