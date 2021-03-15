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

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


class MatchFusePad(Matcher):
    NAME = 'fuse_pad'
    DESCRIPTION = 'Fuse pad operation to subsequent Convolution or Pool'

    @staticmethod
    def remove_padding(shape, padding):
        return Dim.unnamed([dim - sum(padding[idx]) for idx, dim in enumerate(shape)])

    @staticmethod
    def find_conv(G, node):
        if isinstance(node, FilterLikeParameters):
            return node, False
        if isinstance(node, ReshapeParameters):
            out_edges = G.out_edges(node.name)
            if len(out_edges) == 1 and isinstance(out_edges[0].to_node, FilterLikeParameters):
                return out_edges[0].to_node, True
        return None, False

    def match(self, G: GraphView, set_identity: bool = True) -> bool:
        has_modified_graph = False
        for pad_params in [pad for pad in G.nodes() if isinstance(pad, PadParameters)]:
            pad_in_edges = G.in_edges(pad_params.name)
            pad_out_edges = G.out_edges(pad_params.name)
            dont_delete = False
            for pad_out_edge in pad_out_edges:
                filter_like_node, is_1d = self.find_conv(G, pad_out_edge.to_node)
                if not filter_like_node:
                    dont_delete = True
                    continue
                if not filter_like_node.in_dims_hint or not filter_like_node.in_dims_hint[0]:
                    raise ValueError(f"filter {filter_like_node.name} doesn't have a input hint")
                in_hint = filter_like_node.in_dims_hint[0]
                if is_1d:
                    if len(pad_params.padding) != 2:
                        LOG.warning("pad node %s is applied to 1d convolution but has length %s",
                                    pad_params.name,
                                    len(pad_params.padding))
                        dont_delete = True
                        continue
                    expanded_padding = [pad_params.padding[0], (0, 0), pad_params.padding[1]]
                else:
                    if len(pad_params.padding) != 3:
                        LOG.warning("pad node %s is applied to 2d convolution but has length %s",
                                    pad_params.name,
                                    len(pad_params.padding))
                        dont_delete = True
                        continue
                    expanded_padding = pad_params.padding

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

                LOG.info("adding padding from: %s to %s filter: %s",
                         pad_params.name, is_1d and "1D" or "2D", filter_like_node.name)

                for key in ['h', 'w']:
                    if key not in hinted_pad:
                        hinted_pad[key] = (0, 0)

                filter_like_node.padding = PadDim(*(list(hinted_pad['h']) + list(hinted_pad['w'])))
                filter_like_node.pad_type = "zero"
                has_modified_graph = True
                G.remove_edge(pad_out_edge)
                if is_1d:
                    reshape_node = pad_out_edge.to_node
                    reshape_node.old_shape = self.remove_padding(
                        reshape_node.old_shape, pad_params.padding)
                    reshape_node.shape = self.remove_padding(reshape_node.shape, expanded_padding)
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
