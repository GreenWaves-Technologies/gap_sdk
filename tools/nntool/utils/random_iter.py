# Copyright (C) 2022  GreenWaves Technologies, SAS

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

import numpy as np


class RandomIter():
    def __init__(self, count, shapes, ranges, gen=None) -> None:
        self._vars = list(zip(shapes, ranges))
        self._gen = gen
        self._count = count
        self._cur = count
        if self._gen is None:
            self._gen = np.random.default_rng()

    def __iter__(self):
        self._cur = self._count
        return self

    def __next__(self):
        if self._cur <= 0:
            raise StopIteration()
        self._cur -= 1
        return self.val()

    def val(self):
        res = []
        for shape, (minv, maxv) in self._vars:
            res.append((self._gen.random(shape) * (maxv - minv)) + minv)
        return res
