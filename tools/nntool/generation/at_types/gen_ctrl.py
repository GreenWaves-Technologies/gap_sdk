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

from utils.option_list import OptionList

# int TileOrientation;	/* Set Tiling orientation TILE_HOR TILE_VER */
# int ParallelFeatures;	/* Parallelize along channels */
# int ForceDPconv;	/* Forces double precision convolution*/
# int UseHwCE;		/* Enable HW CE */
# AT_PadType PadType;	/* Control padding strategy */
# int EnableIm2Col;	/* Enable mat mul based convolution when feasible */
# int ReluN;		/* if != -1 Overides 6 as a default value for ReLUN */
# int MulBiasScalar;	/* if != -1 Overides default non scalar for MulBias convolutions */


def gen_ctrl_call(api, op, val, code_block):
    if isinstance(val, str):
        val = 'AT_OPT_VAL("%s")' % val
    elif isinstance(val, bool):
        val = val and 'AT_OPT_ON' or 'AT_OPT_OFF'
    elif isinstance(val, int):
        val = 'AT_OPT_VAL(%s)' % val
    else:
        raise ValueError()

    code_block.write('{}({}, {});', api, op, val)


def gen_kernel_ctrl(op, val, code_block):
    gen_ctrl_call('AT_SetKernelCtrl', op, val, code_block)


def gen_graph_ctrl(op, val, code_block):
    gen_ctrl_call('AT_SetGraphCtrl', op, val, code_block)

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
                code_block.write('CNN_SetGenCtrl({}, "{}", AT_OPT_VAL({}));',
                                 self.ctrl_name, name.upper(), val)
            else:
                raise NotImplementedError()
