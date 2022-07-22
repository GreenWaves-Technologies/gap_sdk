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

from ..types import ConcatNode, SplitNode

LOG = logging.getLogger(__name__)


def walk_up(G, edge, concat_node):
    if edge is None:
        raise ValueError('edge is None')
    if edge.params is None:
        raise ValueError(f'edge {edge.from_node.name}[{edge.from_idx}] -> '
                         f'{edge.to_node.name}[{edge.to_idx}] has no edge params set')

    LOG.debug("edge %s %s[%s] -> %s[%s] is an alias",
              edge.params.name, edge.from_node.name, edge.from_idx,
              edge.to_node.name, edge.to_idx)
    edge.params.is_alias = concat_node.no_model_code
    node = edge.from_node
    if node.exclude_from_generation:
        return walk_up(G, G.in_edges(node.name)[0], concat_node)
    if isinstance(node, SplitNode):
        LOG.warning("split node %s is directly connected to concat node %s",
                    node.name, concat_node.name)
        return True
    return False


def walk_down(G, edge, split_node):
    if edge is None:
        raise ValueError('edge is None')
    if edge.params is None:
        raise ValueError(f'edge {edge.from_node.name}[{edge.from_idx}] -> '
                         f'{edge.to_node.name}[{edge.to_idx}] has no edge params set')
    LOG.debug("edge %s %s[%s] -> %s[%s] is an alias",
              edge.params.name, edge.from_node.name, edge.from_idx,
              edge.to_node.name, edge.to_idx)
    edge.params.is_alias = split_node.no_model_code
    node = edge.to_node
    errors = False
    if node.exclude_from_generation:
        for edge in G.out_edges(node.name):
            errors = errors or walk_down(G, edge, split_node)
    elif isinstance(node, ConcatNode):
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
    LOG.debug("looking for aliased edges")
    for concat in G.nodes(node_classes=ConcatNode):
        LOG.debug("looking at concat %s", concat.name)
        for edge in G.in_edges(concat.name):
            res = walk_up(G, edge, concat)
            errors = errors or res
    for split in G.nodes(node_classes=SplitNode):
        LOG.debug("looking at split %s", split.name)
        for edge in G.out_edges(split.name):
            res = walk_down(G, edge, split)
            errors = errors or res
    if errors:
        LOG.warning(
            "splits connected to concats directly in graph - this will not generate correctly")
    return errors
