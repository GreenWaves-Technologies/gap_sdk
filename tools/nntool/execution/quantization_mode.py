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

from typing import Union
from graph.types import Parameters

class QuantizationMode():
    def __init__(self, qlevel: str = "all", qstep: Union[Parameters, int] = None):
        self._qlevel = qlevel
        self._qstep = qstep

    @classmethod
    def all(cls):
        return cls()

    @classmethod
    def none(cls):
        return cls(qlevel="none")

    @classmethod
    def step(cls, qstep: Union[Parameters, int]):
        return cls(qlevel="step", qstep=qstep)

    def get_quantized(self, node: Parameters, step_idx: int):
        if self._qlevel == "none":
            return False
        if self._qlevel == "all":
            return True
        if isinstance(self._qstep, Parameters):
            return node == self._qstep
        return step_idx == self._qstep

    @property
    def is_step(self):
        return self._qlevel == "step"

    @property
    def is_all(self):
        return self._qlevel == "all"

    @property
    def is_none(self):
        return self._qlevel == "none"

    def __str__(self):
        if self.is_none or self.is_all:
            return self._qlevel
        if isinstance(self._qstep, Parameters):
            return "node {}".format(self._qstep.name)
        return "step # {}".format(self._qstep)
