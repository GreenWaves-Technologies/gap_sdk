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

from graph.types import NNEdge, ReverseParameters, RNNBaseParameters
from utils.graph import GraphView

from ..matcher import Matcher, groups, match_name, description, run_before, needs_valid_dimension

LOG = logging.getLogger("nntool." + __name__)

@groups('scaled')
@match_name("rnn_reverse")
@description('Fuse reverse operation into RNN/LSTM')
@run_before('rnn_unpack')
@needs_valid_dimension(True)
class MatchReversedRnn(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        # Only works for reverses connected to one RNN node
        reverse_nodes = set([node for node in G.nodes() if (isinstance(node, ReverseParameters) and
                                                            len(G.out_edges(node.name)) == 1 and
                                                            isinstance(G.out_edges(node.name)[0].to_node,
                                                                       RNNBaseParameters))])

        has_modified_graph = False
        for reverse_node in reverse_nodes:
            in_edges = G.in_edges(reverse_node.name)
            rnn_edge = G.out_edges(reverse_node.name)[0]
            if rnn_edge.to_idx != 0:
                LOG.warning("reverse on rnn input %s", rnn_edge.to_idx)
                continue
            assert not rnn_edge.to_node.revert, "RNN node is already reversed!"
            rnn_edge.to_node.revert = True
            LOG.info("fusing reverses into node %s", rnn_edge.to_node.name)
            has_modified_graph = True
            G.remove(reverse_node)
            for edge in in_edges:
                G.add_edge(NNEdge(edge.from_node, rnn_edge.to_node,
                                  from_idx=edge.from_idx, to_idx=rnn_edge.to_idx))

            for edge in G.out_edges(rnn_edge.to_node.name):
                if not isinstance(edge.to_node, ReverseParameters):
                    continue
                if edge.from_idx != 0:
                    LOG.warning("reverse on rnn output %s", edge.from_idx)
                    continue
                rev_edges = G.out_edges(edge.to_node.name)
                G.remove(edge.to_node)
                for rev_edge in rev_edges:
                    G.add_edge(NNEdge(edge.from_node, rev_edge.to_node,
                                      from_idx=edge.from_idx, to_idx=rev_edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
