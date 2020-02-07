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

import logging
import math
from collections import OrderedDict
from typing import Mapping

import numpy as np

from utils.stats_funcs import qsnr
from utils.node_id import NodeId

from execution.execute_graph import execute, execute_iterator
from execution.quantization_mode import QuantizationMode

from graph.types import FilterParameters

from .stats_collector import ReductionStatsCollector

LOG = logging.getLogger('nntool.' + __name__)

class ErrorStatsCollector(ReductionStatsCollector):
    def __init__(self, limit=None):
        super().__init__()
        self._limit = limit

    def _prepare(self, G):
        pass


    def _collect_execution(self, G, tensors, qrecs=None, qmode=None):
        outputs = []
        fusion_outputs = []
        for step_idx, step, node, output, fusion_op_name, fusion_node, details in\
            execute_iterator(G, tensors, limit=self._limit, qrecs=qrecs, qmode=qmode):
            if qrecs:
                qrec = qrecs[NodeId(node, fusion_node)]
                output = [qrec.out_qs[i].dequantize(out) for i, out in enumerate(output)]
            else:
                output = output.copy()

            del step, fusion_op_name
            if fusion_node:
                fusion_outputs.append({
                    "name": "",
                    "step_idx": "{}_{}".format(step_idx, len(fusion_outputs)),
                    "node": fusion_node,
                    "output": output,
                    "details": details
                })
            else:
                stat = {
                    "name": node.name,
                    "step_idx": str(step_idx),
                    "node": node,
                    "output": output,
                    "details": details,
                    "fusion_outputs": []
                }
                if len(fusion_outputs) > 0:
                    stat['fusion_outputs'] = fusion_outputs.copy()
                    fusion_outputs.clear()
                outputs.append(stat)
        return outputs

    @staticmethod
    def _collect_one(fstat, qstat):
        fout = fstat['output']
        qout = qstat['output']
        error_ = np.abs(fout[0] - qout[0])
        node = fstat['node']
        details = qstat['details']
        if details:
            overflow_dot = details['overflow_dot']
            overflow_acc = details['overflow_acc']
        else:
            overflow_dot = overflow_acc = ""

        stat = {
            'name': fstat['name'],
            'op_name': node.op_name,
            'step': fstat['step_idx'],
            'av_err': np.mean(error_),
            'max_err': np.max(error_),
            'min_err': np.min(error_),
            'qsnr': qsnr(fout[0], qout[0]),
            'overflow_dot' : overflow_dot,
            'overflow_acc' : overflow_acc,
        }

        return stat

    def _collect(self, G, input_tensors) -> Mapping[NodeId, Mapping]:
        LOG.debug("gather quantization statistics")
        foutputs = self._collect_execution(G, input_tensors)
        qoutputs = self._collect_execution(G,
                                           input_tensors,
                                           qrecs=G.quantization,
                                           qmode=QuantizationMode.all())
        stats = OrderedDict()
        for idx, fstat in enumerate(foutputs):
            qstat = qoutputs[idx]
            if fstat['fusion_outputs']:
                for jdx, ffstat in enumerate(fstat['fusion_outputs']):
                    stats[NodeId(fstat['node'], ffstat['node'])] =\
                        self._collect_one(ffstat, qstat['fusion_outputs'][jdx])
            stats[NodeId(fstat['node'], None)] = self._collect_one(fstat, qstat)

        return stats

    def _reduce_prepare(self, all_stats):
        stats = all_stats.pop()
        for stat in stats.values():
            stat['min_qsnr'] = stat['qsnr']
            stat['max_qsnr'] = stat['qsnr']
            for field in ['av_err', 'qsnr']:
                stat[field] = [stat[field]]

        return stats

    def _reduce(self, _, base: Mapping, stat: Mapping):
        for k in ['av_err', 'qsnr']:
            base[k].append(stat[k])
        for k in ['overflow_dot', 'overflow_acc']:
            base[k] += stat[k]
        for k in [('max_err', 'max_err')]:
            base[k[0]] = max(base[k[0]], abs(stat[k[1]]))
        for k in [('min_err', 'min_err')]:
            base[k[0]] = min(base[k[0]], abs(stat[k[1]]))
        for k in [('max_qsnr', 'qsnr')]:
            base[k[0]] = max(base[k[0]], stat[k[1]])
        for k in [('min_qsnr', 'qsnr')]:
            base[k[0]] = min(base[k[0]], stat[k[1]])

    def _reduce_finalize(self, stats: Mapping) -> Mapping:
        for stat in stats.values():
            for field in ['av_err', 'qsnr']:
                stat[field] = sum(stat[field]) / len(stat[field])
        return stats
