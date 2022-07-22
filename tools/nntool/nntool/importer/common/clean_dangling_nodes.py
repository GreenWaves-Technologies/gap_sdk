# Copyright (C) 2021  GreenWaves Technologies, SAS

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
from nntool.graph.types import OutputNode

LOG = logging.getLogger('nntool.' + __name__)

def clean_dangling_nodes(G):
    while True:
        dangling_nodes = [node for node in G.nodes() if not isinstance(node, OutputNode) and len(G.out_edges(node.name)) == 0]
        if not dangling_nodes:
            break
        for node in dangling_nodes:
            if G.quantization:
                nid = node.name
                if nid in G.quantization:
                    del G.quantization[nid]
            LOG.info('removing node %s which is not connected to an output', node.name)
            G.remove(node)