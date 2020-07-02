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

from abc import ABC, abstractmethod
from typing import Mapping, Sequence

from utils.node_id import NodeId

class StatsCollector(ABC):
    def __init__(self):
        self._fusion_cnt = None

    @abstractmethod
    def _collect(self, G, step_idx) -> Mapping[NodeId, Mapping]:
        pass

    def matches_step(self, step_idx, node, fnode):
        if step_idx is None:
            return True
        if isinstance(step_idx, tuple):
            if fnode is None or step_idx[0] != node.step_idx:
                return False
            if self._fusion_cnt is None:
                self._fusion_cnt = 0
            else:
                self._fusion_cnt += 1
            return step_idx[1] == self._fusion_cnt
        return step_idx == node.step_idx and fnode is None

    def collect_stats(self, G, step_idx=None):
        return self._collect(G, step_idx)

class GraphStatsCollector(ABC):
    @abstractmethod
    def collect_stats(self, G, input_tensors, step_idx=None):
        pass

class ReductionStatsCollector(GraphStatsCollector):
    def __init__(self):
        self._collected_stats = []
        self._fusion_cnt = None

    @abstractmethod
    def _collect(self, G, input_tensors, step_idx) -> Mapping[NodeId, Mapping]:
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

    def matches_step(self, step_idx, node, fnode):
        if step_idx is None:
            return True
        if isinstance(step_idx, tuple):
            if fnode is None or step_idx[0] != node.step_idx:
                return False
            if self._fusion_cnt is None:
                self._fusion_cnt = 0
            else:
                self._fusion_cnt += 1
            return step_idx[1] == self._fusion_cnt
        return step_idx == node.step_idx and fnode is None

    def collect_stats(self, G, input_tensors, step_idx=None):
        stat = self._collect(G, input_tensors, step_idx)
        self._collected_stats.append(stat)
        return stat
