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

from abc import ABC, abstractmethod, abstractproperty

import numpy as np


class QTypeBase(ABC):

    @abstractmethod
    def quantize(self, arr: np.ndarray) -> np.ndarray:
        pass

    @abstractmethod
    def dequantize(self, arr: np.ndarray) -> np.ndarray:
        pass

    @abstractproperty
    def dtype(self):
        pass

    @abstractproperty
    def q(self) -> int:
        pass

    @abstractproperty
    def bits(self) -> int:
        pass

    @abstractproperty
    def signed(self) -> bool:
        pass

    @abstractproperty
    def pad_zero_point(self) -> int:
        pass

    def clip(self, arr: np.ndarray, dtype=None, narrow_range=False):
        if dtype is None:
            dtype = self.dtype
        iinfo = np.iinfo(dtype)
        qmax = iinfo.max
        qmin = iinfo.min + (1 if narrow_range else 0)
        return np.minimum(np.maximum(arr, qmin), qmax).astype(dtype)
