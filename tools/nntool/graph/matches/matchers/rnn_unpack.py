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
from copy import deepcopy
from struct import unpack

from graph.dim import Dim
from graph.types import (ConcatParameters, NNEdge, ReshapeParameters,
                         RNNBaseParameters, SplitParameters,
                         StridedSliceParameters, TransposeParameters)
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, groups, match_name, description, needs_valid_dimension

LOG = logging.getLogger("nntool." + __name__)

@groups('scaled')
@match_name("rnn_unpack")
@description('Merge unpack/slice of last output into RNN')
@needs_valid_dimension(True)
class MatchRnnUnpack(Matcher):
    NAME = 'rnn_unpack'
    DESCRIPTION = 'Merge unpack/slice of last output into RNN'
    NEEDS_VALID_DIMENSION = True

    def find_unpack(self, G, node, first=True, out_shape=None, in_idx=None):
        if isinstance(node, RNNBaseParameters):
            if not first:
                return []
            out_shape = [(dim, (0, dim)) for dim in node.out_dims[0].shape]
            if len(out_shape) == 3:
                out_shape = list(zip(('d', 't', 'c'), out_shape))
            else:
                out_shape = list(zip(('t', 'c'), out_shape))
        elif isinstance(node, (StridedSliceParameters, SplitParameters)):
            return [(node, out_shape)]
        elif isinstance(node, TransposeParameters):
            if node.transpose:
                out_shape = [out_shape[idx] for idx in node.transpose]
        elif isinstance(node, ConcatParameters):
            before = sum(dim.shape[node.axis]
                         for dim in node.in_dims[:in_idx:])
            total = sum(dim.shape[node.axis] for dim in node.in_dims)
            in_shape = out_shape[node.axis]
            out_shape[node.axis] = (
                in_shape[0], (total, (in_shape[1][1][0] + before, in_shape[1][1][1] + before)))
        elif isinstance(node, ReshapeParameters):
            # if there is no direction dimension then reshape doesn't work
            if len(out_shape) != 3:
                return []
            if len(node.shape) != 2:
                return []
            if node.shape[0] == node.old_shape[0] * node.old_shape[1]:
                compressed_dims = (0, 1)
            elif node.shape[1] == node.old_shape[1] * node.old_shape[2]:
                compressed_dims = (1, 2)
            else:
                return []
            d_axis = next(
                iter([idx for idx, elem in enumerate(out_shape) if elem[0] == 'd']))
            if d_axis not in compressed_dims:
                return []
            new_shape = []
            for idx, in_shape in enumerate(out_shape):
                if idx in compressed_dims:
                    if idx == d_axis:
                        continue
                    # this is the axis that has been reshaped. The offset of this
                    # path will be the size of this axis before reshape multiplied by the
                    # indexes of the removed axis
                    new_shape.append(
                        (
                            in_shape[0],
                            (
                                in_shape[1][0] * out_shape[d_axis][1][0],
                                tuple(
                                    [in_shape[1][0] * d_idx for d_idx in out_shape[d_axis][1][1]])
                            )
                        )
                    )
                else:
                    new_shape.append(in_shape)
            out_shape = new_shape
        else:
            return []
        edges = G.out_edges(node.name)
        if len(edges) > 1:
            return []
        rest = self.find_unpack(G, edges[0].to_node, first=False,
                                out_shape=deepcopy(out_shape), in_idx=edges[0].to_idx)
        if rest:
            return [(node, out_shape)] + rest
        return []

    def validate_slices(self, G, rnn_nodes):
        """Check that the slice or split at the end of the string of nodes is valid for
        all found paths from rnn node to split"""
        res = {}
        invalid_unpacks = set()
        for rnn_unpack in rnn_nodes:
            if not rnn_unpack:
                continue
            unpack_node, unpack_dim = rnn_unpack[-1]
            rnn_node, _ = rnn_unpack[0]
            time_axis = next(
                iter([idx for idx, elem in enumerate(unpack_dim) if elem[0] == 't']))
            if isinstance(unpack_node, StridedSliceParameters):
                if not unpack_node.only_slices(time_axis):
                    LOG.debug(
                        "can't remove %s. Slice operates on non timestep axis", unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                if unpack_node.act_slice[time_axis][1] != rnn_node.n_cells:
                    LOG.debug(
                        "can't remove %s. Slice not equal to cells", unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                if unpack_node.act_slice[time_axis][2] != 1:
                    LOG.debug("can't remove %s. Slice not of length 1",
                              unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                if unpack_node.act_slice[time_axis][0] != rnn_node.n_cells - 1:
                    LOG.debug("can't remove %s. Slice isn't last cell",
                              unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                res.setdefault(unpack_node, []).append(rnn_unpack)
            elif isinstance(unpack_node, SplitParameters):
                out_edges = G.out_edges(unpack_node.name)
                if len(out_edges) > 1:
                    LOG.debug(
                        "can't remove %s. More than one output edge", unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                out_edge = out_edges[0]
                if out_edge.from_idx != len(unpack_node.act_slices) - 1:
                    LOG.debug("can't remove %s. Not last output",
                              unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                act_slice = unpack_node.act_slices[-1]
                if act_slice[time_axis][1] != rnn_node.n_cells:
                    LOG.debug(
                        "can't remove %s. Slice not equal to cells", unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                if act_slice[time_axis][0] != rnn_node.n_cells - 1:
                    LOG.debug("can't remove %s. Slice isn't last cell",
                              unpack_node.name)
                    invalid_unpacks.add(unpack_node)
                    continue
                res.setdefault(unpack_node, []).append(rnn_unpack)
            else:
                invalid_unpacks.add(unpack_node)
                continue
        for rnn_unpack in invalid_unpacks:
            if rnn_unpack in res:
                del res[rnn_unpack]
        return res

    def all_paths_lead_to_rnn(self, G, node, first=True):
        if isinstance(node, (StridedSliceParameters, SplitParameters)):
            if not first:
                return False
        elif isinstance(node, RNNBaseParameters):
            # verify that this node actually can be modified to output one time slice
            # I guess there could be graphs where one node is only outputting one timeslice
            # but I can't imagine a topology that would work with just reshapes, transpose and
            # concat in between so do not consider it
            return node.n_output_cells > 1
        elif not isinstance(node, (TransposeParameters, ConcatParameters, ReshapeParameters)):
            return False

        return all(self.all_paths_lead_to_rnn(G, edge.from_node, first=False) for edge in G.in_edges(node.name))

    def validate_multi_branch(self, G, rnn_nodes_by_slice):
        """Check that every path from the split or slice leads back to an RNN node that we can modify"""
        for unpack_node in list(rnn_nodes_by_slice):
            if not self.all_paths_lead_to_rnn(G, unpack_node):
                del rnn_nodes_by_slice[unpack_node]
        return rnn_nodes_by_slice

    @staticmethod
    def remove_node(G, node):
        LOG.info("%s", f"removing {node.name}")
        in_edge = G.in_edges(node.name)[0]
        out_edges = G.out_edges(node.name)
        G.remove(node)
        if G.quantization:
            del G.quantization[NodeId(node)]
        for out_edge in out_edges:
            G.add_edge(NNEdge(from_node=in_edge.from_node, to_node=out_edge.to_node,
                              from_idx=in_edge.from_idx, to_idx=out_edge.to_idx))

    def process_path(self, G, rnn_unpack, modified_nodes):
        original_timesteps = None
        last_dim = None
        for node, node_dim in rnn_unpack[:-1:]:
            if node in modified_nodes:
                return
            elif isinstance(node, RNNBaseParameters):
                LOG.info(
                    "%s", f"setting {node.name} to single timestep output")
                original_timesteps = node.n_output_cells
                node.n_output_cells = 1
                # hint is unchanged since the timestep axis is still outputted
            elif isinstance(node, ConcatParameters):
                # setting the time axis to 1 does not change the concat
                LOG.info(
                    "%s", f"passing concat {node.name} on axis {node.axis}")
            elif isinstance(node, ReshapeParameters):
                # divide the shape of the time axis by the original number of timesteps
                in_t_axis = next(
                    iter([idx for idx, dim in enumerate(last_dim) if dim[0] == 't']))
                in_shape = node.old_shape.shape
                out_t_axis = next(
                    iter([idx for idx, dim in enumerate(node_dim) if dim[0] == 't']))
                out_shape = node.shape.shape
                in_shape[in_t_axis] //= original_timesteps
                out_shape[out_t_axis] //= original_timesteps
                if in_shape == out_shape:
                    self.remove_node(G, node)
                else:
                    node.old_shape = Dim.unnamed(in_shape)
                    node.shape = Dim.unnamed(out_shape)
            elif isinstance(node, TransposeParameters):
                remove_node = False
                if not node.transpose:
                    remove_node = True
                else:
                    t_axis = next(
                        iter([idx for idx, dim in enumerate(node_dim) if dim[0] == 't']))
                    new_transpose = [
                        axis - 1 if axis > t_axis else axis for axis in node.transpose if axis != t_axis]
                    if new_transpose == sorted(new_transpose):
                        remove_node = True
                    else:
                        LOG.info(
                            "%s", f"setting {node.name} transpose to {new_transpose}")
                        node.transpose = new_transpose

                if remove_node:
                    self.remove_node(G, node)
            else:
                raise ValueError('unexpected node in sequence')
            last_dim = node_dim
            modified_nodes.add(node)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        rnn_nodes = [self.find_unpack(G, node)
                     for node in G.nodes()
                     if isinstance(node, RNNBaseParameters) and node.n_output_cells > 1]
        rnn_nodes_by_slice = self.validate_slices(G, rnn_nodes)
        rnn_nodes_by_slice = self.validate_multi_branch(G, rnn_nodes_by_slice)
        if not rnn_nodes_by_slice:
            return False

        for unpack_node, rnn_unpacks in rnn_nodes_by_slice.items():
            modified_nodes = set()
            for rnn_unpack in rnn_unpacks:
                self.process_path(G, rnn_unpack, modified_nodes)
            # since process path will have removed all unnecessary nodes the edges will be correct here
            out_edges = G.out_edges(unpack_node.name)
            in_edges = G.in_edges(unpack_node.name)
            assert len(
                in_edges) == 1, "expecting unpack node to have only one in edge"
            in_edge = in_edges[0]
            changes_shape = unpack_node.changes_shape if isinstance(
                unpack_node, StridedSliceParameters) else False

            LOG.info("Eliminating last cell unpack: %s", unpack_node.name)
            G.remove(unpack_node)

            # Here the strided slice can change the output shape of the RNN
            # so insert a reshape to do the shape change
            if changes_shape:
                reshape = ReshapeParameters(unpack_node.name + '_reshape',
                                            old_shape=Dim.unnamed(
                                                unpack_node.slice_shape),
                                            shape=Dim.unnamed(unpack_node.out_shape))
                G.add_edge(NNEdge(from_node=in_edge.from_node,
                                  to_node=reshape, from_idx=in_edge.from_idx))
                for out_edge in out_edges:
                    G.add_edge(NNEdge(from_node=reshape,
                                      to_node=out_edge.to_node, to_idx=out_edge.to_idx))
                if G.quantization:
                    G.quantization[NodeId(
                        reshape)] = G.quantization[NodeId(unpack)]
            else:
                for out_edge in out_edges:
                    G.add_edge(NNEdge(from_node=in_edge.from_node, to_node=out_edge.to_node,
                                      from_idx=in_edge.from_idx, to_idx=out_edge.to_idx))
            if G.quantization:
                del G.quantization[NodeId(unpack_node)]

        if set_identity:
            self.set_identity(G)

        return True
