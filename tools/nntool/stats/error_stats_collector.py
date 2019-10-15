# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
from collections import OrderedDict
from typing import Mapping

import numpy as np

from utils.stats_funcs import qsnr
from utils.node_id import NodeId

from execution.execute_graph import execute
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

    def _collect(self, G, input_tensors) -> Mapping[NodeId, Mapping]:
        LOG.debug("gather quantization statistics")
        output_ = execute(G, input_tensors, limit=self._limit)
        all_details = []
        qoutput_ = execute(G, input_tensors, limit=self._limit,
                           qrecs=G.quantization, qmode=QuantizationMode.all(),
                           all_details=all_details)
        stats = OrderedDict()
        for idx, out in enumerate(output_):
            error_ = np.abs(out[0] - qoutput_[idx][0])
            step = G.graph_state.steps[idx]
            node = step['node']
            details = all_details[idx]
            if details:
                overflow_dot = details['overflow_dot']
                overflow_acc = details['overflow_acc']
            else:
                overflow_dot = overflow_acc = ""

            stats[NodeId(node, None)] = {
                'name': node.name,
                'op_name': node.op_name,
                'step': idx,
                'av_err': np.mean(error_),
                'max_err': np.max(error_),
                'min_err': np.min(error_),
                'qsnr': qsnr(out[0], qoutput_[idx][0]),
                'overflow_dot' : overflow_dot,
                'overflow_acc' : overflow_acc,
            }

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
