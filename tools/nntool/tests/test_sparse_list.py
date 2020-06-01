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
    sparse_list = SparseList()
    sparse_list[2] = True
    assert sparse_list[1] is None
    assert len(sparse_list) == 3
    assert sparse_list[2]
    sparse_list[5] = False
    assert len(sparse_list) == 6
    assert not sparse_list[5]
    del sparse_list[2]
    assert len(sparse_list) == 5
    assert sparse_list[2] is None
    assert not sparse_list[4]
    iter_sparse_list = [v for v in sparse_list]
    assert iter_sparse_list == [None, None, None, None, False]
