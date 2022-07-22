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

from nntool.graph.matches.matchers.fuse_gap_convs import (NewFusionMatch,
                                                   fuse_expression_activation)
from nntool.graph.types import (ExpressionFusionNode, LinearNode,
                         HSigmoidNode,
                         HSwishNode, LeakyNode,
                         ReluNode, SigmoidNode)
from nntool.graph.types.activations import TanHNode
from nntool.utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name,
                       run_adjust_on_match, run_again_on_match,
                       run_qtune_on_match)

LOG = logging.getLogger(__name__)

VALID_ACTIVATIONS_SQ8 = (
    ReluNode,
    LeakyNode,
    HSigmoidNode,
    HSwishNode,
    SigmoidNode,
    TanHNode,
    ExpressionFusionNode
)

VALID_ACTIVATIONS_POW2 = (
    ReluNode,
    LeakyNode,
    HSigmoidNode,
    HSwishNode
)

@groups('*')
@run_qtune_on_match
@run_adjust_on_match
@run_again_on_match('common_fused_expressions')
@match_name("fuse_gap_linear")
@description('Fuse linear layers and activations to match GAP AutoTiler operations')
class MatchGapLinear(Matcher):

    def _match(self, G: GraphView, **kwargs):
        has_modified_graph = False
        group_identity = kwargs.get('group_identity')
        if group_identity == 'pow2_match_group':
            valid_activations = VALID_ACTIVATIONS_POW2
        else:
            valid_activations = VALID_ACTIVATIONS_SQ8
        
        for fc_node in G.nodes(node_classes=(LinearNode,)):
            matcher = NewFusionMatch.from_node(G, fc_node, valid_activations, ('linear_active',),
                                               validators={ExpressionFusionNode: fuse_expression_activation})
            if not matcher or not matcher.can_fuse:
                continue
            has_modified_graph = True
            matcher.fuse(G)



        return has_modified_graph
