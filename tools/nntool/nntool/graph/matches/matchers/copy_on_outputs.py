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

from nntool.graph.types import CopyNode, NNEdge, OutputNode
from nntool.utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name,
                       modifies_dimensions, run_before)

LOG = logging.getLogger(__name__)


def search_down(G, edge):
    node = edge.to_node
    if node.exclude_from_generation:
        res = []
        for out_edge in G.out_edges(node):
            res.extend(search_down(G, out_edge))
        return res
    elif isinstance(node, OutputNode):
        return [node]
    else:
        return []


def search_up(G, edge):
    node = edge.from_node
    out_edges = G.indexed_out_edges(node)[edge.from_idx]
    res = []
    if len(out_edges) > 1:
        for out_edge in out_edges:
            if out_edge == edge:
                continue
            res.extend(search_down(G, out_edge))
    if node.exclude_from_generation:
        edge = G.in_edges(node)[0]
        res.extend(search_up(G, edge))
    return res


@match_name("copy_on_outputs")
@description("Insert copy on multiple edges from same node output to graph outputs")
@modifies_dimensions(True)
@groups('*')
@run_before('*')
class CopyOnOutputs(Matcher):

    def _match(self, G: GraphView, **kwargs):
        outputs = G.nodes(node_classes=OutputNode)
        results = []
        while outputs:
            outp = outputs.pop(0)
            other_outputs = search_up(G, G.in_edges(outp)[0])
            for other_outp in other_outputs:
                if other_outp in outputs:
                    outputs.remove(other_outp)
            results.extend(other_outputs)
        for output in results:
            LOG.info(
                "Insert copy before output %s", output.name)
            copy_node = CopyNode(G.unique_name(f'{output.name}_copy'))
            G.insert_node_before(copy_node, output, edge_class=NNEdge)
            nid = output.name
            if G.quantization and nid in G.quantization:
                G.quantization.copy_qrec(output, 'in', 0, copy_node)

        return bool(results)
