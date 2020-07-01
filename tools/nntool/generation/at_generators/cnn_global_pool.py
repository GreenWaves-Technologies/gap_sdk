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
from .utils import at_bits

GEN_GLOBALPOOL = "CNN_GlobalPool"
# /** \brief CNN_GlobalPool
#  *  Generator for Global Pooling (Max or Average)
#  *

#     \param    Name:           Name of the generated user kernel

#     \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

#     \param    In_DataSize:    1: byte, 2: half word, 4: word
#     \param    Out_DataSize:   1: byte, 2: half word, 4: word

#     \param    In_Q:           In fixed point format
#     \param    Out_Q:          Out fixed point format

#     \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
#     \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

#     \param    InFeat:         Number of input feature's maps
#     \param    OutFeat:        Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
#     \param    Width:          Number of columns of a given feature map
#     \param    Height:         Number of lines of a given feature map

#     \param    PoolOper:       KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL

GlobalPoolATParam = namedtuple('GlobalPoolATParam', [
    "GlobalPoolOper"
])

def gen_globalpool_at_params(params):
    return GlobalPoolATParam(
        GlobalPoolOper="KOP_GLOBAL_AVGPOOL" if params.pool_type == "average" else "KOP_GLOBAL_MAXPOOL"
    )

def gen_at_globalpool(code_block, name, in_q, out_q,
                      in_dim, out_dim, at_globalpool, gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        gen_ctrl = gen_ctrl.ctrl_name
        #raise NotImplementedError("genctrl is not yet implemented")

    if at_ver < 3:
        code_block.write('{}("{}", {}, {}, {}, 1, 1, {}, {}, {}, {}, {});',
                         GEN_GLOBALPOOL, name, gen_ctrl,
                         at_bits(in_q), at_bits(out_q), in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_globalpool.GlobalPoolOper)
    else:
        code_block.write('{}("{}", {}, {}, {}, {}, {}, 1, 1, {}, {}, {}, {}, {});',
                         GEN_GLOBALPOOL, name, gen_ctrl,
                         at_bits(in_q), at_bits(
                             out_q), in_q.q, out_q.q, in_dim.shape[0], out_dim.shape[0],
                         in_dim.shape[1], in_dim.shape[2], at_globalpool.GlobalPoolOper)
