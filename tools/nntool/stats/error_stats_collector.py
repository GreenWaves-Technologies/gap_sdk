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

import logging
from collections import OrderedDict
from typing import Mapping

import numpy as np

from utils.stats_funcs import qsnr, cos_similarity
from utils.node_id import NodeId

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode

from stats.stats_collector import ReductionStatsCollector

LOG = logging.getLogger('nntool.' + __name__)


class ErrorStatsCollector(ReductionStatsCollector):
    def __init__(self, limit=None, quant_compare=False):
        super().__init__()
        self._limit = limit
        self._quant_compare = quant_compare

    def _prepare(self, G):
        pass

    def _collect_execution(self, executer, tensors, qrecs, qmode=None):
        del qrecs
        outputs = []
        fusion_outputs = []
        for step_idx, pnode, fnode, output, details in\
                executer.execute_iterator(tensors, step_idx_limit=self._limit, qmode=qmode):

            if fnode:
                fusion_outputs.append({
                    "name": "",
                    "step_idx": "{}_{}".format(step_idx, len(fusion_outputs)),
                    "node": fnode,
                    "output": output,
                    "details": details
                })
            else:
                stat = {
                    "name": pnode.name,
                    "step_idx": str(step_idx),
                    "node": pnode,
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
    def _collect_one(fstat, qstat, qrec, quant_compare=False):
        fout = fstat['output'][0]
        if quant_compare:
            fout = qrec.out_qs[0].dequantize(qrec.out_qs[0].quantize(fout))
        qout = qstat['output'][0]
        error_ = np.abs(fout - qout)
        node = fstat['node']

        stat = {
            'name': fstat['name'],
            'op_name': node.op_name,
            'step': fstat['step_idx'],
            'av_err': np.mean(error_),
            'max_err': np.max(error_),
            'min_err': np.min(error_),
            'qsnr': qsnr(fout, qout),
            'cos': cos_similarity(fout, qout)
        }

        return stat

    def _collect(self, G, input_tensors, step_idx) -> Mapping[NodeId, Mapping]:
        LOG.debug("gather quantization statistics")
        if G.has_quantized_parameters:
            quantization = G.quantization
        else:
            quantization = None
        executer = GraphExecuter(G, qrecs=quantization)
        foutputs = self._collect_execution(executer, input_tensors, quantization)
        executer = GraphExecuter(G, qrecs=G.quantization)
        qoutputs = self._collect_execution(executer,
                                           input_tensors,
                                           G.quantization,
                                           qmode=QuantizationMode.all_dequantize())
        stats = OrderedDict()
        for idx, fstat in enumerate(foutputs):
            qstat = qoutputs[idx]
            if fstat['fusion_outputs']:
                for jdx, ffstat in enumerate(fstat['fusion_outputs']):
                    nid = NodeId(fstat['node'], ffstat['node'])
                    stats[nid] =\
                        self._collect_one(ffstat,
                                          qstat['fusion_outputs'][jdx],
                                          G.quantization[nid],
                                          quant_compare=self._quant_compare)
            nid = NodeId(fstat['node'], None)
            stats[nid] = self._collect_one(fstat,
                                           qstat,
                                           G.quantization[nid],
                                           quant_compare=self._quant_compare)

        return stats

    def _reduce_prepare(self, all_stats):
        stats = all_stats.pop()
        for stat in stats.values():
            stat['min_qsnr'] = stat['qsnr']
            stat['max_qsnr'] = stat['qsnr']
            stat['min_cos'] = stat['cos']
            stat['max_cos'] = stat['cos']
            for field in ['av_err', 'qsnr', 'cos']:
                stat[field] = [stat[field]]

        return stats

    def _reduce(self, _, base: Mapping, stat: Mapping):
        for k in ['av_err', 'qsnr', 'cos']:
            base[k].append(stat[k])
        for k in [('max_err', 'max_err')]:
            base[k[0]] = max(base[k[0]], abs(stat[k[1]]))
        for k in [('min_err', 'min_err')]:
            base[k[0]] = min(base[k[0]], abs(stat[k[1]]))
        for k in [('max_qsnr', 'qsnr'), ('max_cos', 'cos')]:
            base[k[0]] = max(base[k[0]], stat[k[1]])
        for k in [('min_qsnr', 'qsnr', ('min_cos', 'cos'))]:
            base[k[0]] = min(base[k[0]], stat[k[1]])

    def _reduce_finalize(self, stats: Mapping) -> Mapping:
        for stat in stats.values():
            for field in ['av_err', 'qsnr', 'cos']:
                stat[field] = sum(stat[field]) / len(stat[field])
        return stats
