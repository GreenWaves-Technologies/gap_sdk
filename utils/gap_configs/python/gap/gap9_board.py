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

import gsystree as st
from gap.gap9.gapmod import Gapmod


class Gap9_board(st.Component):

    def __init__(self, parent, name):
        super(Gap9_board, self).__init__(parent, name)

        board = Gapmod(self, 'board')


class Gap9_dualboard(st.Component):

    def __init__(self, parent, name):
        super(Gap9_dualboard, self).__init__(parent, name)

        board = Gapmod(self, 'board')
        board = Gapmod(self, 'board2')
