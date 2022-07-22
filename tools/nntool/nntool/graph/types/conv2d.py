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


from ..dim import DilationDim, Dim, Dim2D
from .base import (ComparableNodeMixin, FilterLikeNodeBase, FilterNodeBase,
                   MultiplicativeBiasNodeBase, NoSizeChangeNodeBase,
                   SingleInputAndOutputMixin, cls_op_name, SensitiveToOrderMixin, nargs)

LOG = logging.getLogger(__name__)


@cls_op_name('batchnorm')
class BatchNormalizationNode(NoSizeChangeNodeBase, SingleInputAndOutputMixin, SensitiveToOrderMixin):

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name, scale=None, bias=None, running_mean=None, axis=0, in_dims_hint=None, out_dims_hint=None,
                 running_variance=None, spatial=None, momentum=None, epsilon=None, **kwargs):
        if in_dims_hint is None:
            in_dims_hint = [['c', 'h', 'w']]
        if out_dims_hint is None:
            out_dims_hint = [['c', 'h', 'w']]
        super(BatchNormalizationNode, self).__init__(name,
                                                     in_dims_hint=in_dims_hint,
                                                     out_dims_hint=out_dims_hint,
                                                     **kwargs)
        self.scale = scale
        self.bias = bias
        self.running_mean = running_mean
        self.running_variance = running_variance
        self.spatial = spatial
        self.momentum = momentum
        self.epsilon = epsilon
        self.axis = axis

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "scale", "bias", "running_mean", "axis",
            "running_variance", "spatial", "momentum", "epsilon"
        ] if getattr(self, k) is not None})
        return super_attrs

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def __str__(self):
        return f"{self.at_options or ''}"


@cls_op_name('conv2d')
@nargs({2, 3})
class Conv2DNode(FilterLikeNodeBase, MultiplicativeBiasNodeBase, ComparableNodeMixin, SensitiveToOrderMixin):

    #pylint: disable-msg=too-many-arguments
    def __init__(self, name, dilation=None, groups=None, multiplier=1,
                 in_dims_hint=None, out_dims_hint=None, batch=None, ker_in_order=None, ker_out_order=None,
                 custom=None, tf_depthwise=False, has_bias=True, **kwargs):

        if ker_in_order is None:
            ker_in_order = [
                ['c', 'h', 'w'],
                ['out_c', 'in_c', 'h', 'w'],
                ['out_c']]
        if ker_out_order is None:
            ker_out_order = [['c', 'h', 'w']]

        if in_dims_hint is None:
            in_dims_hint = [['c', 'h', 'w'], [
                'out_c', 'in_c', 'h', 'w'], ['out_c']]
        if out_dims_hint is None:
            out_dims_hint = [['c', 'h', 'w']]
        super(Conv2DNode, self).__init__(name, has_bias=True,
                                         in_dims_hint=in_dims_hint, out_dims_hint=out_dims_hint,
                                         ker_in_order=ker_in_order, ker_out_order=ker_out_order,
                                         **kwargs)

        if dilation is None:
            dilation = DilationDim(1)

        self.batch = batch
        self.dilation = dilation
        self.tf_depthwise = tf_depthwise

        # if groups is not set then this is a normal convolution
        # a depthwise convolution would have groups = filter.out_c and
        # filter.in_c == filter.out_c
        # i.e. there are as many groups as output and input channels
        # The multiplier parameter correspond to the channel multiplier option
        # in tensorflow. There are multiplier filters per input channel in
        # a depthwise convolution
        if groups is None or groups == 0:
            self.groups = 1
            self.cannot_be_dw = True
        else:
            self.groups = groups
            self.cannot_be_dw = False

        self.multiplier = multiplier
        assert self.multiplier == 1 or self.tf_depthwise,\
            "multiplier should only be set in TF depthwise convolution"

        LOG.debug("created CON2D %s %s", self.name, str(self))

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if self.dilation != 1:
            super_attrs['dilation'] = self.dilation
        if self.groups != 1:
            super_attrs['groups'] = self.groups
        super_attrs.update({k: getattr(self, k) for k in [
            "batch", "multiplier", "tf_depthwise",
        ] if getattr(self, k)})
        return super_attrs

    @property
    def graph_label(self):
        label = [
            self.name,
            f'cur {"".join(self._in_dims_hint[0])} ker {"".join(self._ker_in_order[0])}',
            f'Filter {self.filter_dim}']
        if self.stride.size() != 1:
            label.append(f'Stride {self.stride}')
        if self.padding.has_padding:
            label.append(f'Pad {self.padding}')
        if self.dilation.size() != 1:
            label.append(f'Dilate {self.dilation}')
        return label

    def remove_batch(self):
        self.batch = None
        self.in_dims_hint[0] = self.in_dims_hint[0][1:]
        self.out_dims_hint[0] = self.out_dims_hint[0][1:]
        self.ker_in_order[0] = self.ker_in_order[0][1:]
        self.ker_out_order[0] = self.ker_out_order[0][1:]


    def can_be_grouped_with(self, G, other):
        if not isinstance(other, Conv2DNode) or self.is_grouped_conv() or other.is_grouped_conv():
            return False
        if G.quantization:
            qrec = G.quantization.get(self.name)
            oqrec = G.quantization.get(other.name)
            if not qrec or not oqrec:
                return False
            # If one kernel uses NE16 so must the other
            if bool(qrec.cache.get('use_ne16')) != bool(oqrec.cache.get('use_ne16')):
                return False
            wqtype, owqtype = qrec.in_qs[1], oqrec.in_qs[1]
            # If any kernel is floating or pow2
            if any(getattr(qtype, k) for qtype in [wqtype, owqtype]
                   for k in ['is_floating', 'is_pow2']):
                # Then they all must match floating or pow2
                if not all(getattr(wqtype, k) == getattr(owqtype, k)
                           for k in ['is_floating', 'is_pow2']):
                    return False
            elif wqtype.bits != owqtype.bits:  # SQ8 case - weight bits must match
                return False
        filter_compatible = all(getattr(self.filter_dim, k) == getattr(other.filter_dim, k) for k in ["h", "w", "in_c"])
        return (filter_compatible and self.padding == other.padding and
                self.stride == other.stride and self.dilation == other.dilation and
                all(d1.shape == d2.shape for d1, d2 in zip(self.in_dims, other.in_dims)) and
                self.out_dims[0].shape == other.out_dims[0].shape)

    def is_grouped_conv(self):
        return self.groups > 1

    def is_depthwise_conv(self):
        # this does not cope with TFLITE DW convs with a multiplier but the
        # generator is going to need to split those up into multiple per channel
        # normal convolutions
        return (self.groups == self.filter_dim.out_c and self.filter_dim.in_c == 1 and
                self.multiplier == 1 and not self.cannot_be_dw)

    def get_weights_count(self):
        return self.filter_dim.size() * self.multiplier // self.groups

    def get_bias_count(self):
        return self.multiplier * self.filter_dim.out_c

    def get_parameter_size(self):
        return 0

    @property
    def at_options(self):
        return self._at_options

    def get_output_size(self, in_dims):
        in_dim = in_dims[0].clone()
        if self.batch is not None:
            assert self.batch == in_dim.remove_axis('n')

        assert in_dim.c >= self.groups,\
            "The number of groups cannot be larger than the amount of input channels"
        self.filter_dim.in_c = in_dim.c // self.groups
        if self.padding.is_same:
            self.padding.calculate_same(
                in_dim, self.filter_dim, self.stride, dilation=self.dilation)
        filter_d = self.filter_dim + (self.filter_dim - 1) * (self.dilation - 1)

        pad = self.padding.height_width()

        h = ((in_dim.h - filter_d.h + pad.h)//self.stride.h) + 1
        w = ((in_dim.w - filter_d.w + pad.w)//self.stride.w) + 1
        if h < 0 or w < 0:
            raise ValueError(
                f'{self.name}dimension calculation invalid {h}, {w}')
        out_dim = Dim.named_ordered(h=h, w=w, c=self.filter_dim.out_c)
        out_dim.impose_order(in_dim.order)
        if self.batch is not None:
            out_dim.insert_axis(0, new_name='n')
            out_dim.n = self.batch

        return [out_dim]

    def compute_load(self):
        return self.out_dims[0].size() * self.filter_dim.h * self.filter_dim.w *\
            self.in_dims[0].c // self.groups

    def __str__(self):
        return "F {} S {} D {} G {} M {} P {} {} {}".format(
            self.filter_dim,
            self.stride,
            self.dilation,
            self.groups,
            self.multiplier,
            self.padding,
            self.pad_type,
            self.at_options or ""
        )


@cls_op_name('transpose_conv2d')
@nargs({2, 3})
class TransposeConv2DParameters(FilterNodeBase, FilterLikeNodeBase, SensitiveToOrderMixin):

    def __init__(self, name, output_padding=None, dilation=None, groups=1, has_bias=False, **kwargs):
        super(TransposeConv2DParameters, self).__init__(
            name, has_bias=has_bias, **kwargs)
        self.output_padding = Dim2D(
            0, 0) if output_padding is None else output_padding
        self.dilation = Dim2D(1, 1) if dilation is None else dilation
        self.groups = groups

    def get_weights_count(self):
        return self.filter_dim.size()

    def get_bias_count(self):
        return 0

    def get_parameter_size(self):
        if self.filter_dim.in_c is None:
            return 0
        return self.get_weights_count() + self.get_bias_count()

    def get_output_size(self, in_dims):

        in_dims = in_dims[0]

        pad = self.padding.height_width()

        out_dim = (in_dims - 1) * self.stride - pad + \
            self.output_padding + ((self.filter_dim - 1) * self.dilation + 1)
        out_dim.c = self.filter_dim.out_c * self.groups
        out_dim.impose_order(in_dims.order)
        return [out_dim]

    def compute_load(self):
        return self.out_dims[0].c * self.filter_dim.h * self.filter_dim.w * self.in_dims[0].size()

    def __str__(self):
        return f"F {self.filter_dim} S {self.stride} P {self.padding} D {self.dilation} {self.pad_type} {self.at_options or ''}"
