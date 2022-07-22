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

from functools import reduce
from typing import Sequence

from nntool.graph.manipulations.eliminate_transposes.transpose_helpers import (
    apply_transpose, reverse_transpose)


def reduce_one(state, num):
    (l, idx) = state
    if num == 1:
        return (l + [None], idx)
    return (l + [idx], idx + 1)


def index_or_none(l):
    return reduce(reduce_one, l, ([], 0))[0]


def set_indexes(res_elem, l1_idxes):
    return tuple(l1_idxes[i] for i in res_elem)


def process_result(result, l1_idxes, l2_idxes):
    return tuple(set_indexes(result[idx], l1_idxes) if idx is not None else None for idx in l2_idxes)


def process_results(results, l1_idxes, l2_idxes):
    """Expands the None elements in l2 and substitutes the real l1 indexes"""
    return set(process_result(result, l1_idxes, l2_idxes) for result in results) - set()


def find_combination(dims1: Sequence, dims2: Sequence, result: Sequence[Sequence] = None,
                     cur_l1: Sequence = None, l1_idx: int = 0):
    """Finds the sequence indexes of elements in l1 that match l2 and returns a list
    of tuples of those indexes. If an element is None then the size of that element
    in l2 is 1. If an index is not present then the size of that element in l1 is 1"""
    if result is None:
        results = find_combination([val for val in dims1 if val > 1],
                                   [val for val in dims2 if val > 1],
                                   result=[],
                                   cur_l1=[0],
                                   l1_idx=0)
        return process_results(results, [idx for idx, val in enumerate(dims1) if val > 1], index_or_none(dims2))
    # if both are empty we match
    if not dims1 and not dims2:
        return [result]
    # if one is empty we fail
    if not dims1 or not dims2:
        return []
    results = []
    # if the start matches we match
    if dims1[0] == dims2[0]:
        results += find_combination(dims1[1::], dims2[1::], result=result +
                                    [cur_l1], cur_l1=[l1_idx+1], l1_idx=l1_idx+1)
    # if we can combine two in l1 do it - l1 must be longer than l2
    if len(dims1) > 1 and len(dims1) > len(dims2):
        results += find_combination([dims1[0]*dims1[1]] + dims1[2::], dims2,
                                    result=result, cur_l1=cur_l1 + [l1_idx+1], l1_idx=l1_idx+1)
    # if we can combine two in l2 do it - l2 must be longer than l1
    if len(dims2) > 1 and len(dims2) > len(dims1):
        results += find_combination(dims1, [dims2[0]*dims2[1]] + dims2[2::],
                                    result=result+[cur_l1], cur_l1=cur_l1, l1_idx=l1_idx)
    return results


def find_first(combination, idx):
    """finds the next non null element in the combination sequence whose first
    element matches the index"""
    for fc_idx, fc_set in enumerate(combination):
        if fc_set is None:
            continue
        if idx == fc_set[0]:
            return fc_idx
        elif idx in fc_set:
            # item is not first so no match
            raise IndexError()  # @IgnoreException
    return None


def len_at_start(l, elem):
    """finds the index in l of the first item that does not match elem"""
    for idx, lelem in enumerate(l):
        if lelem != elem:
            return idx
    return 0


def compatible_transpose(combination, trans):
    """Determines if the transpose can be expressed in the combination in descriptor found by find_combination
    """
    res = []
    trans = list(trans)
    while trans:
        try:
            first_idx = find_first(combination, trans[0])
        except IndexError:
            # the next non null segment doesn't match so bail
            return False
        if first_idx is None:
            # this element was not found in the sequence which means it doesn't matter (it's 1)
            trans.pop(0)
            continue
        # we have a segment who's first element matches the transpose
        # the rest of the elements in the segment must be in order
        descriptor_idx = first_idx
        while trans and descriptor_idx < len(combination):
            segs = combination[descriptor_idx]
            if segs is not None:  # if segs is none then it matches anything
                # the first seg must match since it was found by find_first
                # this tests if another seg doesn't match in which case we are
                # successful and break
                if segs[0] != trans[0]:
                    break
                # now check the rest of the seg. Always leave the last trans
                # element in trans so we can look for it in a subsequent trans
                for oidx in segs[1::]:
                    trans.pop(0)
                    # element in segment doesn't match the transpose
                    # so no solution
                    if trans[0] != oidx:
                        return False
            descriptor_idx += 1
        # this trans element no longer matches the next segment or we
        # reached the end
        if trans:
            trans.pop(0)
        # add range from first to last on first time around
        # this will include idxes idxes of Nones after this segment
        res += list(range(first_idx, descriptor_idx))
    at_start = len_at_start(combination, None)
    # The only bit we won't have matched is a descriptor that starts with None's so
    # add those indexes to the start
    return tuple(list(range(0, at_start)) + res)


def find_all_compatible_transposes(combinations, trans):
    for combination in combinations:
        ctrans = compatible_transpose(combination, trans)
        if ctrans:
            yield ctrans


def find_compatible_transpose(fcs, trans):
    return next(find_all_compatible_transposes(fcs, trans), None)


def expand_to_len(trans, length):
    extra = length-len(trans)
    return tuple(list(range(extra)) + [dim + extra for dim in trans])


def reduce_to_len(trans, length):
    extra = len(trans) - length
    return tuple([dim - extra for dim in trans if dim >= extra])


def no_ones(l):
    return tuple(elem for elem in l if elem != 1)


def ones_shuffled(from_shape, to_shape):
    if len(from_shape) != len(to_shape) or no_ones(from_shape) != no_ones(to_shape):
        return False
    return True


def reshape_shuffle_trans(from_shape, to_shape):
    from_shape = list(enumerate(from_shape))
    to_shape = list(enumerate(to_shape))
    ones_pos_from = tuple(shape[0] for shape in from_shape if shape[1] == 1)
    ones_pos_to = list(shape[0] for shape in to_shape if shape[1] == 1)
    idx_to = 0
    idx_from = 0
    idx_from_ones = 0
    trans = []
    for idx_to in range(len(to_shape)):
        if idx_to in ones_pos_to:
            trans.append(ones_pos_from[idx_from_ones])
            idx_from_ones += 1
        else:
            while idx_from in ones_pos_from:
                idx_from += 1
            trans.append(idx_from)
            idx_from += 1
    return trans


def is_broadcasted(from_shape, to_shape):
    from_len = len(from_shape)
    to_len = len(to_shape)
    if from_len >= to_len:
        return False
    return tuple(([1] * (to_len - from_len)) + list(from_shape)) == tuple(to_shape)


def broadcast_transpose(from_shape, to_shape, going_up):
    from_len = len(from_shape)
    to_len = len(to_shape)
    if going_up:
        return tuple((idx,) for idx in range(to_len - from_len, to_len))
    return tuple(([None] * (to_len - from_len))+list(range(from_len)))


def apply_combination(shape, comb):
    res = []
    comb = list(comb)
    while comb:
        elem = comb.pop(0)
        if elem is None:
            res.append(1)
        else:
            res.append(reduce(lambda x, y: x*y, [shape[i] for i in elem]))
    return tuple(res)


def transpose_combination(comb, trans):
    res = []
    comb = list(comb)
    while comb:
        elem = comb.pop(0)
        if elem is None:
            res.append(None)
        else:
            res.append(tuple(trans.index(i) for i in elem))
    return tuple(res)


def calc_new_reshape(trans, new_trans, from_shape, to_shape, going_up):
    if going_up:
        # transpose is in the up direction so is reversed
        # we want to apply it in the down direction so reverse it
        new_to_shape = apply_transpose(to_shape, reverse_transpose(trans))
        # the from_shape gets the new transpose applied to it - this may result in the reshape being eliminated
        # since the shape change that it caused is already in the transpose
        # NOTE - Looking at the reshape as a transpose itself is not correct. It is a shuffle not a transpose
        # the tensor physical order is not changed unlike a transpose
        new_from_shape = apply_transpose(
            from_shape, reverse_transpose(new_trans))
    else:
        # transpose is in the down direction but we want to pass it through this reshape so
        # we want to reverse its effect
        new_from_shape = apply_transpose(from_shape, reverse_transpose(trans))
        # the to_shape gets the new transpose applied to it - this may result in the reshape being eliminated
        # since the shape change that it caused is already in the transpose
        new_to_shape = apply_transpose(to_shape, reverse_transpose(new_trans))
    return (
        tuple(new_trans),
        tuple(new_from_shape),
        tuple(new_to_shape))


def calc_failure_reshapes(trans, from_shape, to_shape, going_up):
    if going_up:
        new_to_shape = tuple(apply_transpose(
            to_shape, reverse_transpose(trans)))
        new_from_shape = None
    else:
        new_from_shape = tuple(apply_transpose(
            from_shape, reverse_transpose(trans)))
        new_to_shape = None
    return (
        None,
        new_from_shape,
        new_to_shape)


def reverse_reshape(trans, from_shape, to_shape, going_up=False, history=None):
    """reverses the effect of this reshape on the transpose. If going up is set then then
    the transpose is in the direction to_shape -> from_shape"""

    if len(from_shape) == 0 or len(to_shape) == 0:
        return calc_failure_reshapes(trans, from_shape, to_shape, going_up)

    # if the from_shape -> to_shape is actually a broadcast reshape
    # i.e. 4, 10, 1 -> 1, 4, 10, 1 we absolutely need to keep the order 4, 10, 1 in
    # the transpose however the 2 1s in the result are ambiguous so handle this as a
    # (simple) special case. Just expand the transpose with no transpose at the start
    # and expand_len + original transpose dim at the end
    if is_broadcasted(from_shape, to_shape):
        broad_trans = broadcast_transpose(from_shape, to_shape, going_up)
        if going_up:
            new_trans = reverse_transpose(reduce_to_len(
                reverse_transpose(trans), len(from_shape)))
        else:
            new_trans = reverse_transpose(expand_to_len(
                reverse_transpose(trans), len(to_shape)))
        return calc_new_reshape(trans, new_trans, from_shape, to_shape, going_up)

    # consider the shapes in the correct order
    shape_order = (to_shape, from_shape) if going_up else (
        from_shape, to_shape)

    if ones_shuffled(shape_order[0], shape_order[1]):
        shuffle_trans = reshape_shuffle_trans(shape_order[0], shape_order[1])
        new_trans = apply_transpose(trans, shuffle_trans)
        return calc_new_reshape(trans, new_trans, from_shape, to_shape, going_up)

    for combination in find_combination(*shape_order):
        if not combination:
            continue
        # going down we are looking at where we could transpose the reshape combination back up the
        # graph in a valid way and then reverse that transpose
        # going up re are propagating a reversed transpose so we still need to reverse
        reversed_new_trans = compatible_transpose(
            combination, reverse_transpose(trans))
        if not reversed_new_trans or len(reversed_new_trans) != len(shape_order[1]):
            continue
        new_trans = reverse_transpose(reversed_new_trans)
        return calc_new_reshape(trans, new_trans, from_shape, to_shape, going_up)

    return calc_failure_reshapes(trans, from_shape, to_shape, going_up)
