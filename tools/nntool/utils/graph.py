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

from itertools import zip_longest

from collections import OrderedDict
from collections.abc import Iterable, Mapping
from typing import Union, Sequence


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
        super(Node, self).__init__(*args, **kwargs)
        self._name = name

    @property
    def name(self):
        '''Node name - must not be changed once node is in graph'''
        return self._name

    def __str__(self):
        return self._name


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


class Edge():
    '''Edge class to inherit for edges'''

    def __init__(self, from_node: Union[str, Node], to_node: Union[str, Node],
                 from_idx: int = 0, to_idx: int = 0):
        self._link = [from_node, from_idx, to_node, to_idx]

    @property
    def from_node(self):
        '''Edge start node'''
        return self._link[0]

    @from_node.setter
    def from_node(self, val):
        '''Edge start node'''
        self._link[0] = val

    @property
    def to_node(self):
        '''Edge end node'''
        return self._link[2]

    @to_node.setter
    def to_node(self, val):
        '''Edge end node'''
        self._link[2] = val

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

    @from_idx.setter
    def from_idx(self, val):
        '''Edge start output index'''
        self._link[1] = val

    @property
    def to_idx(self):
        '''Edge end input index'''
        return self._link[3]

    @to_idx.setter
    def to_idx(self, val):
        '''Edge end input index'''
        self._link[3] = val

    def clone(self):
        return Edge(self.from_node, self.to_node, from_idx=self.from_idx, to_idx=self.to_idx)

    def __eq__(self, value):
        if not isinstance(value, Edge):
            return False
        return all((x == y for x, y in zip(self._link, value._link)))

    def __str__(self):
        return "{}[{}]->{}[{}]".format(*self._link)

    def __hash__(self):
        return str(self).__hash__()

# pylint: disable=too-many-public-methods


class GraphView(Mapping):

    def __init__(self):
        self._out_edges = OrderedDict()
        self._in_edges = OrderedDict()
        self._nodes = OrderedDict()

    @classmethod
    # pylint: disable=unused-argument
    def clone_factory(cls, G):
        return cls()

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

    def num_nodes(self):
        '''Number of nodes len(GraphView) also works'''
        return len(self)

    def num_edges(self):
        '''Number of edges in the graph'''
        return sum(1 for edge_out in self._in_edges.values()
                   for edge_list in edge_out.values()
                   for edge in edge_list)

    def __add_in_edge(self, edge: Edge, update=False):
        edges = self._in_edges.get(edge.to_node.name)
        if not edges:
            edges = {}
            self._in_edges[edge.to_node.name] = edges
        edge_list = edges.get(edge.from_node.name)
        if edge_list is None:
            edge_list = []
            edges[edge.from_node.name] = edge_list
        edge_idx = next((i for i, x in enumerate(edge_list) if x == edge), -1)
        if edge_idx >= 0:
            if update:
                edge_list[edge_idx] = edge
            else:
                raise EdgeExistsError()
        else:
            edge_list.append(edge)

    def __add_out_edge(self, edge: Edge, update=False):
        edges = self._out_edges.get(edge.from_node.name)
        if not edges:
            edges = {}
            self._out_edges[edge.from_node.name] = edges
        edge_list = edges.get(edge.to_node.name)
        if edge_list is None:
            edge_list = []
            edges[edge.to_node.name] = edge_list
        edge_idx = next((i for i, x in enumerate(edge_list) if x == edge), -1)
        if edge_idx >= 0:
            if update:
                edge_list[edge_idx] = edge
            else:
                raise EdgeExistsError()
        else:
            edge_list.append(edge)

    def verify_edges(self):
        problems = []
        for node_name in self.keys():
            in_idxes = {}
            for edge in self.in_edges(node_name):
                if edge.to_idx in in_idxes:
                    problems.append(
                        "edge {0!s} connects to the same input as edge {1!s}"
                        .format(in_idxes[edge.to_idx], edge))
        return problems

    def add_edge(self, edge: Edge):
        '''Adds an edge to the graph'''
        if isinstance(edge.from_node, str):
            edge._link[0] = self._nodes[edge.from_node]
        elif edge.from_node not in self._nodes:
            self._nodes[edge.from_node.name] = edge.from_node
        if isinstance(edge.to_node, str):
            edge._link[2] = self._nodes[edge.to_node]
        elif edge.to_node not in self._nodes:
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
        if isinstance(from_node_name, Node):
            from_node_name = from_node_name.name
        if isinstance(to_node_name, Node):
            to_node_name = to_node_name.name
        existing_edge = self.edge(from_node_name, to_node_name, from_idx=from_idx, to_idx=to_idx)
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
            edges = [i for i in edge_list if (i.from_idx == from_idx and i.to_idx == to_idx)]
            if len(edges) > 1:
                raise InternalGraphError()
            return edges[0] if edges else None
        return None

    def successors(self, node_name: str) -> Iterable:
        '''Nodes with outputs from node_name'''
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
        return set([node.name for edge in self.successors(node_name) for node in edge])

    def predecessors(self, node_name: str) -> Iterable:
        '''Nodes with inputs to node_name'''
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
        for node in self.predecessors(node_name):
            if not node or node in predecessors:
                continue
            predecessors.append(node)
            self.all_predecessors(node.name, predecessors=predecessors)
        return predecessors

    def predecessor_names(self, node_name: str) -> Iterable:
        '''Names of nodes with inputs to node_name'''
        return set([node.name for node in self.predecessors(node_name)])

    def nodes(self):
        '''All the nodes in the graph. GraphView.values() also works.'''
        return self._nodes.values()

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

    def in_edges(self, node_name: str) -> Sequence[Edge]:
        '''Input edges to a node'''
        if node_name not in self._in_edges:
            return []
        return list(edge for edge_list in self._in_edges[node_name].values()
                    for edge in edge_list)

    def in_edges_idx(self, node_name: str, to_idx: int) -> Edge:
        '''Input edge at index to a node'''
        if node_name not in self._in_edges:
            return None

        edges = list(edge for edge_list in self._in_edges[node_name].values()
                     for edge in edge_list if edge.to_idx == to_idx)
        return edges[0] if len(edges) == 1 else None

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
                indexed_edges += [None] * (edge.to_idx + 1 - len(indexed_edges))
            assert not indexed_edges[edge.to_idx], "only one connection is allowed"
            indexed_edges[edge.to_idx] = edge
        return indexed_edges

    def indexed_in_edges(self, node_name: str) -> Sequence[Edge]:
        '''Input edges indexed by to_idx. These differ from out edges in that there
        can only be one input per to_idx.'''
        return self.index_edges_by_to(self.in_edges(node_name))

    def out_edges(self, node_name: str) -> Sequence[Edge]:
        '''Output edges from a node'''
        if node_name not in self._out_edges:
            return []
        return list(edge for edge_list in self._out_edges[node_name].values()
                    for edge in edge_list)

    def indexed_out_edges(self, node_name: str) -> Sequence[Sequence[Edge]]:
        '''Output edges indexed by from_idx'''
        out_edges = self.out_edges(node_name)
        if not out_edges:
            return out_edges
        return self.index_edges_by_from(out_edges)

    def num_in_edges(self, node_name: str) -> int:
        '''Number of input edges to a node'''
        return len(self.in_edges(node_name))

    def num_out_edges(self, node_name: str) -> int:
        '''Number of output edges from a node'''
        return len(self.out_edges(node_name))

    def remove(self, node_or_name: Union[str, Node]):
        '''Removes a node and all its connected edges'''
        node_name = node_or_name.name if isinstance(node_or_name, Node) else node_or_name
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
            return not(x.to_node == edge.to_node and x.from_node == edge.from_node)
        edge_list = self._in_edges[edge.to_node.name][edge.from_node.name]
        self._in_edges[edge.to_node.name][edge.from_node.name]\
            = list(filter(edge_match, edge_list))
        edge_list = self._out_edges[edge.from_node.name][edge.to_node.name]
        self._out_edges[edge.from_node.name][edge.to_node.name]\
            = list(filter(edge_match, edge_list))

    def replace_node(self, node_name: str, new_node: Node):
        '''Replaces a single node with a new node'''
        del self._nodes[node_name]
        self._nodes[new_node.name] = new_node
        for edge in self.out_edges(node_name):
            self.remove_edge(edge)
            edge._link[0] = new_node
            self.add_edge(edge)
        for edge in self.in_edges(node_name):
            self.remove_edge(edge)
            edge._link[2] = new_node
            self.add_edge(edge)

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
                self.add_edge(edge_class(from_node, new_node, from_idx=from_idx, to_idx=to_idx))
        else:
            connections = set()
            for edge in frag_in_edges:
                if edge.from_pair in connections:
                    continue
                connections.add(edge.from_pair)
                edge._link[2] = new_node
                self.add_edge(edge)
        if edge_out_mapping:
            for from_idx, edge_bundle in enumerate(edge_out_mapping):
                for (to_node, to_idx) in edge_bundle:
                    self.add_edge(edge_class(new_node, to_node, from_idx=from_idx, to_idx=to_idx))
        else:
            connections = set()
            for edge in frag_out_edges:
                if edge.to_pair in connections:
                    continue
                connections.add(edge.to_pair)
                edge._link[0] = new_node
                self.add_edge(edge)

    def remove_fragment(self, frag: 'Graph'):
        '''Removes a fragment and then connects all the input edges that have
        a single from node to all the outputs'''
        nodes_not_in_graph = [node_name for node_name in frag if node_name not in self]
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

        frag_out_nodes = set((edge.to_node, edge.to_idx) for frag_out_node in frag.outputs(ignore_names=nodes_not_in_graph)
                             for edge in self.out_edges(frag_out_node.name))
        assert len(frag_out_nodes) == 1, "doesn't work if more than one output"
        frag_out_node = list(frag_out_nodes)[0]

        for node in frag.nodes():
            self.remove(node)

        for edge in unique_frag_in_edges:
            edge._link[2] = frag_out_node[0]
            edge._link[3] = frag_out_node[1]
            self.add_edge(edge)

    def add_node(self, node: Node):
        if node.name in self._nodes:
            raise ValueError("node already in graph")
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

    def __revdfs(self, node, condition, visited_nodes, visited_edges, from_node, from_edge):
        if not node:
            return
        if isinstance(node, str):
            node = self._nodes[node]
        if node not in visited_nodes and\
            (from_node is None or
             all((out_edge in visited_edges) for out_edge in self.out_edges(node.name))) and\
                (not condition or condition(self, from_node, node, from_edge)):

            yield node
            visited_nodes.add(node)
            in_edges = self.in_edges(node.name)
            # Edges are visited in a repeatable order
            in_edges.sort(key=lambda x: str(x.from_idx) + x.from_node.name + str(x.to_idx),
                          reverse=True)
            for edge in in_edges:
                visited_edges.add(edge)

                yield from self.__revdfs(edge.from_node,
                                         condition,
                                         visited_nodes,
                                         visited_edges,
                                         node,
                                         edge)

    def __dfs(self, node, condition, visited_nodes, visited_edges, from_node, from_edge):
        if not node:
            return
        if isinstance(node, str):
            node = self._nodes[node]
        if node not in visited_nodes and \
            (from_node is None or all((in_edge in visited_edges) for in_edge in self.in_edges(node.name))) and \
                (not condition or condition(self, from_node, node, from_edge)):
            yield node
            visited_nodes.add(node)
            out_edges = self.out_edges(node.name)
            # Edges are visited in a repeatable order
            out_edges.sort(key=lambda x: str(x.from_idx) + x.to_node.name + str(x.to_idx))
            for edge in out_edges:
                visited_edges.add(edge)

                yield from self.__dfs(edge.to_node,
                                      condition,
                                      visited_nodes,
                                      visited_edges,
                                      node,
                                      edge)

    def dfs(self, node_or_name=None, condition=None, reverse=False):
        if node_or_name is None:
            if reverse:
                nodes = list(self.outputs())
                # This isn't really necessary but helps with tests
                nodes.reverse()
            else:
                nodes = self.inputs()
        elif isinstance(node_or_name, str):
            nodes = [self._nodes[node_or_name]]
        elif isinstance(node_or_name, Iterable):
            nodes = [node if isinstance(node, Node) else
                     self[node] for node in node_or_name]
        else:
            raise TypeError()

        visited_nodes = set()
        visited_edges = set()
        for node in nodes:
            if reverse:
                yield from self.__revdfs(node, condition, visited_nodes, visited_edges, None, None)
            else:
                yield from self.__dfs(node, condition, visited_nodes, visited_edges, None, None)

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
        match_node_out_edges = self.index_edges_by_from(fragment.out_edges(match_node.name))

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
            up_match_edge = self.match_semantics(fragment.in_edges(match_node.name), real_edge)
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

    def match_fragment(self, fragment: 'GraphView', node_or_name: Node = None, allow_extra_edges=False):
        """Matches a graph fragment against this graph"""
        inputs = fragment.inputs()

        start_points = {}

        def match_start_points(G, from_node, to_node, unused1):
            del unused1
            nonlocal inputs, start_points
            edge = None if from_node is None else G.edge(from_node.name, to_node.name)
            for fragment_input_node in inputs:
                if isinstance(fragment_input_node, MatchNode) and fragment_input_node._match(G, to_node, edge):
                    start_points[to_node] = fragment_input_node
            return True

        _ = list(self.dfs(condition=match_start_points, node_or_name=node_or_name))
        # start points will now be a list of pairs with the start_node name in the graph and the
        # corresponding node in the fragment. The start points have all matched an input none in the
        # fragment

        matched_fragments = []
        while True:
            graph_node = next(start_points.__iter__(), None)
            if graph_node is None:
                return matched_fragments
            match_node = start_points[graph_node]
            del start_points[graph_node]

            matched_graphview = GraphView()
            matched_graphview.add_node(graph_node)
            visited_nodes = set()
            if self.match_down_node(matched_graphview, fragment,
                                    graph_node, match_node,
                                    visited_nodes, start_points,
                                    allow_extra_edges=allow_extra_edges):
                matched_fragments.append(matched_graphview)
        return matched_fragments

    def __eq__(self, other):
        return set(self.nodes()) == set(other.nodes()) and set(self.edges()) == set(other.edges())

    def __len__(self):
        return len(self._nodes)

    def __getitem__(self, key):
        return self._nodes[key]

    def __iter__(self):
        return self._nodes.__iter__()


class Graph(GraphView):
    pass
