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


def identity_transpose(trans):
    if trans is None:
        return False
    return list(trans) == sorted(list(trans))


def apply_transpose(elems, trans):
    return [elems[i] for i in trans]


def strip_ones(shape):
    return tuple(dim for dim in shape if dim != 1)


def reverses_transpose_up(trans1, trans2, dim=None):
    """trans1->trans2->dim
    1) without dim do the transposes cancel
    2) with dim to the transposes cancel considering layout shape (i.e. without 1s in shape"""
    if dim is not None and not isinstance(dim, tuple):
        dim = tuple(dim.shape)
    if trans1 is None or trans2 is None:
        return False, None
    if identity_transpose(apply_transpose(trans1, trans2)):
        return True, None
    if dim is not None:
        # apply dim -> reverse t2 -> reverse t1
        # strip 1s and see if it is the same
        layout_shape_after = strip_ones(dim)
        shape_before = apply_transpose(
                apply_transpose(dim, reverse_transpose(trans2)),
                reverse_transpose(trans1))
        return strip_ones(shape_before) == layout_shape_after, shape_before
    return False, None


def indexes_of(trans1, trans2):
    return [trans1.index(i) for i in trans2]


def transpose_does_nothing(transpose, shape):
    if transpose is None:
        return False

    def reduce_mask(mask):
        return [dim for dim in mask if dim is not None]
    mask = [idx if dim > 1 else None for idx, dim in enumerate(shape)]
    tmask = reduce_mask(apply_transpose(mask, transpose))
    return reduce_mask(mask) == tmask


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
