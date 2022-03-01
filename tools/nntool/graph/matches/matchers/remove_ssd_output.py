# Copyright (C) 2020, 2022  GreenWaves Technologies, SAS

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

from graph.types import SSDDetectorParameters
from utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name, run_qtune_on_match,
                       needs_valid_dimension)

LOG = logging.getLogger("nntool." + __name__)


@match_name('remove_ssd_output')
@description('remove the 4th output on the ssd detector - num detections. This is necessary for '
             'operation with GAP kernels')
@groups('*')
@needs_valid_dimension(True)
@run_qtune_on_match
class RemoveSSDOutput(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False
        for node in G.nodes(node_classes=SSDDetectorParameters):
            if not node.output_detection_count:
                continue
            has_modified_graph = True
            LOG.info(f'removing detection count output on {node.name}')
            edges_below = G.indexed_out_edges(node)[3]
            for edge in edges_below:
                G.remove_below(edge.to_node)
                G.remove(edge.to_node)
            node.output_detection_count = False

        return has_modified_graph
