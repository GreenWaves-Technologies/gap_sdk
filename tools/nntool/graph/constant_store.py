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
import math
import numpy as np
from scipy.cluster.vq import vq, whiten
from sklearn.cluster import KMeans, MeanShift, estimate_bandwidth

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

    def compress(self, node, idx, bits, sparse=True):
        val = self.get(node, idx)
        flattened_val = val.flatten()
        bins = int(math.pow(2, bits) + 1 if sparse else 0)
        kmeans = KMeans(n_clusters=bins)
        bandwidth = estimate_bandwidth(flattened_val.reshape((-1, 1)), quantile=0.1)
        meanshift = MeanShift(bandwidth)
        meanshift.fit(flattened_val.reshape((-1, 1)))

        kmeans.fit(flattened_val.reshape((-1, 1)))
        codebook = kmeans.cluster_centers_
        codes = vq(val.flatten(), codebook)
        freqs = np.unique(codes, return_counts=True)
        max_index = np.where(freqs[1] == freqs[1].max())[0][0]
        sparse_val = freqs[0][max_index]
        compressed_val = np.array([codebook[code] for code in codes[0]]).reshape(val.shape)
        x = 1

    @property
    def fake(self):
        return self._faking

    @fake.setter
    def fake(self, val):
        self._faking = val
