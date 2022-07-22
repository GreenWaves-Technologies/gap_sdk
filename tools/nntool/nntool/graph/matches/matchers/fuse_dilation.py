# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from itertools import zip_longest
import logging
import typing

from nntool.graph.dim import DilationDim, Dim, PadDim
from nntool.graph.types import Conv2DNode, NNEdge, ReshapeNode
from nntool.graph.types.tensor_shape import (BatchToSpaceNode,
                                             SpaceToBatchNode,
                                             StridedSliceNode)

from ..matcher import Matcher, description, groups, match_name, run_before

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph


LOG = logging.getLogger(__name__)


def find_batch_to_space(G: 'NNGraph', node, seen_conv=False):
    if isinstance(node, BatchToSpaceNode):
        if seen_conv:
            return [node]
        return None
    if isinstance(node, (Conv2DNode)):
        seen_conv = True
    elif not isinstance(node, (SpaceToBatchNode, ReshapeNode)):
        return None
    out_edges = G.out_edges(node)
    if len(out_edges) != 1:
        return None
    res = find_batch_to_space(G, out_edges[0].to_node, seen_conv=seen_conv)
    if res is None:
        return None
    return [node] + res


def decompose(nodes, *clses):
    if len(nodes) != len(clses):
        return [None] * len(clses)
    if not all(isinstance(node, cls) for node, cls in zip(nodes, clses)):
        return [None] * len(clses)
    return nodes


@groups('*')
@match_name("fuse_dilation")
@description("""Fuse dilation patterns with space to batch and batch to space into convs""")
@run_before('*')
class FuseDilation(Matcher):

    def _match(self, G: 'NNGraph', **kwargs):
        node_chains = list(filter(lambda x: x is not None, [
            find_batch_to_space(G, node) for node in G.nodes(node_classes=SpaceToBatchNode)]))
        modified_graph = False
        while node_chains:
            node_chain = node_chains.pop()
            dilation = None
            padding = None
            if len(node_chain) == 3: # 2D case
                stb, conv, bts = decompose(
                    node_chain, SpaceToBatchNode, Conv2DNode, BatchToSpaceNode)
                after_node = conv
                before_node = conv
                if not stb:
                    continue
                if (not conv.in_dims_hint or conv.in_dims_hint[0] != ['n', 'h', 'w', 'c']):
                    continue
                block_shape = stb.block_shape
                if len(block_shape) != 2:
                    continue
                dilation = tuple(block_shape)
                padding = stb.pads
                new_shape = conv.in_dims[0].shape[1:]
                remove_batch = ReshapeNode(
                    G.unique_name(f'{conv.name}_rbatch'),
                    shape=new_shape)
                G.insert_node_before(remove_batch, conv, edge_class=NNEdge)
                insert_batch = ReshapeNode(
                    G.unique_name(f'{conv.name}_ibatch'),
                    shape=bts.out_dims[0].shape)
                G.insert_node_after(insert_batch, conv, edge_class=NNEdge)
                crops = bts.crops
            elif len(node_chain) == 5: # 1D case
                stb, before_node, conv, after_node, bts = decompose(
                    node_chain, SpaceToBatchNode, ReshapeNode, Conv2DNode, ReshapeNode, BatchToSpaceNode)
                if (not conv.in_dims_hint or conv.in_dims_hint[0] != ['n', 'h', 'w', 'c']):
                    continue
                block_shape = node_chain[0].block_shape
                if len(block_shape) != 1:
                    continue
                # pad the input shape to the stb in line with the reshape that
                # is doing the 1D -> 2D
                in_dim = list(stb.in_dims[0].shape)
                if before_node.shape[1] == 1:
                    dilation = tuple([1, block_shape[0]])
                    padding = [(0, 0), stb.pads[0]]
                    new_shape = [1] + in_dim[1:] # shape after expand 1D 2D without batch
                    crops = [(0, 0)] + bts.crops
                elif before_node.shape[2] == 1:
                    dilation = tuple([block_shape[0], 1])
                    padding = [stb.pads[0], (0, 0)]
                    new_shape = in_dim[1:2] + [1, in_dim[2]] # shape after expand 1D 2D without batch
                    crops = bts.crops + [(0, 0)]
                else:
                    continue
                # rewrite reshapes - the reshapes are within the stb -> bts
                before_node.shape = Dim.unnamed(new_shape)
                after_node.shape = Dim.unnamed(bts.out_dims[0].shape)
            else:
                continue
            modified_graph = True

            # remove the stb -> bts pair
            G.remove_and_reconnect(stb, edge_class=NNEdge)
            G.remove_and_reconnect(bts, edge_class=NNEdge)

            # modify the conv
            # add new dilation and padding
            conv.dilation = DilationDim(*dilation)
            padding = [dim for dim_set in padding for dim in dim_set]
            pad_dim = PadDim(*padding)
            if pad_dim != 0:
                conv.padding = pad_dim
            # remove the batch dimension
            conv.remove_batch()

            # if there is a crop place that after the convolution
            if any(crop != 0 for crop_pair in crops for crop in crop_pair):
                # calculate new output shape
                out_shape = conv.get_output_size(
                    [Dim.named_ordered(
                        h=new_shape[0], w=new_shape[1], c=new_shape[2])])[0]
                # calculate slice and output shape
                slices = [(crop_start, dim-crop_end, 1)
                        for dim, (crop_start, crop_end)
                        in zip_longest(out_shape, crops, fillvalue=(0, 0))]
                out_shape = [sl[1] - sl[0] for sl in slices]
                ssnode = StridedSliceNode(
                    G.unique_name(f'{conv.name}_slice'),
                    act_slice=slices, out_shape=out_shape)
                G.insert_node_after(conv, ssnode, edge_class=NNEdge)

        return modified_graph
