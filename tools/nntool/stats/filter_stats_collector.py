# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
