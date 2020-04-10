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

from graph.nngraph import NNGraph
from graph.types import ActivationParameters, ConvFusionParameters
from utils.node_id import NodeId
from utils.stats_funcs import calc_bits

LOG = logging.getLogger('nntool.' + __name__)


def tune_relu(G, node, astats, threshold, fnode=None):
    act_node = fnode if fnode is not None else node
    relu_n = 6 if act_node.activation == "relu6" else act_node.activation_params
    calc_relu_n = math.floor(astats['max']) + threshold
    # if the calculated relu n is the same or the amount of bits to represent the relu value
    # is the same then don't change
    if calc_relu_n >= relu_n or calc_bits(calc_relu_n) >= calc_bits(relu_n):
        return
    
    if calc_relu_n == 6:
        G.changes.modify(node, 'activation', 'relu6', fnode=fnode)
        G.changes.modify(node, 'activation_params', None, fnode=fnode)
    else:
        G.changes.modify(node, 'activation', 'relun', fnode=fnode)
        G.changes.modify(node, 'activation_params', calc_relu_n, fnode=fnode)


def adjust_relun(G: NNGraph, activation_stats, threshold=1):
    for node in [step['node'] for step in G.graph_state.steps]:
        if isinstance(node, ConvFusionParameters):
            for fnode in node.contained_nodes():
                if isinstance(fnode, ActivationParameters) and \
                        (fnode.activation == "relu6" or fnode.activation == "relun"):
                    tune_relu(G, node, activation_stats[NodeId(
                        node, fnode)], threshold, fnode=fnode)
        elif isinstance(node, ActivationParameters) and (node.activation == "relu6" or node.activation == "relun"):
            tune_relu(G, node, activation_stats[NodeId(node)], threshold)
