# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from abc import ABC, abstractmethod

from typing import Mapping
from graph.nngraph import NNGraph
from graph.types import Parameters
from .quantization_record import QuantizationRecord

class Quantizer(ABC):
    @abstractmethod
    def quantize(self, G: NNGraph) -> Mapping[Parameters, QuantizationRecord]:
        pass
