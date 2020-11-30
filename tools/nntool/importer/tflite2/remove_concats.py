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

from graph.matches.matcher import find_forward
from graph.types import (ConcatParameters, NNEdge, NoOPParameters,
                         StridedSliceParameters)
from utils.node_id import NodeId

LOG = logging.getLogger('nntool.' + __name__)

def remove_concats(G):
    concat_nodes = list([node for node in G.nodes() if isinstance(node, ConcatParameters)])
    strided_slices_removed = []
    concats_removed = []
    for node in concat_nodes:
        concat_out_edges = G.indexed_out_edges(node.name)[0]
        concat_in_edges = G.indexed_in_edges(node.name)
        axis_slices = []
        start_idx = 0
        # find the slice patterns that can match inputs
        for in_idx, dim in enumerate(node.in_dims):
            slice_patterns = [(start_idx, start_idx + dim.shape[node.axis], 1)]
            if dim.shape[node.axis] == 1:
                # can also match reversed
                slice_patterns.append((start_idx, start_idx - 1, -1))
            axis_slices.append(slice_patterns)
            start_idx += dim.shape[node.axis]
        for out_edge in concat_out_edges:
            edge_lists = find_forward(G, out_edge,
                                      StridedSliceParameters,
                                      skip_node_classes=NoOPParameters)
            # each list of edges goes to a strided slice
            for edge_list in edge_lists:
                edge = edge_list[-1]
                assert isinstance(edge.to_node, StridedSliceParameters)
                ssp = edge.to_node
                LOG.info("found strided slice %s", ssp.name)
                # must only slice axis of concat
                if not ssp.only_slices_axis(node.axis):
                    LOG.info("rejected: slices more than one axis")
                    continue
                # must match a slice pattern on the input
                ssp_slice = ssp.act_slice[node.axis]

                in_idx = None
                for idx, slice_patterns in enumerate(axis_slices):
                    if ssp_slice in slice_patterns:
                        in_idx = idx
                        break
                if in_idx is None:
                    LOG.info("rejected: slices pattern matching concat not found")
                    continue
                LOG.info("removing slice %s", ssp.name)
                strided_slices_removed.append(ssp.name)
                # save the out edges
                ssp_out_edges = G.out_edges(ssp.name)
                in_edge = concat_in_edges[in_idx]
                # remove all the nodes including the ssp
                for inter_edge in edge_list:
                    if G.quantization:
                        del G.quantization[NodeId(inter_edge.to_node)]
                    G.remove(inter_edge.to_node)
                # connect all the ssp out edges to the node on the concat input

                for ssp_out_edge in ssp_out_edges:
                    G.add_edge(NNEdge(in_edge.from_node, ssp_out_edge.to_node,
                                      from_idx=in_edge.from_idx,
                                      to_idx=ssp_out_edge.to_idx))
        # if the concat now has no out edges remove it
        if G.num_out_edges(node.name) == 0:
            LOG.info("removing concat %s", node.name)
            concats_removed.append(node.name)
            G.remove(node)

    return (strided_slices_removed, concats_removed)
