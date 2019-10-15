# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging

from ..dim import DilationDim
from .base import FilterLikeParameters, FilterParameters

LOG = logging.getLogger("nntool." + __name__)

class Conv2DParameters(FilterLikeParameters, FilterParameters):

    op_name = "conv2d"

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name, filt=None, stride=None, padding=None, dilation=None,
                 pad_type="zero", groups=None, multiplier=1,
                 has_bias=False, custom=None, tf_depthwise=False,
                 in_dims_hint=None, out_dims_hint=None):

        super(Conv2DParameters, self).__init__(name, stride=stride, padding=padding,
                                               pad_type=pad_type, filt=filt, has_bias=has_bias)

        self.in_dims_hint = in_dims_hint
        self.out_dims_hint = out_dims_hint

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

    def clone(self, groupn=None):
        """ Clones conv parameters for a specific group number
        """
        if groupn is None:
            new_conv = Conv2DParameters(self.filter, self.stride, self.padding,\
                self.dilation, self.pad_type, self.groups, self.multiplier, self.has_bias)
        else:
            assert groupn < self.groups, "incorrect group number"
            c_per_g = self.filter.out_c//self.groups
            offset = c_per_g * groupn
            new_filter = self.filter.clone()
            new_filter.out_c = c_per_g
            new_conv = Conv2DParameters(new_filter, self.stride, self.padding,\
                self.dilation, self.pad_type, 1, self.multiplier, self.has_bias)
            num_out = c_per_g * self.multiplier
            if self.biases is not None:
                new_conv.biases = self.biases[offset:offset + num_out - 1]

            if self.weights is not None:
                # TODO - THIS IS NOT CORRECT. CHECK ORDER.
                new_conv.weights = self.weights[offset:offset + num_out - 1, ...]

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

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1,\
            "Only expecting one input dimension"
        self.in_dims = in_dims
        in_dims = in_dims[0]

        assert in_dims.c >= self.groups,\
            "The number of groups cannot be larger than the amount of input channels"
        self.filter.in_c = in_dims.c // self.groups
        if self.padding.is_same:
            self.padding.calculate_same(in_dims, self.filter, self.stride)
        filter_d = self.filter + (self.filter - 1) * (self.dilation - 1)

        pad = self.padding.height_width()

        out_dim = ((in_dims - filter_d + pad)//self.stride) + 1
        out_dim.c = self.filter.out_c
        out_dim.impose_order(in_dims.order)
        return [out_dim]

    def __str__(self):
        return "F {} S {} D {} G {} M {} P {} {}".format(
            self.filter,
            self.stride,
            self.dilation,
            self.groups,
            self.multiplier,
            self.padding,
            self.pad_type
        )
