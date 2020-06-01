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

from collections import namedtuple

# ACTIVATION
ActivationATParam = namedtuple('ActivationATParam', [
    "ReLUOper"
])

NO_ACTIVATION = ActivationATParam(ReLUOper='KOP_NONE')

def gen_activation_op(activation, force_relu=False):
    if activation is None or activation == "none":
        aop = "KOP_NONE"
    elif activation == "relu":
        aop = "KOP_RELU"
    elif activation == "relu6":
        aop = "KOP_RELUN" if not force_relu else "KOP_RELU"
    elif activation == "relun":
        aop = "KOP_RELUN" if not force_relu else "KOP_RELU"
    elif activation == "sigmoid" or activation == "hsigmoid":
        aop = "KOP_HSIGMOID"
    elif activation == "swish" or activation == "hswish":
        aop = "KOP_HSWISH"
    else:
        raise NotImplementedError("activation type %s not implemented" % activation)
    return aop

def gen_active_at_params(params, force_relu=False):
    return ActivationATParam(
        ReLUOper=gen_activation_op(params.activation, force_relu=force_relu)
    )

# CONV

ConvATParam = namedtuple('ConvATParam', [
    "ConvOper",
    "Fcx",
    "Fcy",
    "Dcx",
    "Dcy",
    "Scx",
    "Scy",
    "ConvPad"
])

NO_CONV = ConvATParam(ConvOper='KOP_NONE', Fcx=0, Fcy=0, Dcx=0, Dcy=0, Scx=0, Scy=0, ConvPad=0)

# GROUPED CONV
# ConvOper:       Type of convolution, Regular convolution: KOP_CONV,
#                 Regular convolution with double precision output: KOP_CONV_DP,
#                 Depth wise convolution: KOP_CONV_DW
# GroupIn:        Size of the group for input features
# GroupOut:       Size of the group for output features
# Fcx:            Convolution filter x dimension
# Fcy:            Convolution filter y dimension
# Dcx:            Convolution filter dilation factor, x dimension
# Dcy:            Convolution filter dilation factor, y dimension
# Scx:            Convolution filter stride x dimension
# Scy:            Convolution filter stride y dimension
# ConvPad:        0: No padding, 1: Zero padding

GroupedConvATParam = namedtuple('GroupedConvATParam', [
    "ConvOper",
    "GroupIn",
    "GroupOut",
    "Fcx",
    "Fcy",
    "Dcx",
    "Dcy",
    "Scx",
    "Scy",
    "ConvPad"
])

def gen_conv_at_params(params, pad_compatibilities):
    if params.is_depthwise_conv():
        assert params.multiplier == 1, "Multiplier not supported"
        cop = "KOP_CONV_DW"
    elif params.is_grouped_conv():
        cop = "KOP_CONV"
        return GroupedConvATParam(
            ConvOper=cop,
            GroupIn=params.groups,
            GroupOut=params.multiplier,
            Fcx=params.filter.w,
            Fcy=params.filter.h,
            Dcx=params.dilation.w,
            Dcy=params.dilation.h,
            Scx=params.stride.w,
            Scy=params.stride.h,
            ConvPad=params.has_at_zero_pad() and 1 or 0
        )
    else:
        cop = "KOP_CONV"

    pad_compatibilities.append(params.padding.pad_compatibility)
    return ConvATParam(
        ConvOper=cop,
        Fcx=params.filter.w,
        Fcy=params.filter.h,
        Dcx=params.dilation.w,
        Dcy=params.dilation.h,
        Scx=params.stride.w,
        Scy=params.stride.h,
        ConvPad=params.has_at_zero_pad() and 1 or 0
    )

# POOL
PoolATParam = namedtuple('PoolATParam', [
    "PoolOper",
    "Fpx",
    "Fpy",
    "Dpx",
    "Dpy",
    "Spx",
    "Spy",
    "PoolPad"
])

NO_POOL = PoolATParam(PoolOper='KOP_NONE', Fpx=0, Fpy=0, Dpx=0, Dpy=0, Spx=0, Spy=0, PoolPad=0)

def gen_pool_at_params(params, pad_compatibilities):
    if params.pool_type == "average":
        pop = "KOP_AVGPOOL"
    elif params.pool_type == "max":
        pop = "KOP_MAXPOOL"
    else:
        raise NotImplementedError()

    pad_compatibilities.append(params.padding.pad_compatibility)
    return PoolATParam(
        PoolOper=pop,
        Fpx=params.filter.w,
        Fpy=params.filter.h,
        Dpx=1,
        Dpy=1,
        Spx=params.stride.w,
        Spy=params.stride.h,
        PoolPad=params.has_at_zero_pad() and 1 or 0
    )

GlobalPoolATParam = namedtuple('GlobalPoolATParam', [
    "GlobalPoolOper"
])

def gen_globalpool_at_params(params):
    return GlobalPoolATParam(
        GlobalPoolOper="KOP_GLOBAL_AVGPOOL" if params.pool_type == "average" else "KOP_GLOBAL_MAXPOOL"
    )

# LINEAR
LinearATParam = namedtuple('LinearATParam', [
    "LinearOper"
])

def gen_linear_at_params(_):
    return LinearATParam(
        LinearOper="KOP_LINEAR"
    )

# MATRIX ADD
MatrixAddATParam = namedtuple('MatrixAddATParam', [
    "MatrixAddOper"
])

def gen_matrixadd_at_params(_):
    return MatrixAddATParam(
        MatrixAddOper="KOP_MATADD"
    )

# MATRIX SCALE
MatScaleATParam = namedtuple('MatScaleATParam', [
    "ScaleOper",
    "ReLUOper"
])

def gen_matscale_at_params(params):
    # KOP_MATSCALE_VECTOR, KOP_MATSCALE_SCALAR or KOP_MATSCALE_VECTOR_SCALAR
    if params.fusion_type == "vec_scalar":
        ms_op = 'KOP_MATSCALE_VECTOR_SCALAR'
    elif params.fusion_type == "vector":
        ms_op = 'KOP_MATSCALE_VECTOR'
    elif params.fusion_type == "scalar":
        ms_op = 'KOP_MATSCALE_SCALAR'
    else:
        raise NotImplementedError("unknown fusion type %s" % params.fusion_type)
    return MatScaleATParam(
        ScaleOper=ms_op,
        ReLUOper=gen_activation_op(params.activation)
    )

# SOFTMAX
SoftMaxATParam = namedtuple('SoftMaxATParam', [
    "SoftMaxOper"
])

def gen_softmax_at_params(_):
    return SoftMaxATParam(
        SoftMaxOper="KOP_SOFTMAX"
    )
