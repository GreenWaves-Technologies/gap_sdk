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

from graph.types import NNEdge, SplitParameters, StridedSliceParameters
from graph.types.input_output import OutputParameters
from quantization.unified_quantizer import UnifiedQuantizer
from utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before
from .remove_unnecessary_quantize_operators import \
    RemoveUnnecessaryQuantizeOperators

LOG = logging.getLogger("nntool." + __name__)

@match_name("slice_to_split")
@description("collects slices from a single node and converts to a single split")
@run_before('unused_concats')
@groups('*')
class SliceToSplitMatch(Matcher):
    @staticmethod
    def slice_to_split(G, slice_nodes, slices):
        slice_node = slice_nodes[0]
        in_dims = slice_node.in_dims[0].shape
        slices = [sl[0] for sl in slices]
        if any(sl[2] != 1 for sl in slices):
            return
        szes = tuple([sl[1] - sl[0] for sl in slices])
        # find sliced axes that differ
        diff_axis = tuple(idx for idx, (d1, d2) in enumerate(
            zip(szes, in_dims)) if d1 != d2)
        if len(diff_axis) != 1:
            return
        # good to convert to a split
        axis = diff_axis[0]
        axis_slice = slices[axis]
        axis_dim = in_dims[axis]
        outs = []
        splits = []
        if axis_slice[0] > 0:
            splits.append(axis_slice[0])
            oparams = OutputParameters(G.unique_name('unused'))
            oparams.at_options.allocate = 1
            outs.append(
                ((oparams, 0),))
        splits.append(axis_slice[1] - axis_slice[0])
        outs.append([(edge.to_node, edge.to_idx)
                     for edge in G.out_edges(slice_node.name)])
        if axis_slice[1] < axis_dim:
            splits.append(axis_dim - axis_slice[1])
            oparams = OutputParameters(G.unique_name('unused'))
            oparams.at_options.allocate = 1
            outs.append(
                ((oparams, 0),))
        in_edge = G.in_edges(slice_node.name)[0]
        G.remove(slice_node)
        act_slices, out_shapes, axis = SplitParameters.get_splits(
            in_dims, axis, splits=splits)
        LOG.info('replacing strided slice %s with split with %s redundant outputs',
                 slice_node.name, len(outs) - 1)
        if axis != 0:
            LOG.warning('adjust needs to be rerun')
        split_params = SplitParameters(slice_node.name, act_slices=act_slices,
                                       out_shapes=out_shapes, axis=axis)
        G.add_edge(NNEdge(from_node=in_edge.from_node,
                          from_idx=in_edge.from_idx, to_node=split_params))
        for out_idx, out_cons in enumerate(outs):
            for out_con in out_cons:
                G.add_edge(NNEdge(from_node=split_params, from_idx=out_idx,
                                  to_node=out_con[0], to_idx=out_con[1]))
        if G.quantization:
            G.add_dimensions()
            quantizer = UnifiedQuantizer.from_quantized_graph(G)
            quantizer.quantize(G, start_nodes=[split_params])
            RemoveUnnecessaryQuantizeOperators().match(G)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        slices_by_origin = {}
        for slice_node in [node for node in G.nodes() if isinstance(node, StridedSliceParameters)]:
            in_edge = G.in_edges(slice_node.name)[0]
            group = slices_by_origin.setdefault(
                (in_edge.from_node, in_edge.from_idx), [])
            group.append(slice_node)
        for in_edge, slice_nodes in slices_by_origin.items():
            slices = list(zip(*[node.act_slice for node in slice_nodes]))
            if len(slice_nodes) == 1:
                self.slice_to_split(G, slice_nodes, slices)
                continue

            diff_slices = [(idx, elems) for idx, elems in enumerate(slices)
                           if not all(elems[0] == elem for elem in elems[1::])]
            if len(diff_slices) != 1:
                continue
            # strides must be one
            if any(sl[2] != 1 for sl in diff_slices[0][1]):
                continue
            # check if slices are consecutive and non overlapping
            slices = sorted(diff_slices[0][1], key=lambda x: x[0])
            if not all(sl[0] + sl[1] == slices[i+1][0] for i, sl in enumerate(slices[:-1:])):
                continue
            szes = [sl[1] - sl[0] for sl in slices]
            axis = diff_slices[0][0]
            slice_nodes = sorted(
                slice_nodes, key=lambda x: x.act_slice[axis][0])
            act_slices, out_shapes, axis = SplitParameters.get_splits(
                slice_nodes[0].in_dims[0].shape, axis, splits=szes)
            params = SplitParameters(
                slice_nodes[0].name + '_split', act_slices=act_slices, out_shapes=out_shapes, axis=axis)
            in_edge = G.in_edges(slice_nodes[0].name)[0]
            G.add_edge(NNEdge(from_node=in_edge.from_node,
                              to_node=params, from_idx=in_edge.from_idx))
            sub_names = []
            for idx, node in enumerate(slice_nodes):
                sub_names.append(node.name)
                out_edges = G.out_edges(node.name)
                G.remove(node)
                for out_edge in out_edges:
                    G.add_edge(NNEdge(from_node=params, to_node=out_edge.to_node,
                                      from_idx=idx, to_idx=out_edge.to_idx))
            if G.quantization:
                G.add_dimensions()
                quantizer = UnifiedQuantizer.from_quantized_graph(G)
                quantizer.quantize(G, start_nodes=[params])
                RemoveUnnecessaryQuantizeOperators().match(G)

            LOG.info(
                f'replaced slice nodes {",".join(sub_names)} with split node {sub_names[0]}')

            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
