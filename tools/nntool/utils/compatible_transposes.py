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

from copy import deepcopy
from itertools import groupby
from more_itertools import locate


def append_greater(l1, l2):
    last_idx = -1 if not l1 else l1[-1]
    l2 = [i for i in l2 if i > last_idx]
    return l1 + l2


def add_end(l1, idx, append=False):
    l1 = deepcopy(l1)
    if isinstance(idx, list):
        if append or not l1:
            return l1 + [idx]
        return l1[0:-1:] + [append_greater(l1[-1], idx)]

    if append or not l1:
        return l1 + [[idx]]
    return l1[0:-1:] + [append_greater(l1[-1], [idx])]


def tupled(l):
    return tuple([tuple(elem) for elem in l])


def find_combination(l1, l2, sequence=None, l1_idx=0,
                     factored_l1=False, factored_l2=False,
                     combined_l1=False, combined_l2=False,
                     l2_len=None):

    # set up the run
    if sequence is None:
        l1_idxes = [i for i, val in enumerate(l1) if val > 1]
        reses = find_combination([val for val in l1 if val > 1], l2,
                                 sequence=[], l2_len=len(l2))
        return [tuple([tuple([l1_idxes[i] for i in el]) for el in res]) for res in reses]

    # result cannot exceed l2 length
    if len(sequence) > l2_len:
        return set([])

    # found a possible result
    if not l1 and not l2:
        return set([tupled(sequence)])

    # gobble 1s at the end of l1 (due to factorization)
    if not l2 and l1 and l1[0] == 1:
        return find_combination(l1[1::], l2,
                                sequence=sequence,
                                l1_idx=l1_idx,
                                l2_len=l2_len)

    # gobble 1s at the end of l2
    if not l1 and l2 and l2[0] == 1:
        return find_combination(l1, l2[1::],
                                sequence=sequence,
                                l1_idx=l1_idx,
                                l2_len=l2_len)

    # unequal and no 1s means no result.
    if not l1 or not l2:
        return set([])

    # now look for possible solutions
    res = set()
    # if they are equal gobble both and look at the rest
    if l1[0] == l2[0]:
        res |= find_combination(l1[1::], l2[1::],
                                sequence=add_end(sequence, l1_idx, append=not factored_l2),
                                l1_idx=l1_idx + 1,
                                l2_len=l2_len)

    # if l1 is less then look at factors of l2. Don't factor something already combined.
    if l1[0] <= l2[0] and not combined_l2:
        if l2[0] % l1[0] == 0:
            res |= find_combination(l1[1::], [l2[0]//l1[0]] + l2[1::],
                                    sequence=add_end(sequence, l1_idx, append=not factored_l2),
                                    l1_idx=l1_idx + 1, factored_l2=True,
                                    l2_len=l2_len)

    # if l1 is more then look at factors of l1. Don't factor something already combined.
    if l1[0] > l2[0] and not combined_l1:
        if l1[0] % l2[0] == 0:
            res |= find_combination([l1[0]//l2[0]] + l1[1::], l2[1::],
                                    sequence=add_end(sequence, l1_idx, append=not factored_l2),
                                    l1_idx=l1_idx, factored_l1=True,
                                    l2_len=l2_len)

    # try combining the head of l1 if more than 1 item
    if len(l1) > 1:
        res |= find_combination([l1[0] * l1[1]] + l1[2::], l2,
                                sequence=add_end(sequence, [l1_idx, l1_idx + 1]),
                                l1_idx=l1_idx + 1,
                                combined_l1=True,
                                l2_len=l2_len)

    # try combining the head of l2 if more than 1 item
    if len(l2) > 1:
        res |= find_combination(l1, [l2[0] * l2[1]] + l2[2::],
                                sequence=add_end(sequence, l1_idx, append=not factored_l2),
                                l1_idx=l1_idx, combined_l2=True,
                                l2_len=l2_len)
    return res


def compatible_transpose(remap, trans):
    dims_in_remap = set(i for elem in remap for i in elem)

    class State:
        def __init__(self):
            self.value = -1
            self.key = 0
    flag = State()

    def less_than(o):
        if flag.value >= o:
            flag.key += 1
        flag.value = o
        return flag.key
    reduced_trans = tuple(tuple(g) for _, g in groupby(
        [i for i in trans if i in dims_in_remap], key=less_than))
    if all(reduced_trans[i+1] > reduced_trans[i] for i in range(len(reduced_trans) - 1)):
        return tuple(range(len(remap)))
    if all(elem in reduced_trans for elem in remap):
        return tuple(i for elem in reduced_trans for i in locate(remap, lambda x: x == elem))
    return None


def find_all_compatible_transposes(remaps, transpose):
    return iter([item for item in (compatible_transpose(res, transpose) for res in remaps) if item is not None])


def find_compatible_transpose(remaps, transpose):
    return next(find_all_compatible_transposes(remaps, transpose), None)
