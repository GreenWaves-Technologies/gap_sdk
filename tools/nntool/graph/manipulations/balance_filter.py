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

import numpy as np

from graph.types import MultiplicativeBiasParameters
from utils.stats_funcs import astats

LOG = logging.getLogger("nntool." + __name__)


def balance_filter(pnode, precision_threshold=None, small_value_threshold=0.0000000001, fnode=None, G=None):
    node = pnode if fnode is None else fnode
    channel_dim = node.filter.get_order_idx('out_c')

    stats = astats(node.weights, channel_dim=channel_dim, channel_details=True)
    if precision_threshold:
        if not any(stat['avg_prec'] < precision_threshold for stat in stats['channel_stats']):
            LOG.info("layer %s doesn't have any weights below precision threshold", node.name)
        return False

    LOG.info("balancing weights of layer %s", node.name)
    if node.has_mul_bias and node.mul_biases is not None:
        mul_bias = node.mul_biases
    else:
        mul_bias = np.ones(node.filter.out_c, dtype=np.float32)

    scale = np.array([max(abs(stat['max']), abs(stat['min']))
                      for stat in stats['channel_stats']])
    # don't balance channels that are effectively zero
    threshold_idx = scale < small_value_threshold
    scale[threshold_idx] = 1
    weights_shape = [1 if idx != channel_dim else size
                     for idx, size in enumerate(node.weights.shape)]
    node.weights /= scale.reshape(weights_shape)
    if node.has_bias:
        node.biases /= scale

    mul_bias *= scale
    if G:
        G.changes.modify(pnode, 'has_mul_bias', True, fnode=fnode)
    else:
        node.has_mul_bias = True
    node.mul_biases = mul_bias
    return True


def balance_all_filters(G, precision_threshold=0.20):
    for _, node, _, fnode in G.nodes_iterator(yield_fusions=True):
        anode = node if fnode is None else fnode
        if isinstance(anode, MultiplicativeBiasParameters):
            balance_filter(node, precision_threshold=precision_threshold, fnode=fnode, G=G)
