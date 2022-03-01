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

def search_down(G, edge, for_node_classes, can_pass=None, can_pass_fn=None, edge_list=None, follow_multi=False,
                follow_first=True, multi_on_target=True):
    """ Search down for node classes

    Args:
        G (NNGraph): Graph
        edge (Edge): Edge to look down
        for_node_classes (Union[Parameters, Tuple[Parameters]]): Node class or classes to look for
        can_pass (Union[Parameters, Tuple[Parameters]], optional): Will pass through this node class or classes.
                                                                   Defaults to None.
        can_pass_fn (Callable, optional): function with graph and node as parameters. Should return True if
                                          search can pass this node. Defaults to None.
        follow_multi (bool, optional): Follow multi edge outputs. Defaults to False.
        follow_first (bool, optional): Only follow first input. Defaults to True.
        multi_on_target (bool, optional): Allow target to have multiple edges. Defaults to True.

    Returns:
        Optional[Sequence[Edge]]: Edges found or None
    """
    if edge_list is None:
        edge_list = []
    edge_list.append(edge)
    in_edges = G.in_edges(edge.to_node)
    if isinstance(edge.to_node, for_node_classes):
        if len(in_edges) > 1 and not multi_on_target:
            return None
        return edge_list
    if len(in_edges) != 1:
        if not follow_multi:
            return None
        for in_edge in in_edges:
            if in_edge in edge_list:
                continue
            res = search_up(G, in_edge, for_node_classes, can_pass=can_pass, can_pass_fn=can_pass_fn,
                            edge_list=edge_list, follow_multi=follow_multi, follow_first=follow_first,
                            multi_on_target=multi_on_target)
            if res:
                return res
    if ((can_pass and isinstance(edge.to_node, can_pass)) or
            (can_pass_fn and can_pass_fn(G, edge.to_node))):
        out_edges = G.out_edges(edge.to_node)
        if not follow_multi and len(out_edges) != 1:
            return None
        for out_edge in out_edges:
            res = search_down(G, out_edge, for_node_classes=for_node_classes, can_pass_fn=can_pass_fn,
                              can_pass=can_pass, follow_multi=follow_multi, edge_list=edge_list,
                              multi_on_target=multi_on_target)
            if res:
                return res
    return None


def search_up(G, edge, for_node_classes, can_pass=None, can_pass_fn=None, edge_list=None, follow_multi="",
              follow_first=True, multi_on_target=True):
    """ Search up for node classes

    Args:
        G (NNGraph): Graph
        edge (Edge): Edge to look up
        for_node_classes (Union[Parameters, Tuple[Parameters]]): Node class or classes to look for
        can_pass (Union[Parameters, Tuple[Parameters]], optional): Will pass through this node class or classes.
                                                                   Defaults to None.
        can_pass_fn (Callable, optional): function with graph and node as parameters. Should return True if
                                          search can pass this node. Defaults to None.
        follow_multi (str, optional): Follow multi edge outputs. Defaults to empty string which means don't
                follow can be same or any.
        follow_first (bool, optional): Only follow first input. Defaults to True.
        multi_on_target (bool, optional): Allow target to have multiple edges. Defaults to True.

    Returns:
        Optional[Sequence[Edge]]: Edges found or None
    """
    if edge_list is None:
        edge_list = []
    edge_list.append(edge)
    out_edges = G.out_edges(edge.from_node)
    if isinstance(edge.from_node, for_node_classes):
        if len(out_edges) > 1 and not multi_on_target:
            return None
        return edge_list
    if len(out_edges) != 1 and not isinstance(edge.from_node, for_node_classes):
        if not follow_multi:
            return None
        for out_edge in out_edges:
            if out_edge in edge_list:
                continue
            if follow_multi == "same" and out_edge.from_idx != edge.from_idx:
                continue
            res = search_down(G, out_edge, for_node_classes, can_pass=can_pass, can_pass_fn=can_pass_fn,
                            edge_list=edge_list, follow_multi=follow_multi, multi_on_target=multi_on_target)
            if res:
                return res
    if ((can_pass and isinstance(edge.from_node, can_pass)) or
            (can_pass_fn and can_pass_fn(G, edge.from_node))):
        in_edges = G.in_edges(edge.from_node)
        if not follow_multi and len(in_edges) != 1:
            return None
        for in_edge in in_edges:
            if follow_first and in_edge.to_idx != 0:
                continue
            res = search_up(G, in_edge, for_node_classes=for_node_classes, can_pass_fn=can_pass_fn,
                            can_pass=can_pass, follow_first=follow_first,
                            follow_multi=follow_multi, edge_list=edge_list, multi_on_target=multi_on_target)
            if res:
                return res
    return None
