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

from graph.types.constant_input import ConstantInputParameters
from quantization.new_qrec import QRec
from utils.node_id import NodeId
from ..nngraph import NNGraph
from ..types import Parameters, SingleInputAndOutput, NNEdge

LOG = logging.getLogger("nntool." + __name__)


def extract_node(G: NNGraph, keep_node: Parameters):
    if not isinstance(keep_node, SingleInputAndOutput):
        raise NotImplementedError(
            "exclude only works with single input and output nodes at present")
    LOG.info("extracting node %s into new graph", keep_node.name)
    in_edges = G.indexed_in_edges(keep_node)
    out_edges = G.indexed_out_edges(keep_node)
    keep_nodes = [edge.from_node for edge in in_edges
                  if edge is not None and isinstance(edge.from_node, ConstantInputParameters)] + [keep_node]
    in_edges = [edge for edge in in_edges if edge is not None and edge.from_node not in keep_nodes]
    nid = NodeId(keep_node)
    in_qrecs = []
    out_qrecs = []
    if G.quantization and nid in G.quantization:
        qrec = G.quantization[nid]
        for edge in in_edges:
            if edge.to_idx >= len(in_qrecs):
                in_qrecs.extend([None] * (edge.to_idx + 1 - len(in_qrecs)))
            in_qrec = G.quantization[nid]
            in_qrecs[edge.to_idx] = QRec.copy_ktype(qrec, out_qs=[qrec.in_qs[edge.to_idx]])
        for idx, _ in enumerate(out_edges):
            out_qrecs.append(QRec.copy_ktype(qrec, in_qs=[qrec.out_qs[edge.from_idx]], out_qs=[qrec.out_qs[edge.from_idx]]))

    for node in G.nodes():
        if node not in keep_nodes and node.name in G:
            LOG.info(f'remove {node.name}')
            G.remove(node)
    for edge in in_edges:
        input_node = G.add_input(edge.from_node.out_dims[edge.from_idx])
        G.add_edge(NNEdge(input_node, keep_node, to_idx=edge.to_idx))
        if in_qrecs:
            G.quantization[NodeId(input_node)] = in_qrecs[edge.to_idx]
    for idx, _ in enumerate(out_edges):
        output_node = G.add_output()
        G.add_edge(NNEdge(keep_node, output_node, from_idx=idx))
        if out_qrecs:
            G.quantization[NodeId(output_node)] = out_qrecs[idx]
    G.add_dimensions()
