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

from nntool.graph.types import (Conv2DNode, LinearNode,
                         MultiplicativeBiasNodeBase)
from nntool.utils.stats_funcs import astats, calculate_qsnrs

from .ranges import Ranges
from .stats_collector import StatsCollector

LOG = logging.getLogger(__name__)


def filter_stats(G, pnode, fnode, anode, channel_details=None, qrec=None):
    stats = {}
    if isinstance(anode, MultiplicativeBiasNodeBase):
        if anode.has_mul_bias:
            stats['mul_biases'] = mul_biases = astats(anode.mul_biases)
            mul_biases['qstats'] = calculate_qsnrs(anode.mul_biases,
                                                   mul_biases['ibits'],
                                                   force_ideal=False)
        elif qrec and qrec.ktype.startswith('scaled'):
            stats['mul_biases'] = mul_biases = astats(qrec.mul_biases_fps)
            mul_biases['qstats'] = calculate_qsnrs(qrec.mul_biases_fps,
                                                   mul_biases['ibits'],
                                                   force_ideal=False)
    in_edges = G.indexed_in_edges(pnode.name)
    qweights = in_edges[1].from_node.dqvalue
    qbiases = in_edges[2].from_node.dqvalue

    stats['biases'] = biases = astats(qbiases)
    biases['qstats'] = calculate_qsnrs(qbiases,
                                       biases['ibits'],
                                       force_ideal=False)

    stats['weights'] = weights = astats(
        qweights, channel_dim=anode.filter_dim.get_order_idx('out_c'), channel_details=channel_details)
    weights['qstats'] = calculate_qsnrs(qweights, weights['ibits'],
                                        force_ideal=False)
    # store the statistics into the graph for later use
    anode.stats = stats
    stats['step_idx'] = pnode.step_idx
    return anode.stats


STATS_FUNCTIONS = {
    Conv2DNode: filter_stats,
    LinearNode: filter_stats,
}


class FilterStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G, step_idx):
        stats = {}
        for _, pnode, _, fnode in G.nodes_iterator(True):
            if not self.matches_step(step_idx, pnode, fnode):
                continue
            
            if fnode:
                nid = (pnode.name, fnode.name)
                anode = fnode
            else:
                nid = pnode.name
                anode = pnode
            if G.quantization and G.has_quantized_parameters:
                qrec = G.quantization[nid]
            else:
                qrec = None

            anode = pnode if fnode is None else fnode
            LOG.debug("collecting stats for %s step %s",
                      anode.name, pnode.step_idx)
            if anode.__class__ in STATS_FUNCTIONS:
                stats[nid] = STATS_FUNCTIONS[anode.__class__](
                    G, pnode, fnode, anode, channel_details=step_idx is not None, qrec=qrec)
        return stats


class FilterDetailedStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G, step_idx):
        stats = OrderedDict()
        for step_idx, node, _, fnode in G.nodes_iterator(True):
            if fnode:
                cnode = fnode
                key = (node.name, fnode.name)
            else:
                key = node.name
                cnode = node
            if isinstance(cnode, (LinearNode, Conv2DNode)):
                stats[key] = self.detailed_stat(G, step_idx, cnode)
        return stats

    @classmethod
    def detailed_stat(cls, G, idx, node):
        in_r, max_in_r = Ranges.range_input(G, node)
        out_r, max_out_r = Ranges.range_output(G, node)
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
