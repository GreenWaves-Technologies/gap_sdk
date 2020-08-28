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

import numpy as np

from collections import OrderedDict

from execution.graph_executer import GraphExecuter
from graph.types import FilterParameters, MultiplicativeBiasParameters, RNNBaseParameters, LSTMParameters
from utils.node_id import NodeId

from .stats_collector import GraphStatsCollector

def update_peraxis(var, arr: np.ndarray):
    per_axis = var.get('per_axis')
    if not per_axis:
        per_axis = [{'min': np.array([float('inf')] * sz), 'max': np.array(float('-inf') * sz)} for sz in arr.shape]
        var['per_axis'] = per_axis
    for i, _ in enumerate(arr.shape):
        per_axis_elem = per_axis[i]
        other_axis = tuple(j for j in range(len(arr.shape)) if i != j)
        per_axis_elem['min'] = np.minimum(per_axis_elem['min'], arr.min(axis=other_axis))
        per_axis_elem['max'] = np.maximum(per_axis_elem['max'], arr.max(axis=other_axis))

class ActivationRangesCollector(GraphStatsCollector):
    def __init__(self, graph_execution=None):
        super(ActivationRangesCollector, self).__init__()
        self._graph_execution = graph_execution
        self.stats = OrderedDict()

    @staticmethod
    def collect_stat(stat, name, details, details_name=None):
        range_stat = stat.get(name)
        if not range_stat:
            range_stat = {'min': float('inf'), 'max': float('-inf')}
            stat[name] = range_stat
        if details_name is None:
            range_stat['min'] = min(range_stat['min'], details[name]['min'])
            range_stat['max'] = max(range_stat['max'], details[name]['max'])
        else:
            range_stat['min'] = min(range_stat['min'], details['min_' + details_name])
            range_stat['max'] = max(range_stat['max'], details['max_' + details_name])


    def collect_stats(self, G, input_tensors, step_idx=None):
        if self._graph_execution is None:
            if G.has_quantized_parameters:
                quantization = G.quantization
            else:
                quantization = None
            graph_executor = GraphExecuter(G, qrecs=quantization)
            graph_execution = graph_executor.execute_iterator
        else:
            graph_execution = self._graph_execution

        limit = step_idx[0] if isinstance(step_idx, tuple) else step_idx
        for _, node, fnode, output_tensors, details in\
                graph_execution(input_tensors, step_idx_limit=limit, yield_fusions=True, yield_details=True):
            key = NodeId(node, fnode)
            node = (node if fnode is None else fnode)
            stat = self.stats.get(key)
            if stat is None:
                range_in = []
                range_out = [{'min': float('inf'), 'max': float('-inf')}]
                stat = {
                    'range_in': range_in,
                    'range_out': range_out,
                }
                self.stats[key] = stat
                if fnode is None:
                    for edge in G.in_edges(node.name):
                        if len(range_in) <= edge.to_idx:
                            range_in.extend([None] * (edge.to_idx + 1 - len(range_in)))
                        other_stat = self.stats[NodeId(edge.from_node)]
                        range_in[edge.to_idx] = other_stat['range_out'][edge.from_idx]
                    for edge in G.out_edges(node.name):
                        if len(range_out) <= edge.from_idx:
                            range_out.extend([{'min': float('inf'), 'max': float('-inf')}
                                              for _ in range(edge.from_idx + 1 - len(range_out))])

            for idx, tensor in enumerate(output_tensors):
                range_out = stat['range_out'][idx]
                range_out['min'] = min(range_out['min'], tensor.min())
                range_out['max'] = max(range_out['max'], tensor.max())
                update_peraxis(range_out, tensor)

            if isinstance(node, FilterParameters) and details:
                self.collect_stat(stat, 'range_acc', details, details_name='acc')
                if isinstance(node, MultiplicativeBiasParameters) and node.has_mul_bias:
                    self.collect_stat(stat, 'range_pre_mul_bias', details, details_name='pre_mul_bias')
            if isinstance(node, RNNBaseParameters) and details:
                self.collect_stat(stat, 'range_state', details)
                if isinstance(node, LSTMParameters):
                    self.collect_stat(stat, 'range_cell', details)

        return self.stats
