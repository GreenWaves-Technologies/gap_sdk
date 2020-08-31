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
import math
from collections import OrderedDict
from typing import Mapping

import numpy as np

from utils.stats_funcs import qsnr, cos_similarity
from utils.node_id import NodeId

from execution.graph_executer import GraphExecuter

from .stats_collector import ReductionStatsCollector

LOG = logging.getLogger('nntool.' + __name__)


class StepErrorStatsCollector(ReductionStatsCollector):
    def __init__(self, limit=None, quant_compare=False):
        super().__init__()
        self._limit = limit
        self._quant_compare = quant_compare

    def _prepare(self, G):
        pass

    def _collect_execution(self, G, tensors, qrecs):
        outputs = []
        fusion_outputs = []
        executer = GraphExecuter(G, qrecs)
        for step_idx, node, output, details, qoutput, qdetails, fusion_node in\
                executer.execute_qnoq_iterator(tensors):
            output = [np.copy(out) for out in output]
            qoutput = [np.copy(out) for out in qoutput]

            if fusion_node:
                fusion_outputs.append({
                    "name": "",
                    "step_idx": "{}_{}".format(step_idx, len(fusion_outputs)),
                    "node": fusion_node,
                    "output": output,
                    "details": details,
                    "qoutput": qoutput,
                    "qdetails": qdetails
                })
            else:
                stat = {
                    "name": node.name,
                    "step_idx": str(step_idx),
                    "node": node,
                    "output": output,
                    "details": details,
                    "qoutput": qoutput,
                    "qdetails": qdetails,
                    "fusion_outputs": []
                }
                if len(fusion_outputs) > 0:
                    stat['fusion_outputs'] = fusion_outputs.copy()
                    fusion_outputs.clear()
                outputs.append(stat)
        return outputs

    @staticmethod
    def _collect_one(out, qrec, quant_compare=False):
        fout = out['output'][0]
        if quant_compare:
            fout = qrec.out_qs[0].dequantize(qrec.out_qs[0].quantize(fout))
        qout = out['qoutput'][0]

        error_ = np.abs(fout - qout)
        node = out['node']

        stat = {
            'name': out['name'],
            'op_name': node.op_name,
            'step': out['step_idx'],
            'av_err': np.mean(error_),
            'max_err': np.max(error_),
            'min_err': np.min(error_),
            'qsnr': qsnr(fout, qout),
            'cos': cos_similarity(fout, qout),
            'chan_err': []
        }

        dim = node.out_dims[0]
        if dim and dim.is_named and dim.has_key('c'):
            channel_error = []
            dim = node.out_dims[0]
            for i in range(dim.c):
                srange = dim.srange(c=i)
                channel_error.append(np.average(fout[srange] - qout[srange]))
            stat['chan_err'] = channel_error

        return stat

    def _collect(self, G, input_tensors, step_idx) -> Mapping[NodeId, Mapping]:
        LOG.debug("gather quantization statistics")
        outputs = self._collect_execution(G,
                                          input_tensors,
                                          G.quantization)
        stats = OrderedDict()
        for out in outputs:
            if out['fusion_outputs']:
                for fout in out['fusion_outputs']:
                    nid = NodeId(out['node'], fout['node'])
                    stats[nid] =\
                        self._collect_one(fout,
                                          G.quantization[nid],
                                          quant_compare=self._quant_compare)
            nid = NodeId(out['node'], None)
            stats[nid] = self._collect_one(out,
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
            for field in ['av_err', 'qsnr', 'chan_err', 'cos']:
                stat[field] = [stat[field]]

        return stats

    def _reduce(self, _, base: Mapping, stat: Mapping):
        for k in ['av_err', 'qsnr', 'chan_err', 'cos']:
            base[k].append(stat[k])
        for k in [('max_err', 'max_err')]:
            base[k[0]] = max(base[k[0]], abs(stat[k[1]]))
        for k in [('min_err', 'min_err')]:
            base[k[0]] = min(base[k[0]], abs(stat[k[1]]))
        for k in [('max_qsnr', 'qsnr'), ('max_cos', 'cos')]:
            base[k[0]] = max(base[k[0]], stat[k[1]])
        for k in [('min_qsnr', 'qsnr'), ('min_cos', 'cos')]:
            base[k[0]] = min(base[k[0]], stat[k[1]])

    @staticmethod
    def _max_abs(l):
        res = (0, 0)
        for el in l:
            ael = math.fabs(el)
            if ael > res[0]:
                res = (ael, el)
        return res[1]

    def _reduce_finalize(self, stats: Mapping) -> Mapping:
        for stat in stats.values():
            for field in ['av_err', 'qsnr', 'cos']:
                stat[field] = sum(stat[field]) / len(stat[field])
            stat['chan_err'] = [sum(i) for i in zip(*stat['chan_err'])]
            stat['max_chan_err'] = self._max_abs(stat['chan_err'])
        return stats
