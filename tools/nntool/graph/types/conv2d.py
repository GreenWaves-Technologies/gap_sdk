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


from ..dim import DilationDim
from .base import FilterLikeParameters, MultiplicativeBiasParameters, SensitiveToOrder

LOG = logging.getLogger("nntool." + __name__)


class Conv2DParameters(FilterLikeParameters, MultiplicativeBiasParameters, SensitiveToOrder):

    op_name = "conv2d"

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name, dilation=None, groups=None, multiplier=1,
                 custom=None, tf_depthwise=False, **kwargs):

        super(Conv2DParameters, self).__init__(name, **kwargs)

        if dilation is None:
            dilation = DilationDim(1)

        self.dilation = dilation
        self.tf_depthwise = tf_depthwise

        # if groups is not set then this is a normal convolution
        # a depthwise convolution would have groups = filter.out_c and
        # filter.in_c == filter.out_c
        # i.e. there are as many groups as output and input channels
        # The multiplier parameter correspond to the channel multiplier option
        # in tensorflow. There are multiplier filters per input channel in
        # a depthwise convolution
        if groups is None:
            self.groups = 1
        else:
            self.groups = groups

        self.multiplier = multiplier
        assert self.multiplier == 1 or self.tf_depthwise,\
            "multiplier should only be set in TF depthwise convolution"

        if custom is None:
            self.custom = {}
        else:
            self.custom = dict(custom)

        LOG.debug("created CON2D %s", str(self))

    # @property
    # def can_equalize(self):
    #     return not self.tf_depthwise

    def clone(self, name, groupn=None):
        """ Clones conv parameters for a specific group number
        """
        if groupn is None:
            new_conv = Conv2DParameters(name, filter=self.filter, stride=self.stride, padding=self.padding,
                                        dilation=self.dilation, pad_type=self.pad_type, groups=self.groups,
                                        multiplier=self.multiplier, has_bias=self.has_bias)
        else:
            assert groupn < self.groups, "incorrect group number"
            c_per_g = self.filter.out_c//self.groups
            offset = c_per_g * groupn
            new_filter = self.filter.clone()
            new_filter.out_c = c_per_g
            new_conv = Conv2DParameters(name, filter=new_filter, stride=self.stride, padding=self.padding,
                                        dilation=self.dilation, pad_type=self.pad_type, groups=1,
                                        multiplier=self.multiplier, has_bias=self.has_bias)
            num_out = c_per_g * self.multiplier
            if self.biases is not None:
                new_conv.biases = self.biases[offset:offset + num_out - 1]

            if self.weights is not None:
                # TODO - THIS IS NOT CORRECT. CHECK ORDER.
                new_conv.weights = self.weights[offset:offset +
                                                num_out - 1, ...]

        return new_conv

    def is_grouped_conv(self):
        return self.groups > 1

    def is_depthwise_conv(self):
        # this does not cope with TFLITE DW convs with a multiplier but the
        # generator is going to need to split those up into multiple per channel
        # normal convolutions
        return self.groups == self.filter.out_c and self.multiplier == 1

    def get_weights_count(self):
        return self.filter.size() * self.multiplier // self.groups

    def get_bias_count(self):
        return self.multiplier * self.filter.out_c

    def get_parameter_size(self):
        if self.filter.in_c is None:
            return 0
        return self.get_weights_count() + self.get_bias_count()

    @property
    def at_options(self):
        return self._at_options

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1,\
            "Only expecting one input dimension"
        self.in_dims = self.clone_dim_with_hints(in_dims)
        in_dims = self.in_dims[0]

        assert in_dims.c >= self.groups,\
            "The number of groups cannot be larger than the amount of input channels"
        self.filter.in_c = in_dims.c // self.groups
        if self.padding.is_same:
            self.padding.calculate_same(in_dims, self.filter, self.stride, dilation=self.dilation)
        filter_d = self.filter + (self.filter - 1) * (self.dilation - 1)

        pad = self.padding.height_width()

        out_dim = ((in_dims - filter_d + pad)//self.stride) + 1
        out_dim.c = self.filter.out_c
        out_dim.impose_order(in_dims.order)
        return [out_dim]

    def compute_load(self):
        return self.out_dims[0].size() * self.filter.h * self.filter.w *\
            self.in_dims[0].c // self.groups

    def __str__(self):
        return "F {} S {} D {} G {} M {} P {} {} {}".format(
            self.filter,
            self.stride,
            self.dilation,
            self.groups,
            self.multiplier,
            self.padding,
            self.pad_type,
            self.at_options or ""
        )
