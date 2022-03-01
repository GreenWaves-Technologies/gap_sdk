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

from graph.types import (ActivationParameters, MatrixAddParameters,
                         PaddedAddFusionParameters, PadParameters)
from graph.types.base import NNEdge
from quantization.new_qrec import QRec
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, match_name, description, groups, run_before, run_qtune_on_match

LOG = logging.getLogger("nntool." + __name__)


class FusionMatch():
    def __init__(self) -> None:
        self.pad = None
        self.add = None
        self.active = None
        self.order = []

    def add_node(self, params):
        if isinstance(params, PadParameters):
            if self.pad:
                return None
            self.order.append(params)
            self.pad = params
            return self
        elif isinstance(params, MatrixAddParameters):
            if self.add:
                return None
            self.order.append(params)
            self.add = params
            return self
        elif isinstance(params, ActivationParameters):
            if self.active:
                return None
            self.order.append(params)
            self.active = params
            return self
        else:
            return None

    @property
    def fusion_type(self):
        return '_'.join(['pad' if isinstance(params, PadParameters)
                         else 'active' if isinstance(params, ActivationParameters)
                         else 'add' for params in self.order])

@match_name('fuse_padded_add')
@description('Fuse convolutions, pools and activations to match GAP AutoTiler operations')
@run_before('fuse_op_activation_scale8')
@run_qtune_on_match
@groups('scaled')
class MatchPadAddAct(Matcher):

    def get_node_list(self, G, params, result=None):
        if result is None:
            result = FusionMatch()
        if not result.add_node(params):
            return result
        out_edges = G.out_edges(params.name)
        if len(out_edges) > 1:
            return result
        return self.get_node_list(G, out_edges[0].to_node, result=result)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        for pad_node in [params for params in G.nodes() if isinstance(params, PadParameters)]:
            node_list = self.get_node_list(G, pad_node)
            if node_list is None or len(node_list.order) < 2:
                continue
            LOG.info("fusing nodes %s", ",".join(
                (node.name for node in node_list.order)))
            has_modified_graph = True
            subgraph = GraphView()
            padded_input_idx = G.out_edges(node_list.pad.name)[0].to_idx
            subgraph.add_edge(NNEdge(from_node=node_list.pad,
                                     to_node=node_list.add, to_idx=padded_input_idx))
            last_node = node_list.add
            node_list.add.force_quantized_index = 0
            if node_list.active:
                subgraph.add_edge(
                    NNEdge(from_node=node_list.add, to_node=node_list.active))
                last_node = node_list.active
            if padded_input_idx == 0:
                input_mapping = [[(node_list.pad, 0)], [(node_list.add, 1)]]
            else:
                input_mapping = [[(node_list.add, 0)], [(node_list.pad, 1)]]

            output_mapping = [(last_node, 0)]
            pnode = PaddedAddFusionParameters(
                "PADDED_" + node_list.add.name,
                fusion_type=node_list.fusion_type,
                subgraph=subgraph,
                input_mapping=input_mapping,
                output_mapping=output_mapping)
            if G.quantization:
                for node in pnode.contained_nodes():
                    G.quantization.move_to_fusion(node, pnode)
            if padded_input_idx == 0:
                in_edges = G.in_edges(node_list.pad.name) + \
                    G.indexed_in_edges(node_list.add.name)[1::]
            else:
                in_edges = G.indexed_in_edges(node_list.add.name)[
                    0:1:] + G.in_edges(node_list.pad.name)
            out_edges = G.out_edges(last_node.name)
            for node in node_list.order:
                G.remove(node)
            for edge in in_edges:
                G.add_edge(NNEdge(edge.from_node, pnode,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx))
            for edge in out_edges:
                G.add_edge(NNEdge(pnode, edge.to_node,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
