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

from collections import OrderedDict

from graph.types import Conv2DParameters, FcParameters
from utils.node_id import NodeId
from utils.stats_funcs import STATS_BITS, bits

from .ranges import Ranges
from .stats_collector import StatsCollector

def astats(size, do_bits=True):
    """Extracts statistics from a tensor
    """
    ret = {
        'mean': 0,
        'std': 0.25,
        'min': -0.9,
        'max': 0.9,
        'size': size,
        'wols': 0,
        'sols': 0,
        'min_out' : 0,
        'max_out' : 0,
    }
    if do_bits:
        ret['ibits'] = bits(0.9, -0.9)
    return ret

def calculate_qsnrs(ideal_ibits):
    qstats = {}
    for bit_size in STATS_BITS:
        frac_bits = max(bit_size - ideal_ibits, 0)
        qstats[bit_size] = {'q': frac_bits, 'qsnr': 0}
    return qstats

def filter_stats(node):
    stats = {}
    if node.has_bias:
        stats['biases'] = biases = astats(node.filter.out_c)
        biases['qstats'] = calculate_qsnrs(biases['ibits'])
    filter_size = node.filter.size()
    if isinstance(node, Conv2DParameters):
        filter_size /= node.groups
    stats['weights'] = weights = astats(filter_size)
    weights['qstats'] = calculate_qsnrs(weights['ibits'])
    # store the statistics into the graph for later use
    node.stats = stats
    return node.stats

STATS_FUNCTIONS = {
    Conv2DParameters: filter_stats,
    FcParameters: filter_stats,
}

class FakeFilterStatsCollector(StatsCollector):
    def _prepare(self, G):
        pass

    def _collect(self, G):
        stats = {}
        for _, node, _, fnode in G.nodes_iterator(True):
            key = NodeId(node, fnode)
            node = fnode or node
            if node.__class__ in STATS_FUNCTIONS:
                stats[key] = STATS_FUNCTIONS[node.__class__](node)
        return stats
