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
#
# author: martin.croome@greenwaves-technologies.com

import numpy as np

class ConstantStore:
    def __init__(self):
        self._values = {}
        self._faking = False

    def get(self, node, idx):
        node_vals = self._values.get(node)
        if node_vals is None:
            return None
        val = node_vals.get(idx)
        if val is None:
            return None
        if self._faking:
            return np.random.normal(0, 0.2, val[0])
        return val[1]

    def set(self, node, idx, val):
        node_vals = self._values.get(node)
        if node_vals is None:
            node_vals = {}
            self._values[node] = node_vals

        node_vals[idx] = (val.shape, val)

    def set_shape(self, node, idx, shape):
        node_vals = self._values.get(node)
        if node_vals is None:
            node_vals = {}
            self._values[node] = node_vals

        node_vals[idx] = (shape, None)

    @property
    def fake(self):
        return self._faking

    @fake.setter
    def fake(self, val):
        self._faking = val
