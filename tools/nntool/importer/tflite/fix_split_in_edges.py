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

from graph.nngraph import NNGraph
from graph.types import SplitParameters, NNEdge


def fix_split_in_edges(G: NNGraph):
    for split in [node for node in G.nodes() if isinstance(node, SplitParameters)]:
        in_edge = G.in_edges(split.name)[0]
        if in_edge.to_idx == 0:
            continue
        G.remove_edge(in_edge)
        G.add_edge(NNEdge(in_edge.from_node, in_edge.to_node, from_idx=in_edge.from_idx))
