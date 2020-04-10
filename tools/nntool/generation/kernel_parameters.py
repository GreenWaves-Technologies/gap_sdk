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

from utils.option_list import OptionList

# int TileOrientation;	/* Set Tiling orientation TILE_HOR TILE_VER */
# int ParallelFeatures;	/* Parallelize along channels */
# int ForceDPconv;	/* Forces double precision convolution*/
# int UseHwCE;		/* Enable HW CE */
# AT_PadType PadType;	/* Control padding strategy */
# int EnableIm2Col;	/* Enable mat mul based convolution when feasible */
# int ReluN;		/* if != -1 Overides 6 as a default value for ReLUN */
# int MulBiasScalar;	/* if != -1 Overides default non scalar for MulBias convolutions */



CTRL_FEATURES = {
    "TILEORIENTATION": int,
    "PARALLELFEATURES": int,
    "FORCEDPCONV": int,
    "USEHWCE": int,
    "PADTYPE": int,
    "ENABLEIM2COL": int,
    "RELUN": int,
    "MULBIASSCALAR": int,
    "RELUNNONORM": int
}


class GenCtrl(OptionList):
    PREFIX = "gen_ctrl_"

    def __init__(self, options, *args, cname=None, **kwargs):
        super(GenCtrl, self).__init__(*args, valid_options=CTRL_FEATURES, **kwargs)
        if options is not None:
            self.extend(options, name_filter=lambda name: name in CTRL_FEATURES)
        self._cname = cname

    @property
    def is_unmodified(self):
        return len(self) == 0

    @property
    def set_features(self):
        return self.set_options

    @property
    def prefixed_cname(self):
        return self.PREFIX + self._cname

    @property
    def ctrl_name(self):
        if self.is_unmodified:
            return "0"

        return "&{}".format(self.prefixed_cname)

    @property
    def cname(self):
        return self._cname

    @cname.setter
    def cname(self, val):
        self._cname = val

    def gen_ctrl_decl(self, code_block):
        code_block.write('CNN_GenControl_T {};', self.prefixed_cname)
        code_block.write('CNN_InitGenCtrl({});', self.ctrl_name)
        for name, val in self._options.items():
            if self.valid_options[name] == int:
                code_block.write('CNN_SetGenCtrl({}, "{}", (void *){});',
                                 self.ctrl_name, name.upper(), val)
            else:
                raise NotImplementedError()


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

MatrixAddATParam = namedtuple('MatrixAddATParam', [
    "MatrixAddOper"
])

GlobalPoolATParam = namedtuple('GlobalPoolATParam', [
    "GlobalPoolOper"
])

ThreeDTensorTransposeATParam = namedtuple('ThreeDTensorTransposeATParam', [
    'MatPermOper'
])

MatScaleATParam = namedtuple('MatScaleATParam', [
    "ScaleOper",
    "ReLUOper"
])
