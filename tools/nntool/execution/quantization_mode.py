# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
