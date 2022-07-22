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

from collections.abc import Iterable, Mapping
from itertools import zip_longest
from typing import Generic, Optional, Sequence, Set, Tuple, TypeVar, Union

from nntool.generation.code_block import StreamCodeBlock


def is_iterable(x):
    try:
        iter(x)  # @IgnoreException
    except TypeError:
        return False
    return True


class GraphError(Exception):
    pass


class GraphMismatchError(GraphError):
    pass


class EdgeExistsError(GraphError):
    pass


class InternalGraphError(GraphError):
    pass


class MoreThanOneInputError(GraphError):
    pass

# pylint: disable=protected-access


class Node():
    '''Node class to inherit for nodes'''

    def __init__(self, name: str, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._name = name

    @property
    def _noderef_class(self):
        return NodeRef

    @property
    def name(self):
        '''Node name - must not be changed once node is in graph'''
        return self._name

    @name.setter
    def name(self, name):
        '''Node name - must not be changed once node is in graph'''
        self._name = name

    @property
    def attrs(self) -> dict:
        return {}

    def __call__(self, *args, num_outputs=1) -> Union['NodeRef', Sequence['NodeRef']]:
        inputs = []
        fragments = set()
        for arg in args:
            if arg is not None and not isinstance(arg, self._noderef_class):
                if isinstance(arg, Node):
                    inputs.append((arg, 0))
                    continue
                raise ValueError(
                    f"expecting {self._noderef_class.__name__} or Node or None")
            inputs.append(arg.ref[0] if arg else None)
            fragments.add(arg.ref[1] if arg else None)
        fragment = next(
            iter([frag for frag in fragments if frag is not None]), None)
        if fragment is None:
            raise ValueError("At least one input must be a reference")
        other_fragments = fragments - {fragment}

        for other in other_fragments:
            if hasattr(fragment, 'merge'):
                if other is not None:
                    fragment.merge(other)
            else:
                raise ValueError('graph has no merge method')

        for to_idx, from_tuple in enumerate(inputs):
            if from_tuple is not None:
                from_node, from_idx = from_tuple
                fragment.add_edge(fragment._edge_class(from_node=from_node,
                                                       from_idx=from_idx,
                                                       to_node=self,
                                                       to_idx=to_idx))
        if num_outputs == 1:
            return self._noderef_class(fragment, self, 0)
        return tuple(self._noderef_class(fragment, self, idx) for idx in range(num_outputs))

    def write_python_description(self, code_block: StreamCodeBlock, indent=False, tensors=None) -> str:
        attrs = [f'{k}={repr(v)}' for k, v in self.attrs.items()]
        if attrs:
            code_block.write(f'{self.__class__.__name__}(', indent=indent)
            code_block.indent()
            code_block.write(f'"{self._name}",')
            for idx, attr in enumerate(attrs):
                code_block.write(f'{attr}{"," if idx < (len(attrs) - 1) else ""}')
            code_block.deindent()
            code_block.write(')', nl="")
        else:
            code_block.write(f'{self.__class__.__name__}("{self._name}")', nl="", indent=indent)

    def __str__(self):
        return self._name

    def __repr__(self) -> str:
        return f'{self.__class__.__name__}("{self._name}")'


NodeT = TypeVar('NodeT', bound=Node)

class NodeRef(Generic[NodeT]):
    def __init__(self, G: "GraphView", node: NodeT, idx: int) -> None:
        self._G = G
        self._node = node
        self._idx = idx

    @property
    def G(self) -> "GraphView":
        return self._G

    @property
    def ref(self) -> Tuple[Tuple[Node, int], "GraphView"]:
        return ((self._node, self._idx), self._G)

    @property
    def node(self) -> Node:
        return self._node

    def __eq__(self, o: object) -> bool:
        if isinstance(o, NodeRef):
            return super().__eq__(o)
        return self._node.__eq__(o)

    def __hash__(self) -> int:
        return self._node.__hash__()

    def __call__(self, *args, **kwargs):
        raise ValueError("this is already a reference")


class MatchNode(Node):
    '''Node class to inherit for node matchers'''

    def __init__(self, name, matcher=None):
        super().__init__(name)
        self._matcher = matcher

    def _match(self, G, node, edge):
        del G, edge
        if self._matcher:
            return self._matcher(node)
        raise NotImplementedError()


class MatchNameNode(MatchNode):
    '''Node matcher matching on node name'''

    def _match(self, G, node, edge):
        return self.name == node.name


def resolve_name(node):
    if isinstance(node, str):
        return node
    if isinstance(node, Node):
        return node.name
    if isinstance(node, NodeRef):
        return node.node.name
    raise ValueError("expecting Node, NodeRef or node name")


def resolve_node(node):
    if isinstance(node, Node):
        return node
    if isinstance(node, NodeRef):
        return node.node
    raise ValueError("expecting Node, or NodeRef")


def resolve_node_or_str(node, G=None):
    if isinstance(node, Node):
        return node
    if isinstance(node, NodeRef):
        return node.node
    if isinstance(node, str):
        if G:
            return G[node]
        return node
    raise ValueError("expecting Node, NodeRef or node name")


class Edge():
    '''Edge class to inherit for edges'''

    def __init__(self, from_node: Union[str, Node, NodeRef], to_node: Union[str, Node, NodeRef],
                 from_idx: int = 0, to_idx: int = 0):
        from_node = resolve_node_or_str(from_node)
        to_node = resolve_node_or_str(to_node)
        if not isinstance(from_idx, int):
            raise ValueError('expecting int for from_idx')
        if not isinstance(to_idx, int):
            raise ValueError('expecting int for to_idx')
        self._link = (from_node, from_idx, to_node, to_idx)

    @classmethod
    def from_src_to_dest(cls, from_edge, to_edge):
        return cls(
            from_node=from_edge.from_node, from_idx=from_edge.from_idx,
            to_node=to_edge.to_node, to_idx=to_edge.to_idx)

    @property
    def from_node(self):
        '''Edge start node'''
        return self._link[0]

    @property
    def to_node(self):
        '''Edge end node'''
        return self._link[2]

    @property
    def from_pair(self):
        '''Edge start node and index'''
        return (self._link[0], self._link[1])

    @property
    def to_pair(self):
        '''Edge end node and index'''
        return (self._link[2], self._link[3])

    @property
    def from_idx(self):
        '''Edge start output index'''
        return self._link[1]

    @property
    def to_idx(self):
        '''Edge end input index'''
        return self._link[3]

    def clone(self, from_node: Union[str, Node, NodeRef] = None, to_node: Union[str, Node, NodeRef] = None,
              from_idx: int = None, to_idx: int = None):
        if from_node is None:
            from_node = self.from_node
        if to_node is None:
            to_node = self.to_node
        if from_idx is None:
            from_idx = self.from_idx
        if to_idx is None:
            to_idx = self.to_idx
        return self.__class__(from_node, to_node, from_idx=from_idx, to_idx=to_idx)

    def __eq__(self, value):
        if not isinstance(value, Edge):
            return False
        return all((x == y for x, y in zip(self._link, value._link)))

    def __str__(self):
        return "{}:{}->{}:{}".format(*self._link)

    def __hash__(self):
        return self._link.__hash__()

# pylint: disable=too-many-public-methods


class GraphView(Mapping):

    def __init__(self, **attr):
        self._out_edges = {}
        self._in_edges = {}
        self._nodes = {}
        self._attr = attr

    @classmethod
    # pylint: disable=unused-argument
    def clone_factory(cls, G):
        return cls()

    def with_hidden_nodes(self, hidden_fn, edge_class=None):
        if edge_class is None:
            edge_class = Edge

        def real_up_node(G, edge):
            if hidden_fn(edge.from_node):
                edges = G.in_edges(edge.from_node)
                assert len(edges) == 1
                return real_up_node(G, edges[0])
            return edge.from_node, edge.from_idx

        def copy_node(G, new_graph, node):
            for edge in self.in_edges(node):
                from_node, from_idx = real_up_node(G, edge)
                new_edge = edge_class(from_node=from_node, from_idx=from_idx,
                                      to_node=node, to_idx=edge.to_idx)
                if new_graph.has_edge(new_edge):
                    continue
                new_graph.add_edge(new_edge)
                copy_node(G, new_graph, from_node)

        new_graph = self.__class__()
        setattr(new_graph, '_attr', self._attr)
        for node in self.outputs():
            copy_node(self, new_graph, node)
        return new_graph

    def has_edge(self, edge):
        edges = self._in_edges.get(edge.to_node.name)
        if not edges:
            return False
        edges = edges.get(edge.from_node.name)
        if not edges:
            return False
        return edge in edges

    def clear(self):
        '''Clears the graph view'''
        if self._nodes:
            self._in_edges.clear()
            self._out_edges.clear()
            self._nodes.clear()

    def clone(self) -> 'GraphView':
        '''Clones the GraphView'''
        clone = self.__class__.clone_factory(self)
        clone._in_edges = self._in_edges.copy()
        clone._out_edges = self._out_edges.copy()
        clone._nodes = self._nodes.copy()
        return clone

    def merge(self, other: 'Graph'):
        if self != other:
            for edge in other.edges:
                self.add_edge(edge)
        return self

    def num_nodes(self):
        '''Number of nodes len(GraphView) also works'''
        return len(self)

    def num_edges(self):
        '''Number of edges in the graph'''
        return sum(1 for edge_out in self._in_edges.values()
                   for edge_list in edge_out.values()
                   for edge in edge_list)

    def __add_in_edge(self, edge: Edge, update=False):
        edges = self._in_edges.setdefault(edge.to_node.name, {})
        edge_list = edges.setdefault(edge.from_node.name, [])
        edge_idx = next((i for i, x in enumerate(edge_list) if x == edge), -1)
        if edge_idx >= 0:
            if update:
                edge_list[edge_idx] = edge
            else:
                raise EdgeExistsError(
                    f'node {edge.to_node.name} already has an input on {edge.to_idx}')
        else:
            # useful for testing but will cause tests to fail since some fusions
            # use the ability to add a new edge before deleting an old
            # if any(existing_edge.to_idx == edge.to_idx for existing_edge in self.in_edges(edge.to_node.name)):
            #     raise EdgeExistsError(f'node {edge.to_node.name} already has an input on {edge.to_idx}')
            edge_list.append(edge)

    def __add_out_edge(self, edge: Edge, update=False):
        edges = self._out_edges.setdefault(edge.from_node.name, {})
        edge_list = edges.setdefault(edge.to_node.name, [])
        edge_idx = next((i for i, x in enumerate(edge_list) if x == edge), -1)
        if edge_idx >= 0:
            if update:
                edge_list[edge_idx] = edge
            else:
                raise EdgeExistsError()
        else:
            edge_list.append(edge)

    def verify_edges(self, check_connected=True):
        problems = []
        for node_name in self.keys():
            in_idxes = {}
            for edge in self.in_edges(node_name):
                if edge.to_idx in in_idxes:
                    problems.append(
                        "edge {0!s} connects to the same input as edge {1!s}"
                        .format(in_idxes[edge.to_idx], edge))
            if check_connected and self.num_in_edges(node_name) == 0 and self.num_out_edges(node_name) == 0:
                problems.append(f"{node_name} isn't connected")
        return problems

    def add_edge(self, edge: Edge):
        '''Adds an edge to the graph'''
        if isinstance(edge.from_node, str):
            edge = edge.clone(from_node=self._nodes[edge.from_node])
        elif edge.from_node.name not in self._nodes:
            assert edge.from_node not in self._nodes.values()
            self._nodes[edge.from_node.name] = edge.from_node
        if isinstance(edge.to_node, str):
            edge = edge.clone(to_node=self._nodes[edge.to_node])
        elif edge.to_node.name not in self._nodes:
            assert edge.to_node not in self._nodes.values()
            self._nodes[edge.to_node.name] = edge.to_node
        self.__add_in_edge(edge)
        self.__add_out_edge(edge)

    def node(self, node_name):
        '''Find a node by name. GraphView[node_name] also works'''
        return self[node_name]

    def insert_node(self, node_to_insert, from_node_name,
                    to_node_name, from_idx=0, to_idx=0,
                    node_input_idx=0, node_output_idx=0,
                    edge_class=None):
        '''Inserts a node between two existing nodes'''
        if edge_class is None:
            edge_class = Edge
        node_to_insert = resolve_node(node_to_insert)
        from_node_name = resolve_name(from_node_name)
        to_node_name = resolve_name(to_node_name)
        existing_edge = self.edge(
            from_node_name, to_node_name, from_idx=from_idx, to_idx=to_idx)
        self.remove_edge(existing_edge)
        self.add_edge(edge_class(from_node_name, node_to_insert,
                                 from_idx=from_idx, to_idx=node_input_idx))
        self.add_edge(edge_class(node_to_insert, to_node_name,
                                 from_idx=node_output_idx, to_idx=to_idx))

    def edge(self, from_node_name: str, to_node_name: str, from_idx: int = 0, to_idx: int = 0):
        '''Finds first edge between two nodes - WARNING - probably not good in weird situation
        when nodes have multiple connections'''
        edge = self._out_edges.get(from_node_name)
        if edge:
            edge_list = edge.get(to_node_name)
            edges = [i for i in edge_list if (
                i.from_idx == from_idx and i.to_idx == to_idx)]
            if len(edges) > 1:
                raise InternalGraphError()
            return edges[0] if edges else None
        return None

    def successors(self, node_name: str) -> Iterable:
        '''Nodes with outputs from node_name'''
        node_name = resolve_name(node_name)
        edges = self.out_edges(node_name)
        if not edges:
            return edges
        edges.sort(key=lambda x: x.from_idx)
        res = [[]] * (edges[-1].from_idx + 1)
        for edge in edges:
            res[edge.from_idx].append(edge.to_node)
        return res

    def successor_names(self, node_name: str) -> Iterable:
        '''Names of nodes with outputs from node_name'''
        node_name = resolve_name(node_name)
        return set([node.name for edge in self.successors(node_name) for node in edge])

    def predecessors(self, node_name: str) -> Iterable:
        '''Nodes with inputs to node_name'''
        node_name = resolve_name(node_name)
        edges = self.in_edges(node_name)
        if not edges:
            return edges
        edges.sort(key=lambda x: x.to_idx)
        res = [None] * (edges[-1].to_idx + 1)
        for edge in edges:
            assert not res[edge.to_idx], "only one input per index"
            res[edge.to_idx] = edge.from_node
        return res

    def all_predecessors(self, node_name: str, predecessors: Sequence[Node] = None):
        if predecessors is None:
            predecessors = []
        node_name = resolve_name(node_name)
        for node in self.predecessors(node_name):
            if not node or node in predecessors:
                continue
            predecessors.append(node)
            self.all_predecessors(node.name, predecessors=predecessors)
        return predecessors

    def predecessor_names(self, node_name: str) -> Iterable:
        '''Names of nodes with inputs to node_name'''
        node_name = resolve_name(node_name)
        return set([node.name for node in self.predecessors(node_name)])

    def nodes(self, node_classes: Node=None, sort=False) -> Sequence[Node]:
        """Select nodes in the graph filtering by node class.

        Args:
            node_classes (Node, optional): Node classes to filter. Defaults to None.
            sort (bool, optional): Sort nodes by step idx. Defaults to False.

        Returns:
            Sequence[Node]: List of nodes.
        """
        if node_classes is not None:
            nodes = [node for node in self._nodes.values(
            ) if isinstance(node, node_classes)]
        else:
            nodes = list(self._nodes.values())
        if sort:
            nodes.sort(key=lambda x: x.step_idx)
        return nodes

    def contains(self, node):
        return node in self._nodes.values()

    def edges(self, node_name: str = None):
        '''All edges to and from a node or all edges in the graph
        if node_name is None.'''
        if node_name is None:
            return list(edge for edge_out in self._in_edges.values()
                        for edge_list in edge_out.values()
                        for edge in edge_list)
        return set(self.in_edges(node_name)) | set(self.out_edges(node_name))

    def in_edges(self, node_or_name: Union[str, Node]) -> Sequence[Edge]:
        '''Input edges to a node'''
        node_name = resolve_name(node_or_name)
        if node_name not in self._in_edges:
            return []
        return list(edge for edge_list in self._in_edges[node_name].values()
                    for edge in edge_list)

    def in_edges_idx(self, node_or_name: Union[str, Node], to_idx: int) -> Edge:
        '''Input edge at index to a node'''
        node_name = resolve_name(node_or_name)
        if node_name not in self._in_edges:
            return None

        edges = list(edge for edge_list in self._in_edges[node_name].values()
                     for edge in edge_list if edge.to_idx == to_idx)
        return edges[0] if len(edges) == 1 else None

    def out_edge_to(self, node_or_name_from, node_or_name_to):
        node_or_name_from = resolve_name(node_or_name_from)
        node_or_name_to = resolve_name(node_or_name_to)
        for edge in self.out_edges(node_or_name_from):
            if edge.to_node.name == node_or_name_to:
                return edge
        return None

    def connected_nodes(self, node_or_node_name):
        node_or_node_name = resolve_name(node_or_node_name)
        connected_nodes = set(
            edge.from_node for edge in self.in_edges(node_or_node_name))
        connected_nodes |= set(
            out_edge.to_node
            for edge in self.in_edges(node_or_node_name)
            for out_edge in self.indexed_out_edges(edge.from_node)[edge.from_idx]
            if out_edge.to_node.name != node_or_node_name)
        connected_nodes |= set(
            edge.to_node for edge in self.out_edges(node_or_node_name))
        return list(connected_nodes)

    def _old_undirected_dfs(self, node, stop_at, pass_at, visited):
        if node in stop_at or node in visited:
            return
        if pass_at and node not in pass_at:
            return
        visited.add(node)
        yield node
        for edge in self.in_edges(node):
            yield from self._old_undirected_dfs(edge.from_node, stop_at, pass_at, visited)
        for edge in self.out_edges(node):
            yield from self._old_undirected_dfs(edge.to_node, stop_at, pass_at, visited)

    def old_undirected_dfs(self, stop_at=None, start_at=None, pass_at=None):
        if start_at is None:
            start_at = list(self.inputs())
        elif is_iterable(start_at):
            start_at = list(start_at)
        else:
            start_at = [start_at]
        if stop_at is None:
            stop_at = set()
        elif not is_iterable(stop_at):
            stop_at = {stop_at}
        else:
            stop_at = set(stop_at)
        if pass_at is None:
            pass_at = set()
        elif not is_iterable(pass_at):
            pass_at = {stop_at}
        else:
            pass_at = set(pass_at)

        visited = set()
        while start_at:
            yield from self._old_undirected_dfs(start_at.pop(0), stop_at, pass_at, visited)

    def is_vertex_cut(self, node_set):
        start_at = next(
            iter([node for node in self.nodes() if node not in node_set]))
        visited = list(self.old_undirected_dfs(
            start_at=start_at, stop_at=node_set))
        return len(visited) < (len(self) - len(node_set))

    def nodes_between_in(self, node_from, node_to, node_set, start=True):
        """Check that the only nodes between from and to are in node set"""
        if start:
            node_from = resolve_node_or_str(node_from, G=self)
            node_to = resolve_node_or_str(node_to, G=self)
            node_set = set(node_set)
            start = False

        for edge in self.out_edges(node_from):
            if edge.to_node == node_to:
                continue
            if (edge.to_node not in node_set or
                    not self.nodes_between_in(edge.to_node, node_to, node_set, start=start)):
                return False
        return True

    def nodes_between(self, node_from, node_to, visited=None, path=None):
        """Return nodes between node_from and node_to not including those nodes"""
        if visited is None:
            node_from = resolve_node_or_str(node_from, G=self)
            node_to = resolve_node_or_str(node_to, G=self)
            visited = set()
        if path is None:
            path = []
        for edge in self.out_edges(node_from):
            if edge.to_node in visited or edge.to_node == node_to:
                visited.update(path)
            else:
                self.nodes_between(edge.to_node, node_to,
                                   visited=visited, path=path + [edge.to_node])
        return visited

    def paths_between(self, node_from, node_to, path=None, topo=None):
        if topo is None:
            topo = {node: idx for idx, node in enumerate(self.topological_sort())}
            path = []
        if node_from == node_to:
            return path
        if topo[node_from] > topo[node_to]:
            return None
        found_paths = []
        for edge in self.in_edges(node_to):
            up_path = self.paths_between(node_from, edge.from_node, path=[edge] + path, topo=topo)
            if up_path is None:
                continue
            found_paths.append(up_path)
        if not found_paths:
            return None
        if len(found_paths) == 1:
            return found_paths[0]
        return found_paths

    def nodes_below(self, node):
        """Return nodes below node not including node"""
        nodes_below = set(self.directed_dfs(node))
        nodes_above = set(self.undirected_dfs(
            node, start_up=True, stop_down_at=nodes_below))
        return tuple(self.undirected_dfs(node, stop_up_at=nodes_above))

    def nodes_above(self, node, visited=None):
        """Return nodes above node not including node"""
        nodes_above = set(self.directed_dfs(node, go_up=True))
        nodes_below = set(self.undirected_dfs(node, stop_up_at=nodes_above))
        return tuple(self.undirected_dfs(node, stop_down_at=nodes_below, start_up=True))

    def nodes_below_are_class(self, node, classes, visited=None):
        """Check all nodes below are in classes"""
        if visited is None:
            node = resolve_node_or_str(node, G=self)
            visited = set()

        for edge in self.out_edges(node):
            if not isinstance(edge.to_node, classes):
                return False
            visited.add(edge.to_node)
            if not self.nodes_below_are_class(edge.to_node, classes, visited=visited):
                return False
        return True

    def directed_dfs(self,
                     node_or_name: Union[str, Node],
                     stop_at: Optional[Set[Node]] = None,
                     go_up: bool = False,
                     yield_start_node=False,
                     visited=None):
        """Yields all nodes above or below this node searched directed. This is almost a dfs
        since it yields in order going down the graph rather than bottom up

        Args:
            node_or_name (Union[str, Node]): Node or node name to start at
            stop_at (Optional[Set[Node]], optional): Stop at this set of nodes. Defaults to None.
            go_up (bool, optional): Go in an upward direction or downwards if False. Defaults to False.

        Yields:
            Node: Nodes visited
        """
        node = resolve_node_or_str(node_or_name, G=self)
        if visited is None:
            visited = {node}
            started = False
            if stop_at is None:
                stop_at = {}
        else:
            started = True
        if node in stop_at:
            return
        if started or yield_start_node:
            yield node
        if not go_up:
            for edge in self.out_edges(node.name):
                if edge.to_node in visited:
                    continue
                visited.add(edge.to_node)
                yield from self.directed_dfs(edge.to_node, stop_at=stop_at, go_up=go_up, visited=visited)
        if go_up:
            for edge in self.in_edges(node.name):
                if edge.from_node in visited:
                    continue
                visited.add(edge.from_node)
                yield from self.directed_dfs(edge.from_node, stop_at=stop_at, go_up=go_up, visited=visited)

    def connected_groups(self):
        nodes = set(self.nodes())
        groups = []
        while nodes:
            start = nodes.pop()
            group = set(self.undirected_dfs(start))|set(self.undirected_dfs(start, start_up=True))|{start}
            groups.append(group)
            nodes -= group
        return groups

    def undirected_dfs(self,
                       node_or_name: Union[str, Node],
                       stop_at: Optional[Set[Node]] = None,
                       stop_down_at: Optional[Set[Node]] = None,
                       stop_up_at: Optional[Set[Node]] = None,
                       start_up: bool = False,
                       yield_start_node: bool = False,
                       yield_stop_node: bool = False,
                       visited=None):
        """Yields all nodes above or below this node searched undirected. This is almost a dfs
        since it yields in order going down the graph rather than bottom up. It also has a few modes
        where it is edge direction sensitive for stopping

        Args:
            node_or_name (Union[str, Node]): Node or node name to start at
            stop_at (Optional[Set[Node]], optional): Stop at this set of nodes. Defaults to None.
            stop_down_at (Optional[Set[Node]], optional): Stop at this set of nodes going down. Defaults to None.
            stop_up_at (Optional[Set[Node]], optional): Stop at this set of nodes going up. Defaults to None.
            start_up (bool, optional): Start in an upward direction or downwards if False. Defaults to False.

        Yields:
            Node: Nodes visited
        """
        node = resolve_node_or_str(node_or_name, G=self)
        if visited is None:
            visited = {node}
            started = False
            if stop_at is None:
                stop_at = {}
            if stop_down_at is None:
                stop_down_at = {}
            if stop_up_at is None:
                stop_up_at = {}
        else:
            started = True
        if node in stop_at:
            if yield_stop_node:
                yield node
            return
        if started or yield_start_node:
            yield node
        if not start_up or started:
            for edge in self.out_edges(node.name):
                if edge.to_node in visited or edge.to_node in stop_down_at:
                    continue
                visited.add(edge.to_node)
                yield from self.undirected_dfs(edge.to_node, stop_at=stop_at, stop_down_at=stop_down_at,
                                               stop_up_at=stop_up_at, visited=visited)
        if start_up or started:
            for edge in self.in_edges(node.name):
                if edge.from_node in visited or edge.from_node in stop_up_at:
                    continue
                visited.add(edge.from_node)
                yield from self.undirected_dfs(edge.from_node, stop_at=stop_at, stop_down_at=stop_down_at,
                                               stop_up_at=stop_up_at, visited=visited)

    def _topological_sort(self, node: Node, visited_edges):
        yield node
        for edge_bundle in self.indexed_out_edges(node):
            for edge in edge_bundle:
                visited_edges.add(edge)
                if set(self.in_edges(edge.to_node)).issubset(visited_edges):
                    yield from self._topological_sort(edge.to_node, visited_edges)

    def _topological_sort_reversed(self, node: Node, visited_edges):
        yield node
        for edge in reversed(self.indexed_in_edges(node)):
            visited_edges.add(edge)
            if set(self.out_edges(edge.from_node)).issubset(visited_edges):
                yield from self._topological_sort_reversed(edge.from_node, visited_edges)

    def topological_sort(self,
                         start_node_or_nodes: Optional[Union[str,
                                                             Node,
                                                             Sequence[Union[str, Node]]]] = None,
                         reverse: bool = False):
        """[summary]

        Args:
            start_node_or_nodes (Optional[Union[str, Node, Sequence[Union[str, Node]]]], optional):
                Optional start node or nodes. Can also be node names. Defaults to None.
            reverse (bool, optional): Sort from bottom of the graph up. Tries to be a perfect reverse of order.
                Defaults to False.

        Raises:
            ValueError: Bad parameters given

        Yields:
            (Node): Yields nodes in desired sort order
        """
        if start_node_or_nodes is None:
            if reverse:
                nodes = list(reversed(self.outputs()))
            else:
                nodes = self.inputs()
        elif isinstance(start_node_or_nodes, str):
            nodes = [self._nodes[start_node_or_nodes]]
        elif isinstance(start_node_or_nodes, Iterable):
            nodes = [node if isinstance(node, Node) else
                     self[node] for node in start_node_or_nodes]
        else:
            raise ValueError('invalid argument')
        visited_edges = set()
        if reverse:
            while nodes:
                node = nodes.pop(0)
                yield from self._topological_sort_reversed(node, visited_edges)
        else:
            while nodes:
                node = nodes.pop(0)
                yield from self._topological_sort(node, visited_edges)

    def nodes_above_are_class(self, node, classes, visited=None):
        """Check all nodes above are in classes"""
        if visited is None:
            node = resolve_node_or_str(node, G=self)
            visited = set()

        for edge in self.in_edges(node):
            visited.add(edge.from_node)
            if not isinstance(edge.from_node, classes):
                return False
            if not self.nodes_above_are_class(edge.from_node, classes, visited=visited):
                return False
        return True

    @staticmethod
    def index_edges_by_from(edges):
        if not edges:
            return []
        max_from_edge = max(edges, key=lambda x: x.from_idx)
        indexed_edges = [[] for i in range(max_from_edge.from_idx + 1)]
        for edge in edges:
            indexed_edges[edge.from_idx].append(edge)
        return indexed_edges

    @staticmethod
    def index_edges_by_to(edges):
        indexed_edges = []
        for edge in edges:
            if edge.to_idx >= len(indexed_edges):
                indexed_edges += [None] * \
                    (edge.to_idx + 1 - len(indexed_edges))
            if indexed_edges[edge.to_idx]:
                cur_edge = indexed_edges[edge.to_idx]
                raise ValueError(f'{edge.to_node.name} already has an edge '
                                 f'on {edge.to_idx} from {cur_edge.from_node}:{cur_edge.from_idx}')
            assert not indexed_edges[edge.to_idx], "only one connection is allowed"
            indexed_edges[edge.to_idx] = edge
        return indexed_edges

    def indexed_in_edges(self, node_name: str) -> Sequence[Edge]:
        '''Input edges indexed by to_idx. These differ from out edges in that there
        can only be one input per to_idx.'''
        node_name = resolve_name(node_name)
        return self.index_edges_by_to(self.in_edges(node_name))

    def out_edges(self, node_or_name: Union[str, Node]) -> Sequence[Edge]:
        '''Output edges from a node'''
        node_name = resolve_name(node_or_name)
        if node_name not in self._out_edges:
            return []
        return list(edge for edge_list in self._out_edges[node_name].values()
                    for edge in edge_list)

    def indexed_out_edges(self, node_or_name: Union[str, Node]) -> Sequence[Sequence[Edge]]:
        '''Output edges indexed by from_idx'''
        node_name = resolve_name(node_or_name)
        out_edges = self.out_edges(node_name)
        if not out_edges:
            return out_edges
        return self.index_edges_by_from(out_edges)

    def num_in_edges(self, node_or_name: Union[str, Node]) -> int:
        '''Number of input edges to a node'''
        node_name = resolve_name(node_or_name)
        return len(self.in_edges(node_name))

    def num_out_edges(self, node_or_name: Union[str, Node]) -> int:
        '''Number of output edges from a node'''
        node_name = resolve_name(node_or_name)
        return len(self.out_edges(node_name))

    def remove_all(self, nodes: Sequence[Node]):
        """Remove all nodes

        Args:
            nodes (Sequence[Node]): Nodes to remove
        """
        nodes = set(nodes)
        while nodes:
            del_node = nodes.pop()
            if isinstance(del_node, NodeRef):
                del_node = del_node.node
            if del_node.name not in self:
                continue
            self.remove(del_node)

    def remove_below(self, node: Node):
        """Remove the nodes below this node.

        Args:
            node (Node): Remove below this node
        """
        self.remove_all(self.nodes_below(node))

    def remove_above(self, node: Node):
        """Remove the nodes above this node..

        Args:
            node (Node): Remove below this node
        """
        self.remove_all(self.nodes_above(node))

    def keep_between(self, from_node: Node, to_node: Node):
        """Remove all nodes that are not between from_node and to_node

        Args:
            from_node (Node): Remove above this node
            to_node (Node): Remove below this node
        """
        keep_nodes = set(
            self.directed_dfs(
                from_node,
                stop_at={to_node},
                yield_start_node=True)
        ) | set(
            self.directed_dfs(
                to_node,
                stop_at={from_node},
                go_up=True,
                yield_start_node=True))
        self.remove_all(set(self._nodes.values()) - set(keep_nodes))

    def remove(self, node_or_name: Union[str, Node]):
        '''Removes a node and all its connected edges'''
        node_name = resolve_name(node_or_name)
        del self._nodes[node_name]
        node_names_to_look_at = set()
        if node_name in self._in_edges:
            node_names_to_look_at |= set(self._in_edges[node_name].keys())
            del self._in_edges[node_name]
        for from_node_name in node_names_to_look_at:
            if node_name in self._out_edges[from_node_name]:
                del self._out_edges[from_node_name][node_name]
        node_names_to_look_at = set()
        if node_name in self._out_edges:
            node_names_to_look_at |= set(self._out_edges[node_name].keys())
            del self._out_edges[node_name]
        for to_node_name in node_names_to_look_at:
            if node_name in self._in_edges[to_node_name]:
                del self._in_edges[to_node_name][node_name]

    def remove_edge(self, edge: Edge):
        '''Removes an edge from the graph'''
        def edge_match(x):
            return not(x.to_node == edge.to_node and x.from_node == edge.from_node and
                       x.to_idx == edge.to_idx and x.from_idx == edge.from_idx)
        edge_list = self._in_edges[edge.to_node.name][edge.from_node.name]
        self._in_edges[edge.to_node.name][edge.from_node.name]\
            = list(filter(edge_match, edge_list))
        if not self._in_edges[edge.to_node.name][edge.from_node.name]:
            del self._in_edges[edge.to_node.name][edge.from_node.name]
        edge_list = self._out_edges[edge.from_node.name][edge.to_node.name]
        self._out_edges[edge.from_node.name][edge.to_node.name]\
            = list(filter(edge_match, edge_list))
        if not self._out_edges[edge.from_node.name][edge.to_node.name]:
            del self._out_edges[edge.from_node.name][edge.to_node.name]

    def remove_edges(self, edges):
        for edge in edges:
            self.remove_edge(edge)

    def edge_in_graph(self, edge):
        if edge.to_node.name in self._in_edges:
            edges = self._in_edges[edge.to_node.name]
            if edge.from_node.name in edges:
                edges = edges[edge.from_node.name]
                return edge in edges
        return False

    def insert_node_at_edge(self, node, at_edge, edge_class=Edge):
        self.remove_edge(at_edge)
        self.add_edge(edge_class(from_node=at_edge.from_node,
                                 to_node=node, from_idx=at_edge.from_idx))
        self.add_edge(edge_class(from_node=node,
                                 to_node=at_edge.to_node, to_idx=at_edge.to_idx))

    def insert_node_before(self, from_node, to_node, from_idx=0, to_idx=0, from_node_in_idx=0, edge_class=Edge):
        from_node = resolve_node(from_node)
        to_node = resolve_node(to_node)
        in_edges = self.indexed_in_edges(to_node.name)
        if len(in_edges) <= to_idx:
            raise ValueError(f"node {to_node.name} doesn't have edge {to_idx}")
        in_edge = in_edges[to_idx]
        self.remove_edge(in_edge)
        self.add_edge(edge_class(from_node=from_node,
                                 from_idx=from_idx, to_node=to_node, to_idx=to_idx))
        self.add_edge(edge_class(from_node=in_edge.from_node,
                                 from_idx=in_edge.from_idx, to_node=from_node, to_idx=from_node_in_idx))

    def insert_node_after(self, from_node, to_node, from_idx=0, to_idx=0, to_node_out_idx=0, edge_class=Edge):
        from_node = resolve_node(from_node)
        to_node = resolve_node(to_node)
        out_edges = self.indexed_out_edges(from_node.name)
        if len(out_edges) <= from_idx:
            raise ValueError(
                f"node {from_node.name} doesn't have edge {from_idx}")
        out_edges = out_edges[from_idx]
        for edge in out_edges:
            self.remove_edge(edge)
        self.add_edge(edge_class(from_node=from_node,
                                 from_idx=from_idx, to_node=to_node, to_idx=to_idx))
        for edge in out_edges:
            self.add_edge(edge_class(from_node=to_node, from_idx=to_node_out_idx,
                                     to_node=edge.to_node, to_idx=edge.to_idx))

    def remove_and_reconnect(self, node_or_name, edge_class=Edge):
        node_or_name = resolve_name(node_or_name)
        node_name = node_or_name.name if isinstance(
            node_or_name, Node) else node_or_name
        in_edges = self.in_edges(node_name)
        out_edges = self.out_edges(node_name)
        if not all(edge.to_idx == 0 for edge in in_edges):
            raise ValueError(
                f'remove_and_reconnect on {node_name} invalid. in idxes > 0 found.')
        if not all(edge.from_idx == 0 for edge in out_edges):
            raise ValueError(
                f'remove_and_reconnect on {node_name} invalid. out idxes > 0 found.')
        self.remove(node_or_name)
        for in_edge in in_edges:
            for out_edge in out_edges:
                self.add_edge(
                    edge_class(
                        from_node=in_edge.from_node,
                        to_node=out_edge.to_node,
                        from_idx=in_edge.from_idx,
                        to_idx=out_edge.to_idx))

    def replace_node(self, node_name: str, new_node: Node):
        '''Replaces a single node with a new node'''
        node_name = resolve_name(node_name)
        new_node = resolve_node(new_node)
        del self._nodes[node_name]
        self._nodes[new_node.name] = new_node
        for edge in self.out_edges(node_name):
            self.remove_edge(edge)
            self.add_edge(edge.clone(from_node=new_node))
        for edge in self.in_edges(node_name):
            self.remove_edge(edge)
            self.add_edge(edge.clone(to_node=new_node))

    def unique_name(self, prefix):
        if prefix not in self:
            return prefix
        idx = 0
        while f'{prefix}_{idx}' in self:
            idx += 1
        return f'{prefix}_{idx}'

    def replace_fragment(self,
                         frag: 'Graph',
                         new_node: Node,
                         frag_in_edges=None,
                         frag_out_edges=None,
                         edge_in_mapping=None,
                         edge_out_mapping=None,
                         edge_class=None
                         ):
        '''Replaces a fragment with a single node connecting all the in and out
        edges to it. If fragment in and out edges are provided then they are used as
        the boundary of the fragment. The edge in mapping is a list but input index
        of the incoming edges to the new node in the form (from node, from_idx). The edge_out_mapping
        is a list of lists of outgoing edges in the from (to_node, to_idx)'''
        if edge_class is None:
            edge_class = Edge
        if frag_in_edges is None:
            frag_in_edges = [in_edge for input_node in frag.inputs()
                             for in_edge in self.in_edges(input_node.name)]
        if frag_out_edges is None:
            frag_out_edges = [out_edge for output_node in frag.outputs()
                              for out_edge in self.out_edges(output_node.name)]

        for node in frag.nodes():
            if self.contains(node):
                self.remove(node)

        self.add_node(new_node)
        if edge_in_mapping:
            for to_idx, (from_node, from_idx) in enumerate(edge_in_mapping):
                self.add_edge(edge_class(from_node, new_node,
                                         from_idx=from_idx, to_idx=to_idx))
        else:
            connections = set()
            for edge in frag_in_edges:
                if edge.from_pair in connections:
                    continue
                connections.add(edge.from_pair)
                self.add_edge(edge.clone(to_node=new_node))
        if edge_out_mapping:
            for from_idx, edge_bundle in enumerate(edge_out_mapping):
                for (to_node, to_idx) in edge_bundle:
                    self.add_edge(edge_class(new_node, to_node,
                                             from_idx=from_idx, to_idx=to_idx))
        else:
            connections = set()
            for edge in frag_out_edges:
                if edge.to_pair in connections:
                    continue
                connections.add(edge.to_pair)
                self.add_edge(edge.clone(from_node=new_node))

    def remove_fragment(self, frag: 'Graph'):
        '''Removes a fragment and then connects all the input edges that have
        a single from node to all the outputs'''

        nodes_not_in_graph = [
            node_name for node_name in frag if node_name not in self]
        # find the edges in the self graph that point to input nodes that have not been added in
        # the fragment
        frag_in_edges = set(in_edge for input_node in frag.inputs(ignore_names=nodes_not_in_graph)
                            for in_edge in self.in_edges(input_node.name))
        in_nodes = set()
        unique_frag_in_edges = set()
        for edge in frag_in_edges:
            if edge.from_node not in in_nodes:
                unique_frag_in_edges.add(edge)
                in_nodes.add(edge.from_node)
        del frag_in_edges, in_nodes

        frag_out_nodes = set((edge.to_node, edge.to_idx)
                             for frag_out_node in frag.outputs(ignore_names=nodes_not_in_graph)
                             for edge in self.out_edges(frag_out_node.name))
        assert len(frag_out_nodes) == 1, "doesn't work if more than one output"
        frag_out_node = list(frag_out_nodes)[0]

        for node in frag.nodes():
            self.remove(node)

        for edge in unique_frag_in_edges:
            self.add_edge(edge.clone(
                to_node=frag_out_node[0], to_idx=frag_out_node[1]))

    def add_node(self, node: Node):
        if node.name in self._nodes:
            raise ValueError("node already in graph")
        assert node not in self._nodes.values()
        self._nodes[node.name] = node

    def inputs(self, ignore_names=None):
        if ignore_names is None:
            ignore_names = []
        return [self._nodes[node_name] for node_name in self._nodes
                if node_name not in self._in_edges or all(input_name in ignore_names
                                                          for input_name in self._in_edges[node_name])]

    def outputs(self, ignore_names=None):
        if ignore_names is None:
            ignore_names = []
        return [self._nodes[node_name] for node_name in self._nodes
                if node_name not in self._out_edges or all(output_name in ignore_names
                                                           for output_name in self._out_edges[node_name])]

    @staticmethod
    def match_semantics(edges, match_edge):
        for edge in edges:
            if match_edge.from_idx == edge.from_idx and match_edge.to_idx == edge.to_idx:
                return edge
        return None

    def match_down_node(self, matched_graphview, fragment, graph_node, match_node,
                        visited_nodes, start_points, allow_extra_edges=False, exclude_edge=None):
        """Matches a fragment down via out_edges from a node in the graph and corresponding
        node in the fragment"""
        # try to match down. Since a single out edge index can have multiple edges from it
        # bundle them together and try to match any of the matches on that edge index
        match_node_out_edges = self.index_edges_by_from(
            fragment.out_edges(match_node.name))

        # if there are no match edges then this is an exit node from the match
        # so it matches straight away
        if not match_node_out_edges:
            return True

        graph_node_out_edges = self.indexed_out_edges(graph_node.name)
        for edge_group in zip_longest(graph_node_out_edges, match_node_out_edges):
            match_edge_list = edge_group[1]
            # this means that an out_idx on the real graph has no corresponding match edges
            # we've already checked above that this is not an exit node of the match
            if not match_edge_list:
                if not allow_extra_edges:
                    return False
                continue
            # we have a match group but there are no real edges on this index so this
            # doesn't match
            if not edge_group[0]:
                return False
            real_edge_set = set(edge_group[0])
            # each match edge must match one real edge once. real edges that don't match are ignored
            for match_edge in match_edge_list:
                found_real_edge = None
                for real_edge in real_edge_set:
                    if real_edge.to_idx != match_edge.to_idx:
                        continue
                    if exclude_edge == real_edge or self.match_down_edge(matched_graphview, fragment,
                                                                         real_edge, match_edge,
                                                                         visited_nodes, start_points):
                        found_real_edge = real_edge
                        break
                if not found_real_edge:
                    return False
                real_edge_set.remove(found_real_edge)
            # if the match node had edges out from it then for the match to occur they must match all
            # real node edges
            if not allow_extra_edges and real_edge_set:
                return False
        return True

    def match_up_node(self, matched_graphview, fragment, graph_node,
                      match_node, visited_nodes, start_points, allow_extra_edges=False, exclude_edge=None):
        """Matches a fragment up via in_edges from a node in the graph and corresponding
        node in the fragment"""
        # Now try to match up. This is easier because each in idx can only have one edge going to it
        for real_edge in self.in_edges(graph_node.name):
            up_match_edge = self.match_semantics(
                fragment.in_edges(match_node.name), real_edge)
            if up_match_edge is None or up_match_edge == exclude_edge:
                continue
            if isinstance(match_node, MatchNode) and not match_node._match(self, graph_node, real_edge):
                return False
            if not self.match_up_edge(matched_graphview, fragment,
                                      real_edge, up_match_edge, visited_nodes, start_points,
                                      allow_extra_edges=allow_extra_edges):
                return False
        return True

    def match_up_edge(self, matched_graphview, fragment, graph_edge,
                      match_edge, visited_nodes, start_points, allow_extra_edges=False):
        """Matches a fragment up via a specific in_edge"""
        if graph_edge.from_idx != match_edge.from_idx:
            return False
        graph_node = graph_edge.from_node
        if not matched_graphview.contains(graph_node):
            matched_graphview.add_node(graph_node)
        matched_graphview.add_edge(graph_edge.clone())

        if graph_node in visited_nodes:
            return True
        visited_nodes.add(graph_node)

        match_node = match_edge.from_node
        match_node_in_edges = fragment.in_edges(match_node.name)
        graph_node_in_edges = self.out_edges(graph_node.name)

        if len(match_node_in_edges) == 0:
            if isinstance(match_node, MatchNode) and not match_node._match(self, graph_node, None):
                return False
            # since this node can be reached by another start point no point in matching from it
            if graph_node in start_points:
                del start_points[graph_node]
        elif len(match_node_in_edges) != len(graph_node_in_edges):
            return False
        elif not self.match_up_node(matched_graphview, fragment, graph_node,
                                    match_node, visited_nodes, start_points,
                                    allow_extra_edges=allow_extra_edges):
            return False

        if not self.match_down_node(matched_graphview, fragment, graph_node, match_node,
                                    visited_nodes, start_points, allow_extra_edges=allow_extra_edges,
                                    exclude_edge=graph_edge):
            return False
        return True

    def match_down_edge(self, matched_graphview, fragment, graph_edge,
                        match_edge, visited_nodes, start_points, allow_extra_edges=False):
        """Matches a fragment down via a specific out_edge"""
        if graph_edge.to_idx != match_edge.to_idx:
            return False
        graph_node = graph_edge.to_node
        match_node = match_edge.to_node
        if isinstance(match_node, MatchNode) and not match_node._match(self, graph_node, graph_edge):
            return False
        if graph_node in visited_nodes:
            matched_graphview.add_edge(graph_edge.clone())
            return True
        else:
            visited_nodes.add(graph_node)

        matched_graphview.add_node(graph_node)
        matched_graphview.add_edge(graph_edge.clone())

        if not self.match_down_node(matched_graphview, fragment, graph_node, match_node,
                                    visited_nodes, start_points, allow_extra_edges=allow_extra_edges):
            return False

        if not self.match_up_node(matched_graphview, fragment, graph_node, match_node,
                                  visited_nodes, start_points, allow_extra_edges=allow_extra_edges,
                                  exclude_edge=match_edge):
            return False
        return True

    @property
    def _edge_class(self):
        return Edge

    @property
    def _noderef_class(self):
        return NodeRef

    def __eq__(self, other):
        return set(self.nodes()) == set(other.nodes()) and set(self.edges()) == set(other.edges())

    def __hash__(self) -> int:
        return (tuple(self.nodes()), tuple(self.edges())).__hash__()

    def __len__(self):
        return len(self._nodes)

    def __getitem__(self, key):
        return self._nodes[key]  # @IgnoreException

    def __iter__(self):
        return self.nodes().__iter__()

    def __repr__(self) -> str:
        return f'{self.__class__.__name__}()'

class Graph(GraphView):
    pass
