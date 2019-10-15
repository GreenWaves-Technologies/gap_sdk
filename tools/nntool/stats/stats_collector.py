# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from abc import ABC, abstractmethod
from typing import Mapping, Sequence

from utils.node_id import NodeId


class StatsCollector(ABC):
    @abstractmethod
    def _collect(self, G) -> Mapping[NodeId, Mapping]:
        pass

    def collect_stats(self, G):
        return self._collect(G)

class ReductionStatsCollector(ABC):
    def __init__(self):
        self._collected_stats = []

    @abstractmethod
    def _collect(self, G, input_tensors) -> Mapping[NodeId, Mapping]:
        pass

    def reduce_stats(self):
        base = self._reduce_prepare(self._collected_stats)
        for stat in self._collected_stats:
            for key, level in stat.items():
                self._reduce(key, base[key], level)
        return self._reduce_finalize(base)

    @abstractmethod
    def _reduce_prepare(self, all_stats: Sequence[Mapping[NodeId, Mapping]])\
        -> Mapping[NodeId, Mapping]:
        pass

    @abstractmethod
    def _reduce(self,
                key: NodeId,
                base: Mapping[NodeId, Mapping],
                stat: Mapping[NodeId, Mapping]):
        pass

    @abstractmethod
    def _reduce_finalize(self, stats: Mapping) -> Mapping[NodeId, Mapping]:
        pass

    def collect_stats(self, G, input_tensors):
        stat = self._collect(G, input_tensors)
        self._collected_stats.append(stat)
        return stat
