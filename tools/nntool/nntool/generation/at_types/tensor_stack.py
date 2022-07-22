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

# pylint: disable=line-too-long

from typing import Sequence
import logging

LOG = logging.getLogger(__name__)


class TensorStack():
    def __init__(self, out_name: str, in_names: Sequence[str], unused_sizes=None):
        self._out_name = out_name
        self._in_names = in_names
        self._unused_sizes = unused_sizes

    def __str__(self):
        names = [f'"{in_name}"' for in_name in self._in_names]
        if self._unused_sizes:
            for idx, sz in enumerate(self._unused_sizes):
                if sz is None:
                    continue
                names[idx] = f'AT_UnusedStackMember({names[idx]}, {sz})'
        names_len = len(names)
        names = ', '.join(names)
        return f'AddStackedTensors("{self._out_name}", {names_len}, {names});'
