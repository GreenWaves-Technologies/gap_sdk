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

from typing import Sequence, Tuple

import numpy as np

# ND1D2C versions
def space_to_batch_nd(
        x: np.ndarray,
        block_shape: Sequence[int],
        padding: Sequence[Sequence[int]]) -> np.ndarray:
    x_shape = list(x.shape)
    n = x_shape[0]
    spatial_rank = len(block_shape)
    rest_shape = x_shape[1+spatial_rank:]
    rest_rank = len(rest_shape)

    # pad input
    padding = [(0, 0)] + padding + ([(0, 0)] * rest_rank)
    x_pad = np.pad(x, padding, constant_values=0)
    padded_shape = x_pad.shape

    # compute steps on spatial dims
    spatial_shape = []
    spatial_extract = []
    spatial_keep = []
    spatial_keep_shape = []
    for idx, shape in enumerate(block_shape):
        spatial_keep.append(1 + idx * 2)
        spatial_extract.append(2 + idx * 2)
        spatial_keep_shape.append(padded_shape[1+idx]//shape)
        spatial_shape.extend([
            spatial_keep_shape[-1],
            shape
        ])

    # reshape to new shape
    new_shape = [x.shape[0]] + spatial_shape + rest_shape
    reshaped_padded = np.reshape(x_pad, new_shape)

    # transpose out blocks
    rest_start = 1 + spatial_rank * 2
    transpose = spatial_extract + [0] + spatial_keep + \
        list(range(rest_start, rest_start+rest_rank))
    permuted_reshaped_padded = np.transpose(reshaped_padded, transpose)

    # reshape blocks into batch dim
    batch = n * np.prod(block_shape)
    out_shape = [batch] + spatial_keep_shape + rest_shape
    return np.reshape(permuted_reshaped_padded, out_shape)


def space_to_batch_shape_nd(
        x_shape: Sequence[int],
        block_shape: Sequence[int],
        padding: Sequence[Sequence[int]]) -> Tuple[int]:
    x_shape = list(x_shape)
    block_shape_rank = len(block_shape)
    padded_spatial = [sum(pad) + dim for dim,
                      pad in zip(x_shape[1: block_shape_rank + 1], padding)]

    spatial_keep_shape = [padded_spatial[idx]//shape
                          for idx, shape in enumerate(block_shape)]

    return tuple([x_shape[0] * np.prod(block_shape)] +
                 spatial_keep_shape +
                 x_shape[block_shape_rank + 1:])


def batch_to_space_nd(
        x: np.ndarray,
        block_shape: Sequence[int],
        crops: Sequence[Sequence[int]]) -> np.ndarray:
    x_shape = list(x.shape)
    n = x_shape[0]
    spatial_rank = len(block_shape)
    rest_shape = x_shape[1+spatial_rank:]
    rest_rank = len(rest_shape)
    new_batch = n//np.prod(block_shape)
    # expand batch blocks
    reshaped_shape = block_shape + [new_batch] + x_shape[1:]
    reshaped = np.reshape(x, reshaped_shape)

    # Interleave them back into spatial dim
    transpose = [spatial_rank]
    spatial_reshape = []
    for idx in range(spatial_rank):
        transpose.extend([idx, len(block_shape) + 1 + idx])
        spatial_reshape.append(block_shape[idx]*x_shape[idx+1])
    rest_start = 1 + len(block_shape) * 2
    transpose.extend(list(range(rest_start, rest_start + rest_rank)))
    permuted = np.transpose(reshaped, transpose)

    # Absorb them with a reshape
    out_shape = [new_batch] + spatial_reshape + rest_shape
    reshaped_permuted = np.reshape(permuted, out_shape)

    # crop the original pad
    crops = [[0, 0]] + crops + [[0, 0]] * rest_rank
    crop_slice = tuple([slice(scrop, dim-ecrop) for (dim, (scrop, ecrop))
                        in zip(reshaped_permuted.shape, crops)])
    return reshaped_permuted[crop_slice]


def batch_to_space_shape_nd(
        x_shape: Sequence[int],
        block_shape: Sequence[int],
        crops: Sequence[Sequence[int]] = None) -> Tuple[int]:

    x_shape = list(x_shape)
    block_shape_rank = len(block_shape)
    new_batch = x_shape[0] // np.prod(block_shape)
    new_spatial = [x_shape[idx+1]*dim
                   for idx, dim in enumerate(block_shape)]
    if crops:
        new_spatial_cropped = [dim - sum(crop)
                            for dim, crop in zip(new_spatial, crops)]
    else:
        new_spatial_cropped = new_spatial
    return tuple([new_batch] +
                 new_spatial_cropped +
                 x_shape[1+block_shape_rank:])

def space_to_depth(x, block_size):
    x = np.asarray(x)
    batch, height, width, depth = x.shape
    reduced_height = height // block_size
    reduced_width = width // block_size
    y = x.reshape(batch, reduced_height, block_size,
                         reduced_width, block_size, depth)
    z = np.swapaxes(y, 2, 3).reshape(batch, reduced_height, reduced_width, -1)
    return z