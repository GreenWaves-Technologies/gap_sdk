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

from ..nngraph import NNGraph
from ..types import Parameters, SingleInputAndOutput, NNEdge

LOG = logging.getLogger("nntool." + __name__)

def extract_node(G: NNGraph, keep_node: Parameters):
    if not isinstance(keep_node, SingleInputAndOutput):
        raise NotImplementedError("exclude only works with single input and output nodes at present")
    LOG.info("extracting node %s into new graph", keep_node.name)
    for node in list(G.nodes()):
        if node != keep_node:
            G.remove(node)
    G.reset_inout_counts()
    if isinstance(keep_node, SingleInputAndOutput):
        input_node = G.add_input(keep_node.in_dims[0])
        output_node = G.add_output()
        G.add_edge(NNEdge(input_node, keep_node))
        G.add_edge(NNEdge(keep_node, output_node))
        G.add_dimensions()
    else:
        raise NotImplementedError()
