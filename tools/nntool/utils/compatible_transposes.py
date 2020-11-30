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

from typing import Sequence
from functools import reduce


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
            raise IndexError()
    return None


def len_at_start(l, elem):
    """finds the index in l of the first item that does not match elem"""
    for idx, lelem in enumerate(l):
        if lelem != elem:
            return idx
    return 0


def compatible_transpose(combination, trans):
    """Determines if the transpose can be expressed in the combination in fc
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
        fc_idx = first_idx
        while trans and fc_idx < len(combination):
            segs = combination[fc_idx]
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
            fc_idx += 1
        # this trans element no longer matches the next segment or we
        # reached the end
        if trans:
            trans.pop(0)
        # add range from first to last on first time around
        # this will include idxes idxes of Nones after this segment
        res += list(range(first_idx, fc_idx))
    at_start = len_at_start(combination, None)
    # The only bit we won't have matched is a fc that starts with None's so
    # add those indexes to the start
    return tuple(list(range(0, at_start)) + res)


def find_all_compatible_transposes(combinations, trans):
    for combination in combinations:
        ctrans = compatible_transpose(combination, trans)
        if ctrans:
            yield ctrans


def find_compatible_transpose(fcs, trans):
    return next(find_all_compatible_transposes(fcs, trans), None)
