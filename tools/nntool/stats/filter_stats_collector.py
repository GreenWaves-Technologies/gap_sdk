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

from graph.types import (Conv2DParameters, FcParameters,
                         MultiplicativeBiasParameters)
from utils.node_id import NodeId
from utils.stats_funcs import astats, calculate_qsnrs
from quantization.multiplicative.mult_quantization import MultScalableFilterQuantizationRecord

from .ranges import Ranges
from .stats_collector import StatsCollector

LOG = logging.getLogger("nntool." + __name__)


def filter_stats(pnode, fnode, anode, channel_details=None, qrec=None):
    stats = {}
    if isinstance(anode, MultiplicativeBiasParameters):
        if anode.has_mul_bias:
            stats['mul_biases'] = mul_biases = astats(anode.mul_biases)
            mul_biases['qstats'] = calculate_qsnrs(anode.mul_biases,
                                                   mul_biases['ibits'],
                                                   force_ideal=False)
        elif isinstance(qrec, MultScalableFilterQuantizationRecord):
            stats['mul_biases'] = mul_biases = astats(qrec.mul_biases_fps)
            mul_biases['qstats'] = calculate_qsnrs(qrec.mul_biases_fps,
                                                   mul_biases['ibits'],
                                                   force_ideal=False)
    if anode.has_bias:
        if qrec:
            qbiases = qrec.prepare_biases(anode, anode.biases, anode.weights, ktype="float32")
        else:
            qbiases = anode.biases

        stats['biases'] = biases = astats(qbiases)
        biases['qstats'] = calculate_qsnrs(qbiases,
                                           biases['ibits'],
                                           force_ideal=False)
    if qrec:
        qweights = qrec.prepare_weights(anode, anode.weights, ktype="float32")
    else:
        qweights = anode.weights

    stats['weights'] = weights = astats(
        qweights, channel_dim=anode.filter.get_order_idx('out_c'), channel_details=channel_details)
    weights['qstats'] = calculate_qsnrs(qweights, weights['ibits'],
                                        force_ideal=False)
    # store the statistics into the graph for later use
    anode.stats = stats
    stats['step_idx'] = pnode.step_idx
    return anode.stats


STATS_FUNCTIONS = {
    Conv2DParameters: filter_stats,
    FcParameters: filter_stats,
}


class FilterStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G, step_idx):
        stats = {}
        for _, pnode, _, fnode in G.nodes_iterator(True):
            if not self.matches_step(step_idx, pnode, fnode):
                continue

            nid = NodeId(pnode, fnode)
            if G.quantization and G.has_quantized_parameters:
                qrec = G.quantization[nid]
            else:
                qrec = None

            anode = pnode if fnode is None else fnode
            LOG.debug("collecting stats for %s step %s", anode.name, pnode.step_idx)
            if anode.__class__ in STATS_FUNCTIONS:
                stats[nid] = STATS_FUNCTIONS[anode.__class__](
                    pnode, fnode, anode, channel_details=step_idx is not None, qrec=qrec)
        return stats


class FilterDetailedStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G, step_idx):
        stats = OrderedDict()
        for step_idx, node, _, fnode in G.nodes_iterator(True):
            key = NodeId(node, fnode)
            node = fnode or node
            if isinstance(node, (FcParameters, Conv2DParameters)):
                stats[key] = self.detailed_stat(step_idx, node)
        return stats

    @classmethod
    def detailed_stat(cls, idx, node):
        in_r, max_in_r = Ranges.range_input(node)
        out_r, max_out_r = Ranges.range_output(node)
        in_p = [in_elem/max_in_r for in_elem in in_r]
        out_p = [out_elem/max_out_r for out_elem in out_r]
        return [
            idx,
            node.name,
            min(in_r),
            max(in_r),
            min(out_r),
            max(out_r),
            min(in_p),
            sum(in_p)/len(in_p),
            min(out_p),
            sum(out_p)/len(out_p),
        ]
