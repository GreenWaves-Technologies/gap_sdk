# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

from ..dim import PoolFilterDim
from .base import FilterLikeParameters

LOG = logging.getLogger("nntool." + __name__)

class PoolingParameters(FilterLikeParameters):

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name: str, filt, stride, padding, pad_type="zero", pool_type="max",
                 in_dims_hint=None, out_dims_hint=None):
        super(PoolingParameters, self).__init__(name, stride=stride, padding=padding,
                                                pad_type=pad_type)

        self.in_dims_hint = in_dims_hint
        self.out_dims_hint = out_dims_hint

        if filt is None:
            self.filter = None
        else:
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

    def clone(self, groupn=None):
        return PoolingParameters(self.filter.clone(), self.stride.clone(),\
            self.padding.clone(), self.pad_type, self.pool_type)

    def __str__(self):
        return "T {} F {} S {} P {} {}".format(
            self.pool_type,
            self.filter,
            self.stride,
            self.padding,
            self.pad_type
        )
