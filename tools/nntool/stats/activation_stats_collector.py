# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from collections import OrderedDict
from typing import Mapping, Sequence

from execution.execute_graph import execute_iterator
from graph.types import FilterParameters, InputParameters
from utils.node_id import NodeId
from utils.stats_funcs import astats, calculate_qsnrs

from .stats_collector import ReductionStatsCollector


def gather_stats(activation, force_ideal=False):
    stat = astats(activation)
    stat['qstats'] = calculate_qsnrs(activation, stat['ibits'], force_ideal)
    return stat

class ActivationStatsCollector(ReductionStatsCollector):
    def _collect(self, G, input_tensors):
        stats = OrderedDict()
        for _, _, node, output, _, fusion_node, details in\
            execute_iterator(G, input_tensors, disable_cache=True):

            key = NodeId(node, fusion_node)
            node = (node if fusion_node is None else fusion_node)
            stat = gather_stats(output[0], force_ideal=not isinstance(node, InputParameters))
            if isinstance(node, FilterParameters) and details:
                stat['min_acc'] = details['min_acc']
                stat['max_acc'] = details['max_acc']
            stats[key] = stat

        return stats

    def _reduce_prepare(self, all_stats: Sequence[Mapping]):
        stats = all_stats.pop()
        for stat in stats.values():
            for field in ['mean', 'std']:
                stat[field] = [stat[field]]
        return stats

    def _reduce(self, _, base: Mapping, stat: Mapping):
        if stat['ibits'] > base['ibits']:
            base['qstats'] = stat['qstats']
            base['ibits'] = stat['ibits']
            base['size'] = stat['size']
        base['max'] = max(base['max'], stat['max'])
        base['min'] = min(base['min'], stat['min'])
        for field in ['mean', 'std']:
            base[field].append(stat[field])

        if 'min_acc' in stat:
            base['min_acc'] = min(stat['min_acc'], base['min_acc'])
            base['max_acc'] = max(stat['max_acc'], base['max_acc'])

    def _reduce_finalize(self, stats: Mapping):
        for stat in stats.values():
            for field in ['mean', 'std']:
                stat[field] = sum(stat[field]) / len(stat[field])
        return stats
