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

from typing import Any

from graph.types import Parameters


class NameCache():
    def __init__(self):
        self._cache = {}

    def set(self, node: Parameters, name: str, val: Any):
        entry = self._cache.get(node)
        if entry is None:
            entry = {}
            self._cache[node] = entry
        entry[name] = val

    def get(self, node: Parameters, name: str) -> Any:
        entry = self._cache.get(node)
        if entry is not None:
            entry = entry.get(name)
        return entry

    def __getitem__(self, param: Parameters):
        if param in self._cache:
            return self._cache[param]
        raise KeyError("%s not found"%param.name)
