# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from collections import OrderedDict

from graph.types import Conv2DParameters, FcParameters
from utils.node_id import NodeId
from utils.stats_funcs import astats, calculate_qsnrs

from .ranges import Ranges
from .stats_collector import StatsCollector


def filter_stats(node):
    stats = {}
    if node.has_bias:
        stats['biases'] = biases = astats(node.biases)
        biases['qstats'] = calculate_qsnrs(node.biases,
                                           biases['ibits'],
                                           force_ideal=False)
    stats['weights'] = weights = astats(node.weights)
    weights['qstats'] = calculate_qsnrs(node.weights, weights['ibits'],
                                        force_ideal=False)
    # store the statistics into the graph for later use
    node.stats = stats
    return node.stats

STATS_FUNCTIONS = {
    Conv2DParameters: filter_stats,
    FcParameters: filter_stats,
}

class FilterStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G):
        stats = {}
        for _, node, _, fnode in G.nodes_iterator(True):
            key = NodeId(node, fnode)
            node = fnode or node
            if node.__class__ in STATS_FUNCTIONS:
                stats[key] = STATS_FUNCTIONS[node.__class__](node)
        return stats

class FilterDetailedStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G):
        stats = OrderedDict()
        for step_idx, node, _, fnode in G.nodes_iterator(True):
            key = NodeId(node, fnode)
            node = fnode or node
            if isinstance(node, (FcParameters, Conv2DParameters)):
                stats[key] = self.detailed_stat(step_idx, node)
        return stats

    @classmethod
    def detailed_stat(cls, idx, node):
        in_r, max_in_r = Ranges.range_input(node)
        out_r, max_out_r = Ranges.range_output(node)
        in_p = [in_elem/max_in_r for in_elem in in_r]
        out_p = [out_elem/max_out_r for out_elem in out_r]
        return [
            node.name,
            idx,
            min(in_r),
            max(in_r),
            min(out_r),
            max(out_r),
            min(in_p),
            sum(in_p)/len(in_p),
            min(out_p),
            sum(out_p)/len(out_p),
        ]
