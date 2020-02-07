# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
