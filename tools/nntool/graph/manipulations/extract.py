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

import logging

from utils.graph import Edge

from ..nngraph import NNGraph
from ..types import Parameters, SingleInputAndOutput

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
        G.add_edge(Edge(input_node, keep_node))
        G.add_edge(Edge(keep_node, output_node))
        G.add_dimensions()
    else:
        raise NotImplementedError()
