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

import numpy as np


def reverse_transpose(trans):
    if trans is None:
        return trans
    return [trans.index(idx) for idx in range(len(trans))]


def reverses_transpose(trans1, trans2, dim=None):
    """Checks if one transpose reverses another. If a dim is provided then
    look if the transpose sequence produces an equivalent dim to cope with 1s in
    dimensions."""
    if trans1 is None or trans2 is None:
        return False
    if dim and dim.layout_shape == dim.calc_transpose(trans1).calc_transpose(trans2).layout_shape:
        return True
    for idx, val in enumerate(trans1):
        if trans2[val] != idx:
            return False
    return True


def apply_transpose(elems, trans):
    return [elems[i] for i in trans]


def transpose_does_nothing(transpose, shape):
    if transpose is None:
        return False

    def reduce_mask(mask):
        return [dim for dim in mask if dim is not None]
    mask = [idx if dim > 1 else None for idx, dim in enumerate(shape)]
    tmask = reduce_mask(apply_transpose(mask, transpose))
    return reduce_mask(mask) == tmask


def strip_ones(shape):
    return tuple(dim for dim in shape if dim != 1)


def reshape_is_transpose(old_shape, new_shape):
    # TODO - check the order of the non 1 dimensions
    if len(old_shape) != len(new_shape):
        return False
    if set(old_shape) != set(new_shape):
        return False
    if np.prod(set(old_shape)) != np.prod(set(new_shape)):
        return False
    return strip_ones(old_shape) == strip_ones(new_shape)


def get_reshape_transpose(old_shape, new_shape):
    old_shape = list(old_shape)
    transpose = []
    for dim in new_shape:
        idx = old_shape.index(dim)
        transpose.append(idx)
        old_shape[idx] = None
    return tuple(transpose)
