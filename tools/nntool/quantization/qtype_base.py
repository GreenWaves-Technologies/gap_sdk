# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from abc import ABC, abstractmethod

from numpy import array


class QTypeBase(ABC):

    @abstractmethod
    def quantize(self, arr: array) -> array:
        pass

    @abstractmethod
    def dequantize(self, arr: array) -> array:
        pass

    @abstractmethod
    def expand_from(self, arr: array, from_qtype: 'QTypeBase') -> array:
        pass

    @abstractmethod
    def reduce_from(self, arr: array, from_qtype: 'QTypeBase') -> array:
        pass
