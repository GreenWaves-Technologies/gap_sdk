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

from execution.graph_executer import GraphExecuter
from graph.types import FilterParameters, InputParameters, MultiplicativeBiasParameters
from utils.node_id import NodeId
from utils.stats_funcs import astats, calculate_qsnrs

from .stats_collector import ReductionStatsCollector


def gather_stats(activation, force_ideal=False, channel_dim=None, channel_details=None):
    stat = astats(activation, channel_dim=channel_dim, channel_details=channel_details)
    stat['qstats'] = calculate_qsnrs(activation, stat['ibits'], force_ideal)
    return stat

class ActivationStatsCollector(ReductionStatsCollector):
    def __init__(self, graph_execution=None):
        super(ActivationStatsCollector, self).__init__()
        self._graph_execution = graph_execution

    def _collect(self, G, input_tensors, step_idx):
        if self._graph_execution is None:
            if G.has_quantized_parameters:
                quantization = G.quantization
            else:
                quantization = None
            graph_executor = GraphExecuter(G, qrecs=quantization)
            graph_execution = graph_executor.execute_iterator
        else:
            graph_execution = self._graph_execution

        stats = OrderedDict()
        limit = step_idx[0] if isinstance(step_idx, tuple) else step_idx
        for _, node, fnode, output_tensors, details in\
                graph_execution(input_tensors, step_idx_limit=limit, yield_fusions=True, yield_details=True):
            if not self.matches_step(step_idx, node, fnode):
                continue
            key = NodeId(node, fnode)
            node = (node if fnode is None else fnode)
            if node.out_dims[0].is_named and node.out_dims[0].has_key('c'):
                channel_dim = node.out_dims[0].get_order_idx('c')
            else:
                channel_dim = 0
            stat = gather_stats(output_tensors[0],
                                force_ideal=not isinstance(node, InputParameters),
                                channel_dim=channel_dim,
                                channel_details=step_idx is not None)
            if isinstance(node, FilterParameters) and details:
                stat['min_acc'] = details['min_acc']
                stat['max_acc'] = details['max_acc']
                if isinstance(node, MultiplicativeBiasParameters) and node.has_mul_bias:
                    stat['pre_mul_bias_min'] = details['pre_mul_bias_min']
                    stat['pre_mul_bias_max'] = details['pre_mul_bias_max']

            stats[key] = stat

        return stats

    def _reduce_prepare(self, all_stats: Sequence[Mapping]):
        stats = all_stats.pop()
        for stat in stats.values():
            for field in ['mean', 'std', 'avg_prec']:
                if field in stat:
                    stat[field] = [stat[field]]
                else:
                    stat[field] = []
                if 'channel_stats' in stat:
                    for cstat in stat['channel_stats']:
                        if field in cstat:
                            cstat[field] = [cstat[field]]
                        else:
                            cstat[field] = []
        return stats

    @staticmethod
    def reduce_elem(base, stat):
        if stat['ibits'] > base['ibits']:
            base['qstats'] = stat['qstats']
            base['ibits'] = stat['ibits']
            base['size'] = stat['size']
        base['max'] = max(base['max'], stat['max'])
        base['min'] = min(base['min'], stat['min'])
        for field in ['mean', 'std', 'avg_prec']:
            if field in stat:
                base[field].append(stat[field])

        if 'min_acc' in stat:
            base['min_acc'] = min(stat['min_acc'], base['min_acc'])
            base['max_acc'] = max(stat['max_acc'], base['max_acc'])

        if 'pre_mul_bias_min' in stat:
            stat['pre_mul_bias_min'] = min(stat['pre_mul_bias_min'], base['pre_mul_bias_min'])
            stat['pre_mul_bias_max'] = max(stat['pre_mul_bias_max'], base['pre_mul_bias_min'])

    def _reduce(self, _, base: Mapping, stat: Mapping):
        self.reduce_elem(base, stat)
        if 'channel_stats' in stat:
            for chan, cstat in enumerate(stat['channel_stats']):
                self.reduce_elem(base['channel_stats'][chan], cstat)


    def _reduce_finalize(self, stats: Mapping):
        for stat in stats.values():
            for field in ['mean', 'std', 'avg_prec']:
                if len(stat[field]) > 0:
                    stat[field] = sum(stat[field]) / len(stat[field])
                else:
                    stat[field] = None
                if 'channel_stats' in stat:
                    for cstat in stat['channel_stats']:
                        if len(cstat[field]) > 0:
                            cstat[field] = sum(cstat[field]) / len(cstat[field])
                        else:
                            cstat[field] = None
        return stats
