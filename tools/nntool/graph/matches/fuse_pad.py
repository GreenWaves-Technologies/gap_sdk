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

from graph.types import (FilterLikeParameters, PadParameters)
from utils.graph import MatchNode, GraphView, Edge

from .matcher import DefaultMatcher

LOG = logging.getLogger("nntool." + __name__)

class MatchFusePad(DefaultMatcher):
    NAME = 'fuse_pad'
    DESCRIPTION = 'Fuse pad operation to subsequent Convolution or Pool'

    @staticmethod
    def has_no_padding(node):
        return node.padding.size() == 0

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0', matcher=lambda node:\
                isinstance(node, PadParameters)))
        sub.add_node(MatchNode('1', matcher=lambda node:\
                isinstance(node, FilterLikeParameters) and\
                self.has_no_padding(node)))
        sub.add_edge(Edge('0', '1'))
        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        for node in subgraph.nodes():
            if isinstance(node, FilterLikeParameters):
                filter_like_node = node
            elif isinstance(node, PadParameters):
                pad_node = node

        LOG.debug("adding padding from: %s to filter: %s", pad_node.name, filter_like_node.name)

        filter_like_node.padding = pad_node.padding
        filter_like_node.pad_type = "zero"
        if G.quantization:
            G.quantization.remove_node(pad_node)
        return filter_like_node, None, None
