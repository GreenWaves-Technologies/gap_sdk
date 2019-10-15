# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from abc import ABC, abstractmethod
from typing import Sequence

from graph.nngraph import NNGraph
from utils.tabular import Tabular

class Reporter(ABC):
    @abstractmethod
    def report(self, G: NNGraph, stats: Sequence) -> Tabular:
        pass
