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

import logging
import math
from collections import namedtuple

import numpy as np
from scipy.cluster.vq import vq
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from utils.stats_funcs import qsnr

LOG = logging.getLogger("nntool." + __name__)

CompressedVal = namedtuple(
    'CompressedVal', ['compressed_val', 'bits', 'codebook', 'size', 'sparse'])


class ConstantStore:
    def __init__(self):
        self._values = {}
        self._faking = False

    def get(self, node, idx, compressed=False):
        node_vals = self._values.get(node)
        if node_vals is None:
            return None
        val = node_vals.get(idx)
        if val is None:
            return None
        if self._faking:
            return np.random.normal(0, 0.2, val[0])
        if compressed and val[2] is not None:
            return val[2].compressed_val
        return val[1]

    def get_codes(self, node, idx):
        node_vals = self._values.get(node)
        if node_vals is None:
            return None
        val = node_vals.get(idx)
        if val is None:
            return None
        if val[2] is None:
            return None
        codes = vq(val[1].flatten().reshape((-1, 1)), val[2].codebook)
        return codes[0], val[2].codebook, val[2].bits

    def get_compressed_size(self, node, idx):
        node_vals = self._values.get(node)
        if node_vals is None:
            return 0
        val = node_vals.get(idx)
        if val is None:
            return 0
        if val[2] is None:
            return 0
        return val[2].size

    def set(self, node, idx, val, compressed_val=None):
        node_vals = self._values.get(node)
        if node_vals is None:
            node_vals = {}
            self._values[node] = node_vals

        node_vals[idx] = (val.shape, val,  compressed_val)

    def set_shape(self, node, idx, shape):
        node_vals = self._values.get(node)
        if node_vals is None:
            node_vals = {}
            self._values[node] = node_vals

        node_vals[idx] = (shape, None, None, None)

    def compress(self, node, idx, bits=None, min_qsnr=None, force_sparse=False):
        val = self.get(node, idx)
        flattened_val = val.flatten()
        codes = None
        if val.size <= 4:
            LOG.warning('value in node %s is too small to compress', node.name)
            return None
        if bits is not None:
            bins = int(math.pow(2, bits))
            if bins > val.size:
                bits = max(int(math.floor(math.log2(val.size))), 2)
                bins = int(math.pow(2, bits))
                LOG.warning('more bins than values for node %s - reducing to %s bits', node.name, bits)
            compressed_val, codes, codebook = self.cluster(bins, flattened_val, val)
        elif min_qsnr:
            cur_qsnr = -math.inf
            bits = 1
            while cur_qsnr < min_qsnr:
                bits += 1
                if bits > 8:
                    LOG.warning('value in node %s cannot meet %s QSNR at 8 bits or under - not compressing', node.name, min_qsnr)
                    return None
                bins = int(math.pow(2, bits))
                if bins > val.size:
                    LOG.warning('value in node %s cannot be reduced in size - not compressing', node.name)
                    return None
                compressed_val, codes, codebook = self.cluster(bins, flattened_val, val)
                cur_qsnr = qsnr(compressed_val.astype(
                    np.float32), val.astype(np.float32))
        else:
            # automatic search of optimal k with inertia method
            silhouette = []
            inertia = []
            for bits in range(2, 9):
                bins = int(math.pow(2, bits))
                if bins > val.size - 1:
                    break
                compressed_val, codes, codebook = self.cluster(bins, flattened_val, val, inertia=inertia)
                silhouette.append(silhouette_score(flattened_val.reshape(-1, 1),
                                                   compressed_val.flatten()))
            if len(inertia) <= 1:
                compressed_val, codes, codebook = self.encode_shorter(flattened_val, val)
            else:
                # 2nd grade derivative to find the elbow
                if len(inertia) > 2:
                    cinertia = np.array(inertia)
                    # cinertia[cinertia>1] = 1
                    elb_idx = np.argmax(np.diff(np.diff(cinertia))) + 1
                else:
                    elb_idx = 1
                # take the three around the elbow and look at the silhouette
                bits = np.argmax(np.array(silhouette[elb_idx-1:elb_idx+1])) + elb_idx + 1
                bins = int(math.pow(2, bits))
                compressed_val, codes, codebook = self.cluster(bins, flattened_val, val)
        # see if sparse representation is better
        # TODO - this is not entirely correct since it is not accounting for the extra bin created by the sparse value
        freqs = np.unique(codes, return_counts=True)
        max_index = np.where(freqs[1] == freqs[1].max())[0][0]
        sparse_freq = freqs[1][max_index]
        sparse_size = math.ceil((codes.size - sparse_freq) * (bits + 1) + sparse_freq)/8
        unsparse_size = math.ceil(codes.size * bits)/8
        if force_sparse:
            sparse = True
            comp_size = sparse_size
        else:
            sparse = sparse_size < unsparse_size
            comp_size = int(min(sparse_size, unsparse_size) + codebook.size)
        if comp_size >= val.size:
            LOG.warning('value in node %s cannot be compressed smaller with this setting', node.name)
            return None
        comp_val = CompressedVal(compressed_val, bits, codebook, comp_size, sparse)
        self.set(node, idx, val, comp_val)
        return comp_val

    @staticmethod
    def encode_shorter(flattened_val, val):
        freqs = np.unique(flattened_val, return_counts=True)
        codebook = np.concatenate((freqs[0], np.array([0] * (4 - freqs[0].size))))
        compressed_val, codes = ConstantStore.codes_and_compressed(flattened_val, codebook, val.shape)
        return compressed_val, codes, codebook

    @staticmethod
    def cluster(bins, flattened_val, val, inertia=None):
        kmeans = KMeans(n_clusters=bins)
        kmeans.fit(flattened_val.reshape((-1, 1)))
        codebook = kmeans.cluster_centers_
        codebook = codebook.astype(val.dtype).flatten()
        compressed_val, codes = ConstantStore.codes_and_compressed(flattened_val, codebook, val.shape)
        if inertia is not None:
            inertia.append(kmeans.inertia_)
        return compressed_val, codes, codebook

    @staticmethod
    def codes_and_compressed(flattened_val, codebook, val_shape):
        codes = vq(flattened_val, codebook)[0]
        compressed_val = np.array(
            [codebook[code] for code in codes]).reshape(val_shape)
        return compressed_val, codes

    @property
    def fake(self):
        return self._faking

    @fake.setter
    def fake(self, val):
        self._faking = val
