# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

from ..dim import Dim
from .base import FilterParameters, SingleInputAndOutput

LOG = logging.getLogger("nntool." + __name__)

class FcParameters(FilterParameters, SingleInputAndOutput):
    op_name = "linear"
    def __init__(self, name, filt, has_bias=False, sz_order=None,
                 in_dims_hint=None, out_dims_hint=None):

        self.in_dims_hint = in_dims_hint
        self.out_dims_hint = out_dims_hint

        super(FcParameters, self).__init__(name, filt=filt, has_bias=has_bias)
        self.sz_order = sz_order
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

    def clone(self, groupn=None):
        return FcParameters(self.filter.clone(), self.has_bias)

    def __str__(self):
        return "F {}".format(self.filter)
