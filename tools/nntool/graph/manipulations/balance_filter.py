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

import numpy as np
import math

from utils.stats_funcs import astats, range_twos_complement

def balance_filter(node):
    channel_dim = node.filter.get_order_idx('out_c')
    weights = astats(node.weights, channel_dim=channel_dim)
    av_ibits = math.ceil(sum([stat['ibits'] for stat in weights['channel_stats']]) / len(weights['channel_stats']))
    if node.has_mul_bias and node.mul_biases is not None:
        mul_bias = node.mul_biases
    else:
        mul_bias = np.ones(node.filter.out_c, dtype=np.float32)
    (_, rng_pos) = range_twos_complement(av_ibits)
    for channel_idx, channel_stat in enumerate(weights['channel_stats']):
        if channel_stat['max'] > 0:
            max_v = math.floor(channel_stat['max'])
            min_v = 0
            abs_min_v = 0
        else:
            max_v = 0
        if channel_stat['min'] < 0:
            min_v = math.floor(channel_stat['min'])
            abs_min_v = abs(min_v) - 1
        abs_max = max(max_v, abs_min_v, 1)

        scale = abs_max/rng_pos
        node.weights[node.filter.srange(out_c=channel_idx)] /= scale
        if node.has_bias:
            node.biases[channel_idx] /= scale
        mul_bias[channel_idx] *= scale
    node.mul_biases = mul_bias
    node.has_mul_bias = True
