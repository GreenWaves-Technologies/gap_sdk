# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from collections import namedtuple

# ConvOper:       Type of convolution, Regular convolution: KOP_CONV,
#                 Regular convolution with double precision output: KOP_CONV_DP,
#                 Depth wise convolution: KOP_CONV_DW
# Fcx:            Convolution filter x dimension
# Fcy:            Convolution filter y dimension
# Dcx:            Convolution filter dilation factor, x dimension
# Dcy:            Convolution filter dilation factor, y dimension
# Scx:            Convolution filter stride x dimension
# Scy:            Convolution filter stride y dimension
# ConvPad:        0: No padding, 1: Zero padding

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

# PoolOper:       Type of Pooling, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
# Fpx:            Pooling filter x dimension
# Fpy:            Pooling filter y dimension
# Dpx:            Pooling filter dilation factor, x dimension
# Dpy:            Pooling filter dilation factor, y dimension
# Spx:            Pooling filter stride x dimension
# Spy:            Pooling filter stride y dimension
# PoolPad:        0: No padding, 1: Zero padding

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

ActivationATParam = namedtuple('ActivationATParam', [
    "ReLUOper"
])

NO_ACTIVATION = ActivationATParam(ReLUOper='KOP_NONE')

LinearATParam = namedtuple('LinearATParam', [
    "LinearOper"
])

SoftMaxATParam = namedtuple('SoftMaxATParam', [
    "SoftMaxOper"
])
