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

from functools import reduce
from typing import Sequence, Union, Any


def disjoint_reduction(sets: Sequence[Union[set, frozenset]]) -> Sequence[set]:
    """Reduce a series of sets to an elementary set of disjoint sets that can express all the sets
    i.e. {a,b}, {a,b,c} {d} {c, d} -> {a,b} {c} {d}
    {a,b} -> {a,b}
    {a,b,c} -> {a,b} {c}
    {c,d} -> {c} {d}
    {d} -> {d}

    This is inspired by this great stack overflow answer:
    https://stackoverflow.com/questions/34693166/what-can-be-the-algorithm-to-find-all-disjoint-sets-from-a-set-of-sets/34693890#34693890
    """

    def reduction(state: dict, elem: Any):
        # key in the map is the set of membership of the element in each input set
        state.setdefault(frozenset(elem_set_idx for elem_set_idx, elem_set in enumerate(sets)
                                   if elem in elem_set), set()).add(elem)
        return state

    return reduce(reduction, frozenset.union(*sets), {}).values()
