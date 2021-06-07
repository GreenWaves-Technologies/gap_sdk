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
from bfloat16 import bfloat16
from graph.matches.matchers.remove_unnecessary_quantize_operators import \
    RemoveUnnecessaryQuantizeOperators
from graph.types.fusions import FusionBase

from quantization.unified_quantizer import UnifiedQuantizer

LOG = logging.getLogger('nntool.' + __name__)

FLOAT_DTYPES = {
    'float16': np.float16,
    'float32': np.float32,
    'bfloat16': bfloat16,
}

POW2_DTYPES = {
    'int16': np.int16,
    'int8': np.int8,
}


class TuneError(Exception):
    pass

# set dtype
# set bits
# set scheme
# out or in and out
# out forces out
# in forces all outs above
# forces are stored with quantization


def tune_options(G, nodes, options):
    all_nodes = get_nodes_and_fusion_nodes(nodes)
    force_options = {node: options for node in all_nodes}
    quantizer = UnifiedQuantizer.from_quantized_graph(G)
    quantizer.quantize(
        G, start_nodes=nodes, force_options=force_options)
    RemoveUnnecessaryQuantizeOperators().match(G)
    G.add_dimensions()


def tune_float(G, nodes, float_type):
    all_nodes = get_nodes_and_fusion_nodes(nodes)
    force_scheme = {node: 'float' for node in all_nodes}
    force_options = {node: {'float_type': float_type} for node in all_nodes}
    quantizer = UnifiedQuantizer.from_quantized_graph(G, extra_schemes=['float'])
    quantizer.quantize(
        G, start_nodes=nodes, force_scheme=force_scheme, force_options=force_options)
    RemoveUnnecessaryQuantizeOperators().match(G)
    G.add_dimensions()


def get_nodes_and_fusion_nodes(nodes):
    all_nodes = []
    for node in nodes:
        all_nodes.append(node)
        if isinstance(node, FusionBase) and node.quantize_internals:
            all_nodes.extend(node.subgraph.nodes())
    return all_nodes


def tune_scaled(G, nodes):
    all_nodes = get_nodes_and_fusion_nodes(nodes)
    force_scheme = {node: 'SQ8' for node in all_nodes}
    quantizer = UnifiedQuantizer.from_quantized_graph(G, extra_schemes=['SQ8'])
    quantizer.quantize(G, start_nodes=nodes, force_scheme=force_scheme)
    RemoveUnnecessaryQuantizeOperators().match(G)
    G.add_dimensions()


def tune_pow2(G, nodes, pow2_type):
    all_nodes = get_nodes_and_fusion_nodes(nodes)
    force_scheme = {node: 'POW2' for node in all_nodes}
    force_options = {node: {'bits': 16 if pow2_type == 'int16' else 8}
                     for node in all_nodes}
    quantizer = UnifiedQuantizer.from_quantized_graph(G, extra_schemes=['POW2'])
    quantizer.quantize(
        G, start_nodes=nodes, force_scheme=force_scheme, force_options=force_options)
    RemoveUnnecessaryQuantizeOperators().match(G)
    G.add_dimensions()
