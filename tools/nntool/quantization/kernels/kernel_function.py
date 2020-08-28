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

from abc import ABC, abstractproperty
from typing import Callable, Iterable, NewType, Mapping, Any, Optional

import numpy as np

from graph.types import Parameters
from quantization.quantization_record_base import QuantizationRecordBase

KernelFunction = NewType('KernelFunction',
                         Callable[
                             [Parameters,
                              Iterable[np.ndarray],
                              QuantizationRecordBase,
                              Optional[Mapping[Any, Any]]],
                             Iterable[np.ndarray]
                         ])

class KernelFunctionSetBase(ABC):

    @abstractproperty
    def graph_input(self) -> KernelFunction:
        pass

    @abstractproperty
    def graph_output(self) -> KernelFunction:
        pass

    @abstractproperty
    def constant_input(self) -> KernelFunction:
        pass

    @abstractproperty
    def relu(self) -> KernelFunction:
        pass

    @abstractproperty
    def leaky(self) -> KernelFunction:
        pass

    @abstractproperty
    def hswish(self) -> KernelFunction:
        pass

    @abstractproperty
    def hsigmoid(self) -> KernelFunction:
        pass

    @abstractproperty
    def matadd(self) -> KernelFunction:
        pass

    @abstractproperty
    def matsub(self) -> KernelFunction:
        pass

    @abstractproperty
    def matdiv(self) -> KernelFunction:
        pass

    @abstractproperty
    def matmul(self) -> KernelFunction:
        pass

    @abstractproperty
    def matscale(self) -> KernelFunction:
        pass

    @abstractproperty
    def conv2d(self) -> KernelFunction:
        pass

    @abstractproperty
    def linear(self) -> KernelFunction:
        pass

    @abstractproperty
    def softmax(self) -> KernelFunction:
        pass

    @abstractproperty
    def reshape(self) -> KernelFunction:
        pass

    @abstractproperty
    def transpose(self) -> KernelFunction:
        pass

    @abstractproperty
    def concat(self) -> KernelFunction:
        pass

    @abstractproperty
    def av_pool(self) -> KernelFunction:
        pass

    @abstractproperty
    def av_global_pool(self) -> KernelFunction:
        pass

    @abstractproperty
    def max_pool(self) -> KernelFunction:
        pass

    @abstractproperty
    def max_global_pool(self) -> KernelFunction:
        pass

    @abstractproperty
    def sum_global_pool(self) -> KernelFunction:
        pass

    @abstractproperty
    def pad(self) -> KernelFunction:
        pass

    @abstractproperty
    def image_format(self) -> KernelFunction:
        pass

    @abstractproperty
    def rnn(self) -> KernelFunction:
        pass

    @abstractproperty
    def strided_slice(self) -> KernelFunction:
        pass

    @abstractproperty
    def cast(self) -> KernelFunction:
        pass

    @abstractproperty
    def split(self) -> KernelFunction:
        pass

    @abstractproperty
    def copy(self) -> KernelFunction:
        pass
