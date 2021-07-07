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
from .base import (ComparableParameters, FilterLikeParameters, FilterParameters,
                   MultiplicativeBiasParameters, NoSizeChangeParameters,
                   SingleInputAndOutput, Transposable, cls_op_name)

LOG = logging.getLogger("nntool." + __name__)


@cls_op_name('batchnorm')
class BatchNormalizationParameters(NoSizeChangeParameters, SingleInputAndOutput, Transposable):

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name, scale=None, bias=None, running_mean=None,
                 running_variance=None, spatial=None, momentum=None, epsilon=None, **kwargs):
        super(BatchNormalizationParameters, self).__init__(name, **kwargs)
        self.scale = scale
        self.bias = bias
        self.running_mean = running_mean
        self.running_variance = running_variance
        self.spatial = spatial
        self.momentum = momentum
        self.epsilon = epsilon

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def __str__(self):
        return "{} {}".format(
            Transposable.__str__(self),
            self.at_options or ""
        )


@cls_op_name('conv2d')
class Conv2DParameters(FilterLikeParameters, MultiplicativeBiasParameters, Transposable, ComparableParameters):

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name, dilation=None, groups=None, multiplier=1,
                 custom=None, tf_depthwise=False, has_bias=True, **kwargs):

        super(Conv2DParameters, self).__init__(name, has_bias=True, **kwargs)

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
            self.cannot_be_dw = True
        else:
            self.groups = groups
            self.cannot_be_dw = False

        self.multiplier = multiplier
        assert self.multiplier == 1 or self.tf_depthwise,\
            "multiplier should only be set in TF depthwise convolution"

        if custom is None:
            self.custom = {}
        else:
            self.custom = dict(custom)

        self._ker_in_order = [['c', 'h', 'w'], [
            'out_c', 'in_c', 'h', 'w'], ['out_c']]
        self._ker_out_order = [['c', 'h', 'w']]

        LOG.debug("created CON2D %s", str(self))

    @property
    def graph_label(self):
        label = [self.name, f'Filter {self.filter}']
        if self.stride.size() != 1:
            label.append(f'Stride {self.stride}')
        if self.padding.has_padding:
            label.append(f'Pad {self.padding}')
        if self.dilation.size() != 1:
            label.append(f'Dilate {self.dilation}')
        return label

    def can_be_grouped_with(self, other):
        if not isinstance(other, Conv2DParameters) or self.is_grouped_conv() or other.is_grouped_conv():
            return False
        return (self.filter == other.filter and self.padding == other.padding and
                self.stride == other.stride and self.dilation == other.dilation and
                all(d1.shape == d2.shape for d1, d2 in zip(self.in_dims, other.in_dims)) and
                self.out_dims[0].shape == other.out_dims[0].shape)

    def is_grouped_conv(self):
        return self.groups > 1

    def is_depthwise_conv(self):
        # this does not cope with TFLITE DW convs with a multiplier but the
        # generator is going to need to split those up into multiple per channel
        # normal convolutions
        return (self.groups == self.filter.out_c and self.filter.in_c == 1 and
                self.multiplier == 1 and not self.cannot_be_dw)

    def get_weights_count(self):
        return self.filter.size() * self.multiplier // self.groups

    def get_bias_count(self):
        return self.multiplier * self.filter.out_c

    def get_parameter_size(self):
        return 0

    @property
    def at_options(self):
        return self._at_options

    def get_output_size(self, in_dims):
        in_dim = in_dims[0]
        if self.transpose_in and self.transpose_in[0]:
            in_dim = in_dim.calc_transpose(self.transpose_in[0])

        assert in_dim.c >= self.groups,\
            "The number of groups cannot be larger than the amount of input channels"
        self.filter.in_c = in_dim.c // self.groups
        if self.padding.is_same:
            self.padding.calculate_same(
                in_dim, self.filter, self.stride, dilation=self.dilation)
        filter_d = self.filter + (self.filter - 1) * (self.dilation - 1)

        pad = self.padding.height_width()

        out_dim = ((in_dim - filter_d + pad)//self.stride) + 1
        out_dim.c = self.filter.out_c
        out_dim.impose_order(in_dim.order)
        if self.transpose_out and self.transpose_out[0]:
            out_dim = out_dim.calc_transpose(self.transpose_out[0])
        return [out_dim]

    def compute_load(self):
        return self.out_dims[0].size() * self.filter.h * self.filter.w *\
            self.in_dims[0].c // self.groups

    def __str__(self):
        return "F {} S {} D {} G {} M {} P {} {} {} {}".format(
            self.filter,
            self.stride,
            self.dilation,
            self.groups,
            self.multiplier,
            self.padding,
            self.pad_type,
            Transposable.__str__(self),
            self.at_options or ""
        )


@cls_op_name('transpose_conv2d')
class TransposeConv2DParameters(FilterParameters, FilterLikeParameters, Transposable):

    def __init__(self, name, **kwargs):
        super(TransposeConv2DParameters, self).__init__(
            name, has_bias=False, **kwargs)

    def get_weights_count(self):
        return self.filter.size()

    def get_bias_count(self):
        return 0

    def get_parameter_size(self):
        if self.filter.in_c is None:
            return 0
        return self.get_weights_count() + self.get_bias_count()

    def get_output_size(self, in_dims):

        in_dims = in_dims[0]

        if self.transpose_in and self.transpose_in[0]:
            in_dims = in_dims.calc_transpose(self.transpose_in[0])

        pad = self.padding.height_width()

        out_dim = (in_dims - 1) * self.stride - pad + self.filter
        out_dim.c = self.filter.out_c
        out_dim.impose_order(in_dims.order)
        if self.transpose_out and self.transpose_out[0]:
            out_dim = out_dim.calc_transpose(self.transpose_out[0])
        return [out_dim]

    def compute_load(self):
        return self.out_dims[0].c * self.filter.h * self.filter.w * self.in_dims[0].size()

    def __str__(self):
        return "F {} S {} P {} {} {} {}".format(
            self.filter,
            self.stride,
            self.padding,
            self.pad_type,
            Transposable.__str__(self),
            self.at_options or ""
        )
