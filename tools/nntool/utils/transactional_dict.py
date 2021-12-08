# Copyright (C) 2021 GreenWaves Technologies, SAS

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
from collections.abc import MutableMapping


def rzip(a, b):
    m = min(len(a), len(b))
    for i in range(m):
        yield a[-(i+1)], b[-(i+1)]


class TransactionalDict(MutableMapping):
    def __init__(self, *args, init=None, **kwargs) -> None:
        if init:
            self._stack = [init.copy()]
        else:
            self._stack = [{}]
        self._deleted = [set()]
        self._collapsed = None

    def __getitem__(self, k):
        if self._collapsed is not None:
            return self._collapsed.__getitem__(k) # @IgnoreException
        for dict_elems, deleted_elems in zip(self._stack, self._deleted):
            if k in dict_elems:
                return dict_elems[k]
            if k in deleted_elems:
                break
        raise KeyError(f'{k} not found')  # @IgnoreException

    def __setitem__(self, k, v):
        self._collapsed = None
        self._stack[0].__setitem__(k, v)
        if k in self._deleted[0]:
            self._deleted[0].remove(k)

    def __delitem__(self, k) -> None:
        if len(self._stack) == 1:
            return self._stack[0].__delitem__(k)
        if k in self._stack[0]:
            del self._stack[0][k]
            self._deleted[0].add(k)
            self._collapsed = None
            return
        for dict_elems, deleted_elems in zip(self._stack, self._deleted):
            if k in deleted_elems:
                raise KeyError(f'{k} not found')  # @IgnoreException
            if k in dict_elems:
                self._deleted[0].add(k)
                self._collapsed = None
                return
        raise KeyError(f'{k} not found')  # @IgnoreException

    @property
    def changed_keys(self):
        return list(self._stack[0]) + list(self._deleted[0])

    def _collapse(self):
        if self._collapsed is not None:
            return

        def reduction(state, x):
            state.update(x[0])
            for k in x[1]:
                if k in state:
                    del state[k]
            return state

        self._collapsed = reduce(reduction, rzip(
            self._stack, self._deleted), {})

    def checkpoint(self):
        self._stack.insert(0, {})
        self._deleted.insert(0, set())

    def commitall(self):
        self._collapse()
        self._stack = [self._collapsed]
        self._deleted = [set()]

    def commit(self):
        if len(self._stack) == 1:
            raise ValueError('all checkpoints committed')
        to_commit = self._stack.pop(0)
        self._stack[0].update(to_commit)
        to_delete = self._deleted.pop(0)
        for k in to_delete:
            if k in self._stack[0]:
                del self._stack[0][k]
            if len(self._stack) != 1:
                self._deleted[0].add(k)

    def rollback(self):
        self._stack.pop(0)
        self._deleted.pop(0)
        self._collapsed = None

    def __len__(self) -> int:
        self._collapse()
        return len(self._collapsed)

    def __iter__(self):
        self._collapse()
        return self._collapsed.__iter__()
