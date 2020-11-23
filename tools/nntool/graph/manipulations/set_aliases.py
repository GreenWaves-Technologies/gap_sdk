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

from graph.types.others import TransposeParameters

from ..types import ConcatParameters, ReshapeParameters, SplitParameters

LOG = logging.getLogger("nntool." + __name__)


def walk_up(G, edge, concat_node):
    LOG.debug("edge %s %s[%s] -> %s[%s] is an alias",
              edge.params.name, edge.from_node.name, edge.from_idx,
              edge.to_node.name, edge.to_idx)
    edge.params.is_alias = True
    node = edge.from_node
    if isinstance(node, ReshapeParameters):
        # since it is a reshape it can only have one input
        return walk_up(G, G.in_edges(node.name)[0], concat_node)
    if isinstance(node, TransposeParameters):
        if not node.does_nothing():
            return False
        # since it is a reshape it can only have one input
        return walk_up(G, G.in_edges(node.name)[0], concat_node)
    if isinstance(node, SplitParameters):
        LOG.warning("split node %s is directly connected to concat node %s",
                    node.name, concat_node.name)
        return True
    return False


def walk_down(G, edge, split_node):
    LOG.debug("edge %s %s[%s] -> %s[%s] is an alias",
              edge.params.name, edge.from_node.name, edge.from_idx,
              edge.to_node.name, edge.to_idx)
    edge.params.is_alias = True
    node = edge.to_node
    errors = False
    if isinstance(node, ReshapeParameters):
        for edge in G.out_edges(node.name):
            errors = errors or walk_down(G, edge, split_node)
    elif isinstance(node, TransposeParameters):
        if not node.does_nothing():
            return errors
        for edge in G.out_edges(node.name):
            errors = errors or walk_down(G, edge, split_node)
    elif isinstance(node, ConcatParameters):
        LOG.warning("concat node %s is directly connected to split node %s",
                    node.name, split_node.name)
        return True
    else:
        return False
    return errors


def set_aliases(G):
    """ Walks graph looking for edges that are aliases
    """
    errors = False
    LOG.info("looking for aliased edges")
    all_concats = [node for node in G.nodes() if isinstance(node, ConcatParameters)]
    for concat in all_concats:
        for edge in G.in_edges(concat.name):
            errors = errors or walk_up(G, edge, concat)
    all_splits = [node for node in G.nodes() if isinstance(node, SplitParameters)]
    for split in all_splits:
        for edge in G.out_edges(split.name):
            errors = errors or walk_down(G, edge, split)
    if errors:
        LOG.warning("splits connected to concats directly in graph - this will not generate correctly")
    return errors
