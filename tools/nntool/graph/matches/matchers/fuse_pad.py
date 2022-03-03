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

from graph.dim import Dim, PadDim
from graph.types import FilterLikeParameters, PadParameters
from graph.types.base import NNEdge
from graph.types.others import ReshapeParameters
from utils.graph import GraphView

from ..matcher import Matcher, match_name, description, run_before, groups

LOG = logging.getLogger("nntool." + __name__)


def expand_padding(from_shape, to_shape, padding):
    """ Expands padding with new axes added by reshape. Will fail if the reshape
    is doing more than just inserting 1s"""
    from_shape = tuple(from_shape)
    to_shape = tuple(to_shape)
    if from_shape == to_shape:
        return padding
    from_idx = 0
    to_idx = 0
    new_padding = []
    while to_idx < len(to_shape) or from_idx < len(from_shape):
        if from_idx < len(from_shape) and to_idx < len(to_shape) and from_shape[from_idx] == to_shape[to_idx]:
            new_padding.append(padding[from_idx])
            from_idx += 1
            to_idx += 1
        elif to_idx < len(to_shape) and to_shape[to_idx] == 1:
            new_padding.append((0, 0))
            to_idx += 1
        elif from_idx < len(from_shape) and from_shape[from_idx] == 1 and sum(padding[from_idx]) == 0:
            from_idx += 1
        else:
            return None
    return new_padding if from_idx == len(from_shape) and to_idx == len(to_shape) else None

@match_name('fuse_pad')
@description('Fuse pad operation to subsequent Convolution or Pool')
@groups('*')
@run_before('fuse_gap_convs', 'fuse_gap_pool')
class MatchFusePad(Matcher):
    @staticmethod
    def remove_padding(shape, padding):
        return Dim.unnamed([dim - sum(padding[idx]) for idx, dim in enumerate(shape)])

    @staticmethod
    def find_conv(G, node, padding, reshapes=None):
        """ Search for filterlike nodes passing through potential single edge reshapes"""
        if reshapes is None:
            reshapes = []
        if isinstance(node, FilterLikeParameters):
            return node, padding, reshapes
        if isinstance(node, ReshapeParameters):
            out_edges = G.out_edges(node.name)
            # could handle multi edge in the future
            if len(out_edges) == 1:
                new_padding = expand_padding(
                    node.old_shape.shape, node.shape.shape, padding)
                if new_padding is None:
                    LOG.warning(
                        'pad before filter found but reshape %s is not allowing it to be fused', node.name)
                    return None, None, None
                out_edge = out_edges[0]
                # keep track of the reshapes we passed through
                reshapes.append((node, padding, new_padding))
                return MatchFusePad.find_conv(G, out_edge.to_node, new_padding, reshapes)
            else:
                LOG.warning(
                    'pad before filter found but reshape %s is not allowing it to be fused - multi edge', node.name)

        return None, None, None

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        for pad_params in [pad for pad in G.nodes() if isinstance(pad, PadParameters)]:
            pad_in_edges = G.in_edges(pad_params.name)
            pad_out_edges = G.out_edges(pad_params.name)
            dont_delete = False
            if len(pad_in_edges) == 1 and all(sum(padding) == 0 for padding in pad_params.padding):
                LOG.info("removing zero padding node %s",
                         pad_params.name)
                G.remove(pad_params)
                if G.quantization:
                    G.quantization.remove_node(pad_params)
                dont_delete = True
                in_edge = pad_in_edges[0]
                for out_edge in pad_out_edges:
                    G.add_edge(NNEdge(from_node=in_edge.from_node,
                                      to_node=out_edge.to_node,
                                      from_idx=in_edge.from_idx,
                                      to_idx=out_edge.to_idx))
            else:
                for pad_out_edge in pad_out_edges:
                    filter_like_node, expanded_padding, reshapes = self.find_conv(
                        G, pad_out_edge.to_node, pad_params.padding)
                    if not filter_like_node:
                        dont_delete = True
                        continue
                    if not filter_like_node.in_dims_hint or not filter_like_node.in_dims_hint[0]:
                        raise ValueError(
                            f"filter {filter_like_node.name} doesn't have a input hint")
                    in_hint = filter_like_node.in_dims_hint[0]

                    hinted_pad = {in_hint[idx]: pad for idx,
                                  pad in enumerate(expanded_padding) if sum(pad) > 0}
                    key_set = set(hinted_pad.keys())
                    key_set -= set(['h', 'w'])
                    if len(key_set) > 0:
                        dont_delete = True
                        LOG.error("node %s has padding on axes %s and cannot be fused with filter %s",
                                  pad_params.name, key_set, filter_like_node.name)
                        continue
                    if any(pval != 0 for val in pad_params.pad_vals for pval in val):
                        dont_delete = True
                        LOG.error("node %s has non zero pad values and cannot be fused with filter %s",
                                  pad_params.name, filter_like_node.name)
                        continue

                    LOG.info("adding padding from: %s to filter: %s - has %s reshapes",
                             pad_params.name, filter_like_node.name, len(reshapes))

                    for key in ['h', 'w']:
                        if key not in hinted_pad:
                            hinted_pad[key] = (0, 0)

                    filter_like_node.padding = PadDim(
                        *(list(hinted_pad['h']) + list(hinted_pad['w'])))
                    filter_like_node.pad_type = "zero"
                    has_modified_graph = True
                    G.remove_edge(pad_out_edge)
                    for reshape_node, old_padding, new_padding in reshapes:
                        reshape_node.old_shape = self.remove_padding(
                            reshape_node.old_shape, old_padding)
                        reshape_node.shape = self.remove_padding(
                            reshape_node.shape, new_padding)

                    for in_edge in pad_in_edges:
                        G.add_edge(NNEdge(from_node=in_edge.from_node,
                                          to_node=pad_out_edge.to_node,
                                          from_idx=in_edge.from_idx,
                                          to_idx=pad_out_edge.to_idx))

            if not dont_delete:
                G.remove(pad_params)
                if G.quantization:
                    G.quantization.remove_node(pad_params)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
