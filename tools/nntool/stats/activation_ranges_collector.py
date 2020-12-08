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

def update_ema(ema, value, decay):
    ema = value * decay + (1 - decay) * ema
    return ema

class ActivationRangesCollector(GraphStatsCollector):
    def __init__(self, graph_execution=None, use_ema=False, ema_decay=0.999):
        super(ActivationRangesCollector, self).__init__()
        self._graph_execution = graph_execution
        self.stats = OrderedDict()
        self.use_ema = use_ema
        self.ema_decay = ema_decay

    def collect_stat(self, stat, name, details, details_name=None):
        range_stat = stat.get(name)
        if not range_stat:
            range_stat = {'min': float('inf'), 'max': float('-inf')}
            stat[name] = range_stat
        if details_name is None:
            self.update_ranges(range_stat, details[name]['min'], details[name]['max'])
        else:
            self.update_ranges(range_stat, details['min_' + details_name], details['max_' + details_name])

    def update_ranges(self, range_out, tensor_min, tensor_max):
        if self.use_ema and all([range_out['min'] != float('inf'), range_out['max'] != float('-inf')]):
            range_out['min'] = update_ema(range_out['min'], tensor_min, self.ema_decay)
            range_out['max'] = update_ema(range_out['max'], tensor_max, self.ema_decay)
        else:
            range_out['min'] = min(range_out['min'], tensor_min)
            range_out['max'] = max(range_out['max'], tensor_max)

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
                range_out = [{'min': float('inf'), 'max': float('-inf'), 'std': 0.0}] * len(output_tensors)
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
                            range_out.extend([{'min': float('inf'), 'max': float('-inf'), 'std': 0.0}
                                              for _ in range(edge.from_idx + 1 - len(range_out))])

            for idx, tensor in enumerate(output_tensors):
                range_out = stat['range_out'][idx]
                self.update_ranges(range_out, tensor.min(), tensor.max())
                range_out['std'] = np.std(tensor)
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
