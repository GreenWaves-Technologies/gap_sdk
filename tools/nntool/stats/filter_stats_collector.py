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

from .ranges import Ranges
from .stats_collector import StatsCollector

LOG = logging.getLogger("nntool." + __name__)

def filter_stats(pnode, fnode, anode, channel_details=None):
    stats = {}
    if isinstance(anode, MultiplicativeBiasParameters) and anode.has_mul_bias:
        stats['mul_biases'] = mul_biases = astats(anode.mul_biases)
        mul_biases['qstats'] = calculate_qsnrs(anode.mul_biases,
                                               mul_biases['ibits'],
                                               force_ideal=False)
    if anode.has_bias:
        stats['biases'] = biases = astats(anode.biases)
        biases['qstats'] = calculate_qsnrs(anode.biases,
                                           biases['ibits'],
                                           force_ideal=False)
    stats['weights'] = weights = astats(
        anode.weights, channel_dim=anode.filter.get_order_idx('out_c'), channel_details=channel_details)
    weights['qstats'] = calculate_qsnrs(anode.weights, weights['ibits'],
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

            key = NodeId(pnode, fnode)
            anode = pnode if fnode is None else fnode
            LOG.debug("collecting stats for %s step %s", anode.name, pnode.step_idx)
            if anode.__class__ in STATS_FUNCTIONS:
                stats[key] = STATS_FUNCTIONS[anode.__class__](pnode, fnode, anode, channel_details=step_idx is not None)
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
