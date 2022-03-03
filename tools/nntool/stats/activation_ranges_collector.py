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
from copy import deepcopy

import numpy as np
from execution.graph_executer import GraphExecuter
from graph.types.fusions import FusionBase, FusionInputParameters
from stats.ranges_utils import collect_stat, update_ranges
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId

from .stats_collector import GraphStatsCollector


def update_peraxis(var, arr: np.ndarray):
    per_axis = var.get('per_axis')
    if not per_axis:
        per_axis = [{'min': np.array(
            [float('inf')] * sz), 'max': np.array(float('-inf') * sz)} for sz in arr.shape]
        var['per_axis'] = per_axis
    for i, _ in enumerate(arr.shape):
        per_axis_elem = per_axis[i]
        other_axis = tuple(j for j in range(len(arr.shape)) if i != j)
        per_axis_elem['min'] = np.minimum(
            per_axis_elem['min'], arr.min(axis=other_axis))
        per_axis_elem['max'] = np.maximum(
            per_axis_elem['max'], arr.max(axis=other_axis))

class Rolling(JsonSerializable):
    def __init__(self) -> None:
        self._values = []

    def __float__(self):
        if not self._values:
            return 0
        return float(np.sum(self._values)/len(self._values))

    def add_val(self, val: float):
        self._values.append(val)

    def _encapsulate(self):
        return float(self)

    def __mul__(self, other):
        return float(self).__mul__(other)

    def __add__(self, other):
        return float(self).__add__(other)

    def __truediv__(self, other):
        return float(self).__truediv__(other)

    def __floordiv__(self, other):
        return float(self).__floordiv__(other)

    def __mod__(self, other):
        return float(self).__mod__(other)

    def __divmod__(self, other):
        return float(self).__divmod__(other)

    def __pow__(self, other):
        return float(self).__pow__(other)

    def __sub__(self, other):
        return float(self).__sub__(other)

    def __radd__(self, other):
        return float(self).__radd__(other)

    def __rsub__(self, other):
        return float(self).__rsub__(other)

    def __rmul__(self, other):
        return float(self).__rmul__(other)

    def __rtruediv__(self, other):
        return float(self).__rtruediv__(other)

    def __rfloordiv__(self, other):
        return float(self).__rfloordiv__(other)

    def __rmod__(self, other):
        return float(self).__rmod__(other)

    def __rpow__(self, other):
        return float(self).__rpow__(other)

    @classmethod
    def _dencapsulate(cls, val):
        return val

    def __repr__(self) -> str:
        return f'{float(self)}'

    def __str__(self) -> str:
        return f'{float(self)}'


class ActivationRangesCollector(GraphStatsCollector):
    def __init__(self, graph_execution=None, use_ema=False, ema_decay=0.999):
        super(ActivationRangesCollector, self).__init__()
        self._graph_execution = graph_execution
        self.stats = OrderedDict()
        self.use_ema = use_ema
        self.ema_decay = ema_decay


    def collect_stat(self, stat: dict, name, details, details_name=None):
        ema_decay = self.ema_decay if self.use_ema else None
        collect_stat(stat, name, details, details_name=details_name, ema_decay=ema_decay)

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
        for _, pnode, fnode, output_tensors, details in\
                graph_execution(input_tensors, step_idx_limit=limit, yield_fusions=True, yield_details=True):
            key = NodeId(pnode, fnode)
            node = (pnode if fnode is None else fnode)
            stat = self.stats.get(key)
            if stat is None:
                range_in = []
                range_out = [
                    {
                        'min': float('inf'),
                        'max': float('-inf'),
                        'std': Rolling(),
                        'mean': Rolling(),
                        'b': Rolling()
                    } for _ in output_tensors]
                stat = {
                    'range_in': range_in,
                    'range_out': range_out,
                }
                self.stats[key] = stat
                if fnode is None or pnode.quantize_internals:
                    if fnode is None:
                        cur_G = G
                    else:
                        cur_G = pnode.subgraph

                    for edge in cur_G.in_edges(node.name):
                        if len(range_in) <= edge.to_idx:
                            range_in.extend(
                                [None] * (edge.to_idx + 1 - len(range_in)))
                        if fnode is None:
                            other_stat = self.stats[NodeId(edge.from_node)]
                            range_in[edge.to_idx] = other_stat['range_out'][edge.from_idx]
                        elif not isinstance(edge.from_node, FusionInputParameters):
                            other_stat = self.stats[NodeId(
                                pnode, edge.from_node)]
                            range_in[edge.to_idx] = other_stat['range_out'][edge.from_idx]

                for edge in G.out_edges(node.name):
                    if len(range_out) <= edge.from_idx:
                        range_out.extend([{'min': float('inf'), 'max': float('-inf'), 'std': 0.0}
                                          for _ in range(edge.from_idx + 1 - len(range_out))])

            for idx, tensor in enumerate(output_tensors):
                range_out = stat['range_out'][idx]
                ema_decay = self.ema_decay if self.use_ema else None
                update_ranges(range_out, tensor.min(), tensor.max(), ema_decay=ema_decay)
                range_out['std'].add_val(np.std(tensor))
                mean = np.mean(tensor)
                range_out['mean'].add_val(mean)
                range_out['b'].add_val(np.mean(np.abs(tensor - mean)))
                update_peraxis(range_out, tensor)

            if details:
                node.details_collector(self.stats, stat, details)

            if isinstance(node, FusionBase) and pnode.quantize_internals:
                for inode in node.subgraph.nodes(node_classes=FusionInputParameters):
                    finput_in_stat = stat['range_in'][inode.idx]
                    for edge in node.subgraph.out_edges(inode.name):
                        fstat = self.stats[NodeId(node, edge.to_node)]
                        fstat['range_in'][edge.to_idx] = finput_in_stat

        return self.stats
