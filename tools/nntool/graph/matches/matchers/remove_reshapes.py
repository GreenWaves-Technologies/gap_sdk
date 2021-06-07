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

from graph.types.others import TransposeParameters
import logging

from graph.dim import Dim
from graph.types import NNEdge, ReshapeParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, match_name, description, run_before, groups, needs_valid_dimension

LOG = logging.getLogger("nntool." + __name__)

@match_name("remove_reshapes")
@description("Remove double reshapes")
@run_before('*')
@groups('*')
@needs_valid_dimension(True)
class RemoveReshapes(Matcher):

    @staticmethod
    def validate_reshape(G, reshape):
        if reshape.transpose_out:
            return False
        out_shape = None
        candidates = []
        out_edges = G.out_edges(reshape.name)
        if len(out_edges) > 1:
            return False
        edge = out_edges[0]
        if not isinstance(edge.to_node, (ReshapeParameters, TransposeParameters)):
            return False
        candidate = edge.to_node
        if isinstance(candidate, TransposeParameters):
            if not candidate.does_nothing():
                return False
            out_shape = tuple(candidate.out_dims[0].shape)
        else:
            if candidate.transpose_out or candidate.transpose_in:
                return False
            if out_shape is not None:
                if out_shape != tuple(candidate.shape.shape):
                    return False
            else:
                out_shape = tuple(candidate.shape.shape)
        candidates.append(candidate)
        return (reshape, candidates, out_shape)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        modified_graph = True
        while modified_graph:
            modified_graph = False
            for reshape in G.nodes(node_classes=(ReshapeParameters,)):
                if not reshape.has_transpose and reshape.shape.shape == reshape.old_shape.shape:
                    modified_graph = True
                    LOG.info('removing reshape that does nothing %s', reshape.name)
                    G.remove_and_reconnect(reshape, edge_class=NNEdge)
                    nid = NodeId(reshape)
                    if G.quantization and nid in G.quantization:
                        del G.quantization[nid]
            res = None
            for reshape in G.nodes(node_classes=(ReshapeParameters,)):
                res = self.validate_reshape(G, reshape)
                if res:
                    LOG.info('unnecessary reshape found after %s', reshape.name)
                    modified_graph = True
                    (reshape, candidates, out_shape) = res
                    for candidate in candidates:
                        LOG.info(
                            'removing unnecessary reshape or transpose %s', candidate.name)
                        edges = G.out_edges(candidate.name)
                        G.remove(candidate)
                        nid = NodeId(candidate)
                        if G.quantization and nid in G.quantization:
                            del G.quantization[nid]
                        for edge in edges:
                            G.add_edge(NNEdge(from_node=reshape,
                                            to_node=edge.to_node, to_idx=edge.to_idx))
                    reshape.shape = Dim.unnamed(out_shape)
                    break

        if set_identity:
            self.set_identity(G)

        return modified_graph
