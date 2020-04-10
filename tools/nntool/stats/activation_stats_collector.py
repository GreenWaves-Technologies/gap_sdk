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

from collections import OrderedDict
from typing import Mapping, Sequence

from execution.execute_graph import execute_iterator
from graph.types import FilterParameters, InputParameters
from utils.node_id import NodeId
from utils.stats_funcs import astats, calculate_qsnrs

from .stats_collector import ReductionStatsCollector


def gather_stats(activation, force_ideal=False, channel_dim=None, channel_details=None):
    stat = astats(activation, channel_dim=channel_dim, channel_details=channel_details)
    stat['qstats'] = calculate_qsnrs(activation, stat['ibits'], force_ideal)
    return stat

class ActivationStatsCollector(ReductionStatsCollector):
    def _collect(self, G, input_tensors, step_idx):
        stats = OrderedDict()
        limit = step_idx[0] if isinstance(step_idx, tuple) else step_idx
        for _, _, node, output, _, fusion_node, details in\
                execute_iterator(G, input_tensors, disable_cache=True, limit=limit):
            if not self.matches_step(step_idx, node, fusion_node):
                continue
            key = NodeId(node, fusion_node)
            node = (node if fusion_node is None else fusion_node)
            if node.out_dims[0].is_named and node.out_dims[0].has_key('c'):
                channel_dim = node.out_dims[0].get_order_idx('c')
            else:
                channel_dim = 0
            stat = gather_stats(output[0],
                                force_ideal=not isinstance(node, InputParameters),
                                channel_dim=channel_dim,
                                channel_details=step_idx is not None)
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
                if 'channel_stats' in stat:
                    for cstat in stat['channel_stats']:
                        cstat[field] = [cstat[field]]
        return stats

    @staticmethod
    def reduce_elem(base, stat):
        if 'min_prec' in stat:
            if 'min_prec' in base:
                base['min_prec'] = min(stat['min_prec'], base['min_prec'])
            else:
                base['min_prec'] = stat['min_prec']
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

    def _reduce(self, _, base: Mapping, stat: Mapping):
        self.reduce_elem(base, stat)
        if 'channel_stats' in stat:
            for chan, cstat in enumerate(stat['channel_stats']):
                self.reduce_elem(base['channel_stats'][chan], cstat)


    def _reduce_finalize(self, stats: Mapping):
        for stat in stats.values():
            for field in ['mean', 'std']:
                stat[field] = sum(stat[field]) / len(stat[field])
                if 'channel_stats' in stat:
                    for cstat in stat['channel_stats']:
                        cstat[field] = sum(cstat[field]) / len(cstat[field])
        return stats
