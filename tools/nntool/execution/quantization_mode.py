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

from typing import Union, Optional
from graph.types import Parameters
from utils.node_id import NodeId

class QuantizationMode():
    def __init__(self, qlevel: str = "all", qstep: Optional[Union[int, NodeId]] = None, dequantize=False):
        self._qlevel = qlevel
        self._qstep = qstep
        self._dequantize = dequantize

    @classmethod
    def all(cls):
        return cls()

    @classmethod
    def step_all(cls):
        return cls(qlevel="step_all")

    @classmethod
    def all_float_quantize_dequantize(cls):
        return cls(qlevel="float_q_deq")

    @classmethod
    def all_dequantize(cls):
        return cls(dequantize=True)

    @classmethod
    def none(cls):
        return cls(qlevel="none")

    @classmethod
    def step(cls, qstep: Union[Parameters, int]):
        return cls(qlevel="step", qstep=qstep)

    def get_quantized(self, node: Parameters, step_idx: int):
        if self._qlevel in ("none", "float_q_deq"):
            return False
        if self._qlevel == "all":
            return True
        if isinstance(self._qstep, Parameters):
            return node == self._qstep
        return step_idx == self._qstep

    @property
    def is_float_q_deq(self):
        return self._qlevel == "float_q_deq"

    @property
    def is_step(self):
        return self._qlevel == "step"

    @property
    def is_step_all(self):
        return self._qlevel == "step_all"

    @property
    def is_all(self):
        return self._qlevel == "all"

    @property
    def is_none(self):
        return self._qlevel == "none"

    @property
    def dequantize(self):
        return (self.is_step or self.is_all) and self._dequantize

    def __str__(self):
        if self.is_none or self.is_all:
            return self._qlevel
        if isinstance(self._qstep, Parameters):
            return "node {}".format(self._qstep.name)
        return "step # {}".format(self._qstep)
