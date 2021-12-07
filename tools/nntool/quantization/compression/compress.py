# Copyright (C) 2021  GreenWaves Technologies, SAS

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

USE_KMEANS_CUDA = False
if USE_KMEANS_CUDA:
    try:
        from libKMCUDA import kmeans_cuda
    except ImportError:
        kmeans_cuda = None

USE_KMEANS_1D = True
if USE_KMEANS_1D:
    try:
        import kmeans1d
    except ImportError:
        kmeans1d = None


LOG = logging.getLogger("nntool." + __name__)

CompressedVal = namedtuple(
    'CompressedVal', ['value', 'bits', 'codebook', 'size', 'sparse', 'sparse_idx', 'threshold'])

class CompressionError(Exception):
    pass

def compress(orig_val, bits=None, min_qsnr=None, force_sparse=False,
             allow_sparse=True, qbits=8, threshold=None, force=True):
    val = orig_val.copy()
    manual_bits = bits is not None and force
    if threshold:
        val[np.logical_and(val < threshold, val > 0)] = 0
        val[np.logical_and(val > np.negative(threshold), val < 0)] = 0

    if np.all(val == 0):
        raise CompressionError('value is all zeros')
    flattened_val = val.flatten()
    codes = None
    if val.size <= 4:
        raise CompressionError('value is too small to compress')
    if bits is not None:
        bins = int(math.pow(2, bits)) + (1 if force_sparse else 0)
        if bins > val.size:
            bits = max(int(math.floor(math.log2(val.size))), 2)
            bins = int(math.pow(2, bits))
            LOG.info(f'more bins than values - reducing to {bits} bits')
        compressed_val, codes, codebook = cluster(
            bins, flattened_val, val)
    elif min_qsnr:
        cur_qsnr = -math.inf
        bits = 1
        while cur_qsnr < min_qsnr:
            if bits == 8 and not force_sparse:
                bins = int(math.pow(2, bits)) + 1
                force_sparse = True
            else:
                bits += 1
                if bits > 8:
                    raise CompressionError(f'value cannot meet {min_qsnr} QSNR at 8 bits or under')
                bins = int(math.pow(2, bits)) + (1 if force_sparse else 0)
            if bins > val.size:
                raise CompressionError('value cannot be reduced in size')
            compressed_val, codes, codebook = cluster(
                bins, flattened_val, val)
            cur_qsnr = qsnr(compressed_val.astype(
                np.float32), val.astype(np.float32))
    else:
        # automatic search of optimal k with inertia method
        silhouette = []
        inertia = []
        for bits in range(2, 8):
            bins = int(math.pow(2, bits))
            if bins > val.size - 1:
                break
            compressed_val, codes, codebook = cluster(
                bins, flattened_val, val, inertia=inertia)
            silhouette.append(silhouette_score(flattened_val.reshape(-1, 1),
                                               compressed_val.flatten()))
        if len(inertia) <= 1:
            compressed_val, codes, codebook = encode_shorter(
                flattened_val, val)
        else:
            # 2nd grade derivative to find the elbow
            if len(inertia) > 2:
                cinertia = np.array(inertia)
                # cinertia[cinertia>1] = 1
                elb_idx = np.argmax(np.diff(np.diff(cinertia))) + 1
            else:
                elb_idx = 1
            # take the three around the elbow and look at the silhouette
            bits = np.argmax(
                np.array(silhouette[elb_idx-1:elb_idx+1])) + elb_idx + 1
            bins = int(math.pow(2, bits))
            compressed_val, codes, codebook = cluster(
                bins, flattened_val, val)
    # see if sparse representation is better
    unsparse_size = int(math.ceil(codes.size * bits)/8)
    qelem_codebook_size = math.ceil((codebook.size * qbits)/8)
    uncompressed_size = int(math.ceil((val.size * qbits)/8))
    if allow_sparse:
        sparse_idx, sparse_freq = compute_sparse(codes)
        sparse_size = int(
            math.ceil((val.size - sparse_freq) * (bits + 1) + sparse_freq)/8)
        if force_sparse or sparse_size < unsparse_size:
            sparse = True
            compressed_size = sparse_size
        else:
            sparse = False
            compressed_size = unsparse_size
    else:
        compressed_size = unsparse_size
        sparse = False
        sparse_idx = 0

    compressed_size += qelem_codebook_size
    if not manual_bits and compressed_size >= uncompressed_size:
        raise CompressionError('value cannot be reduced in size')
    comp_val = CompressedVal(
        compressed_val, bits,
        codebook, compressed_size, sparse, sparse_idx, threshold)
    return comp_val

def compute_sparse(codes):
    freqs = np.unique(codes, return_counts=True)
    sparse_idx = np.where(freqs[1] == freqs[1].max())[0][0]
    sparse_freq = freqs[1][sparse_idx]
    return sparse_idx, sparse_freq


def encode_shorter(flattened_val, val):
    freqs = np.unique(flattened_val, return_counts=True)
    codebook = np.concatenate(
        (freqs[0], np.array([0] * (4 - freqs[0].size))))
    compressed_val, codes = codes_and_compressed(
        flattened_val, codebook, val.shape)
    return compressed_val, codes, codebook


def cluster(bins, flattened_val, val, inertia=None):
    if USE_KMEANS_CUDA and kmeans_cuda:
        invalids = None
        int_bins = bins
        while invalids is None or int_bins - invalids < bins:
            if invalids:
                int_bins = bins + invalids
            codebook, _ = kmeans_cuda(
                flattened_val.reshape((-1, 1)), int_bins, device=1)
            invalids = np.count_nonzero(np.isnan(codebook).any(axis=1)) + np.count_nonzero(
                np.isneginf(codebook).any(axis=1)) + np.count_nonzero(np.isposinf(codebook).any(axis=1))
        # this is not a good solution since it will possibly end up with the wrong numer of bins
        # however kmeans cuda is not easy to install so probably not right anyway
        codebook = codebook[~np.isnan(codebook).any(axis=1)]
        codebook = codebook[~np.isneginf(codebook).any(axis=1)]
        codebook = codebook[~np.isposinf(codebook).any(axis=1)]
        last_inertia = None
    elif USE_KMEANS_1D and kmeans1d:
        clustered = kmeans1d.cluster(flattened_val, bins)
        codebook = np.array(clustered.centroids)
        encoded = codebook[clustered.clusters]
        last_inertia = np.sum(np.power(flattened_val, 2) - np.power(encoded, 2))
    else:
        # scipy is horribly slow
        kmeans = KMeans(n_clusters=bins)
        kmeans.fit(flattened_val.reshape((-1, 1)))
        codebook = kmeans.cluster_centers_
        last_inertia = kmeans.inertia_
    codebook = codebook.astype(val.dtype).flatten()
    compressed_val, codes = codes_and_compressed(
        flattened_val, codebook, val.shape)
    if last_inertia is not None and inertia is not None:
        inertia.append(last_inertia)
    return compressed_val, codes, codebook


def codes_and_compressed(flattened_val, codebook, val_shape):
    codes = vq(flattened_val, codebook)[0]
    compressed_val = np.array(
        [codebook[code] for code in codes]).reshape(val_shape)
    return compressed_val, codes
