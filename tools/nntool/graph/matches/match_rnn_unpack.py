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

from graph.types import (NNEdge, ReshapeParameters, RNNBaseParameters,
                         SplitParameters, StridedSliceParameters)
from utils.graph import Edge, GraphView
from utils.node_id import NodeId

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


class MatchRnnUnpack(Matcher):
    NAME = 'rnn_unpack'
    DESCRIPTION = 'Merge unpack/slice of last output into RNN'
    NEEDS_VALID_DIMENSION = True

    def find_unpack(self, G, node, first=True):
        if isinstance(node, (StridedSliceParameters, SplitParameters)):
            return [node]
        if not isinstance(node, (ReshapeParameters, RNNBaseParameters)):
            return []
        if not first and isinstance(node, RNNBaseParameters):
            return []
        edges = G.out_edges(node.name)
        if len(edges) > 1:
            return []
        rest = self.find_unpack(G, edges[0].to_node, first=False)
        if rest:
            return [node] + rest
        return []

    def match(self, G: GraphView, set_identity: bool = True):
        rnn_nodes = [self.find_unpack(G, node) for node in G.nodes() if isinstance(node, RNNBaseParameters)]
        has_modified_graph = False
        for rnn_unpack in rnn_nodes:
            if not rnn_unpack:
                continue
            unpack_node = rnn_unpack[-1]
            rnn_node = rnn_unpack[0]
            time_axis = rnn_node.transpose_out[0].index(0) if rnn_node.transpose_out else 0
            if isinstance(unpack_node, StridedSliceParameters):
                if unpack_node.act_slice[time_axis][1] != rnn_node.n_cells:
                    LOG.debug("can't remove %s. Slice not equal to cells", unpack_node.name)
                    continue
                if unpack_node.act_slice[time_axis][2] != 1:
                    LOG.debug("can't remove %s. Slice not of length 1", unpack_node.name)
                    continue
                if unpack_node.act_slice[time_axis][0] != rnn_node.n_cells - 1:
                    LOG.debug("can't remove %s. Slice isn't last cell", unpack_node.name)
                    continue
                out_edge = G.out_edges(unpack_node.name)[0]
            elif isinstance(unpack_node, SplitParameters):
                out_edges = G.out_edges(unpack_node.name)
                if len(out_edges) > 1:
                    LOG.debug("can't remove %s. More than one output edge", unpack_node.name)
                    continue
                out_edge = out_edges[0]
                if out_edge.from_idx != len(unpack_node.act_slices) - 1:
                    LOG.debug("can't remove %s. Not last output", unpack_node.name)
                    continue
                act_slice = unpack_node.act_slices[-1]
                if act_slice[time_axis][1] != rnn_node.n_cells:
                    LOG.debug("can't remove %s. Slice not equal to cells", unpack_node.name)
                    continue
                if act_slice[time_axis][0] != rnn_node.n_cells - 1:
                    LOG.debug("can't remove %s. Slice isn't last cell", unpack_node.name)
                    continue
                out_edge = G.out_edges(unpack_node.name)[0]
            else:
                continue

            has_modified_graph = True
            for node in rnn_unpack[1::]:
                LOG.info("Eliminating last cell unpack: %s", node.name)
                if G.quantization:
                    del G.quantization[NodeId(node)]
                G.remove(node)
            rnn_node.n_output_cells = 1
            rnn_node.out_dims[0] = unpack_node.out_dims[out_edge.from_idx]
            rnn_node.out_dims_hint = [unpack_node.out_dims_hint[out_edge.from_idx]]
            rnn_node.transpose_out = None
            G.add_edge(NNEdge(rnn_node, out_edge.to_node, to_idx=out_edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
