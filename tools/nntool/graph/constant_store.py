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
from scipy.cluster.vq import vq
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from utils.stats_funcs import qsnr

class ConstantStore:
    def __init__(self):
        self._values = {}
        self._faking = False

    def get(self, node, idx, get_compressed=False, get_sparse_val=False):
        node_vals = self._values.get(node)
        if node_vals is None:
            return None
        val = node_vals.get(idx)
        if val is None:
            return None
        if self._faking:
            return np.random.normal(0, 0.2, val[0])
        if get_sparse_val:
            return val[3]
        if get_compressed and val[2] is not None:
            return val[2]
        return val[1]

    def get_sparse_val(self, node, idx):
        return self.get(node, idx, get_compressed=False, get_sparse_val=True)

    def set(self, node, idx, val, val_compressed=None, sparse_val=None):
        node_vals = self._values.get(node)
        if node_vals is None:
            node_vals = {}
            self._values[node] = node_vals

        node_vals[idx] = (val.shape, val, val_compressed, sparse_val)

    def set_shape(self, node, idx, shape):
        node_vals = self._values.get(node)
        if node_vals is None:
            node_vals = {}
            self._values[node] = node_vals

        node_vals[idx] = (shape, None, None, None)

    def compress(self, node, idx, bits=None, min_qsnr=None, sparse=False):
        val = self.get(node, idx)
        flattened_val = val.flatten()
        if bits is not None:
            bins = int(math.pow(2, bits))
            if bins > val.size:
                raise Exception('More bins than values with {} bits'.format(bits))
            kmeans = KMeans(n_clusters=bins)
            kmeans.fit(flattened_val.reshape((-1, 1)))
            codebook = kmeans.cluster_centers_
            codebook = codebook.astype(val.dtype)
            codes = vq(flattened_val.reshape((-1, 1)), codebook)
            compressed_val = np.array([codebook[code] for code in codes[0]]).reshape(val.shape)
        elif min_qsnr:
            cur_qsnr = - math.inf
            bits = 1
            while cur_qsnr < min_qsnr:
                bits += 1
                if bits > 7:
                    raise Exception('Cannot find a solution with less than 8 bits \
                                     for {} with min_qsnr = {}'.format(node.name, min_qsnr))
                bins = int(math.pow(2, bits))
                if bins > val.size:
                    break
                kmeans = KMeans(n_clusters=bins)
                kmeans.fit(flattened_val.reshape((-1, 1)))
                codebook = kmeans.cluster_centers_
                codebook = codebook.astype(val.dtype)
                codes = vq(flattened_val.reshape((-1, 1)), codebook)
                compressed_val = np.array([codebook[code] for code in codes[0]]).reshape(val.shape)
                cur_qsnr = qsnr(compressed_val.astype(np.float32), val.astype(np.float32))
        else:
            # automatic search of optimal k with inertia method
            silhouette = []
            inertia = []
            for bits in range(1, 8):
                bins = int(math.pow(2, bits))
                if bins > val.size:
                    break
                kmeans = KMeans(n_clusters=bins)
                kmeans.fit(flattened_val.reshape((-1, 1)))
                codebook = kmeans.cluster_centers_
                codebook = codebook.astype(val.dtype)
                codes = vq(flattened_val.reshape((-1, 1)), codebook)
                compressed_val = np.array([codebook[code] for code in codes[0]]).reshape(val.shape)
                inertia.append(kmeans.inertia_)
                silhouette.append(silhouette_score(flattened_val.reshape(-1, 1),
                                                   compressed_val.flatten().reshape(-1, 1)))
            elb_idx = np.argmax(np.diff(np.diff(np.array(inertia)))) # 2nd grade derivative to find the elbow
            elb_idx = 1 if elb_idx == 0 else elb_idx
            bits = np.argmax(np.array(silhouette[elb_idx-1:elb_idx+1])) + 1 # take the three around the elbow and look at the silhouette
            bins = int(math.pow(2, bits))
            kmeans = KMeans(n_clusters=bins)
            kmeans.fit(flattened_val.reshape((-1, 1)))
            codebook = kmeans.cluster_centers_
            codebook = codebook.astype(val.dtype)
            codes = vq(flattened_val.reshape((-1, 1)), codebook)
            compressed_val = np.array([codebook[code] for code in codes[0]]).reshape(val.shape)

        if sparse:
            freqs = np.unique(codes, return_counts=True)
            max_index = np.where(freqs[1] == freqs[1].max())[0][0]
            sparse_val = freqs[0][max_index]
        else:
            sparse_val = None
        self.set(node, idx, val, compressed_val, sparse_val)
        x = 1

    @property
    def fake(self):
        return self._faking

    @fake.setter
    def fake(self, val):
        self._faking = val
