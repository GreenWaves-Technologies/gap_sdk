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

from utils.sparse_list import SparseList

def test1():
    sl = SparseList()
    sl[2] = True
    assert sl[1] is None
    assert len(sl) == 3
    assert sl[2] == True
    sl[5] = False
    assert len(sl) == 6
    assert sl[5] == False
    del sl[2]
    assert len(sl) == 5
    assert sl[2] is None
    assert sl[4] == False
    tl = [v for v in sl]
    assert tl == [None, None, None, None, False]
