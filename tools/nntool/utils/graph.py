# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
    def __init__(self, name: str):
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

    def __str__(self):
        return "{}[{}]->{}[{}]".format(*self._link)

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
        return sum(1 for edge_out in self._in_edges.values()\
            for edge_list in edge_out.values()\
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
        try:
            edge_idx = edge_list.index(edge)
            if update:
                edge_list[edge_idx] = edge
            else:
                raise EdgeExistsError()
        except ValueError:
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
        try:
            edge_idx = edge_list.index(edge)
            if update:
                edge_list[edge_idx] = edge
            else:
                raise EdgeExistsError()
        except ValueError:
            edge_list.append(edge)

    def verify_edges(self):
        problems = []
        for node_name in self.keys():
            in_idxes = {}
            for edge in self.in_edges(node_name):
                if edge.to_idx in in_idxes:
                    problems.append(\
                        "edge {0!s} connects to the same input as edge {1!s}"\
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
        return set((edge.to_node.name for edge in edge_list)\
            for edge_list in self.successors(node_name))

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
        return set((edge.from_node.name for edge in edge_list)\
            for edge_list in self.predecessors(node_name))

    def nodes(self):
        '''All the nodes in the graph. GraphView.values() also works.'''
        return self._nodes.values()

    def edges(self, node_name: str = None):
        '''All edges to and from a node or all edges in the graph
        if node_name is None.'''
        if node_name is None:
            return list(edge for edge_out in self._in_edges.values()\
                for edge_list in edge_out.values()\
                for edge in edge_list)
        return set(self.in_edges(node_name)) | set(self.out_edges(node_name))

    def in_edges(self, node_name: str) -> Sequence[Edge]:
        '''Input edges to a node'''
        if node_name not in self._in_edges:
            return []
        return list(edge for edge_list in self._in_edges[node_name].values()\
            for edge in edge_list)

    def indexed_in_edges(self, node_name: str) -> Sequence[Edge]:
        '''Input edges indexed by to_idx. These differ from out edges in that there
        can only be one input per to_idx.'''
        in_edges = self.in_edges(node_name)
        in_edges.sort(key=lambda x: x.to_idx)
        indexed_in_edges = [None] * (in_edges[-1].to_idx + 1)
        for edge in in_edges:
            assert not indexed_in_edges[edge.to_idx]
            indexed_in_edges[edge.to_idx] = edge
        return indexed_in_edges

    def out_edges(self, node_name: str) -> Sequence[Edge]:
        '''Output edges from a node'''
        if node_name not in self._out_edges:
            return []
        return list(edge for edge_list in self._out_edges[node_name].values()\
            for edge in edge_list)

    def indexed_out_edges(self, node_name: str) -> Sequence[Sequence[Edge]]:
        '''Output edges indexed by from_idx'''
        out_edges = self.out_edges(node_name)
        if not out_edges:
            return out_edges
        out_edges.sort(key=lambda x: x.from_idx)
        indexed_out_edges = [[]] * (out_edges[-1].from_idx + 1)
        for edge in out_edges:
            indexed_out_edges[edge.from_idx].append(edge)
        return indexed_out_edges

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
        edge_match = lambda x: not(x.to_node == edge.to_node and x.from_node == edge.from_node)
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

    def replace_fragment(self, frag: 'Graph', new_node: Node):
        '''Replaces a fragment with a single node connecting all the in and out
        edges to it'''
        frag_in_edges = [in_edge for input_node in frag.inputs()\
            for in_edge in self.in_edges(input_node.name)]
        frag_out_edges = [out_edge for output_node in frag.outputs()\
            for out_edge in self.out_edges(output_node.name)]

        for node in frag.nodes():
            self.remove(node)

        self.add_node(new_node)
        for edge in frag_in_edges:
            edge._link[2] = new_node
            self.add_edge(edge)
        for edge in frag_out_edges:
            edge._link[0] = new_node
            self.add_edge(edge)

    def remove_fragment(self, frag: 'Graph'):
        '''Removes a fragment and then connects all the input edges that have
        a single from node to all the outputs'''
        frag_in_edges = set(in_edge for input_node in frag.inputs()\
            for in_edge in self.in_edges(input_node.name))
        in_nodes = set()
        unique_frag_in_edges = set()
        for edge in frag_in_edges:
            if edge.from_node not in in_nodes:
                unique_frag_in_edges.add(edge)
                in_nodes.add(edge.from_node)
        del frag_in_edges, in_nodes

        frag_out_nodes = set(edge.to_node for frag_out_node in frag.outputs()\
            for edge in self.out_edges(frag_out_node.name))
        assert len(frag_out_nodes) == 1, "doesn't work if more than one output"
        frag_out_node = list(frag_out_nodes)[0]

        for node in frag.nodes():
            self.remove(node)

        for edge in unique_frag_in_edges:
            edge._link[2] = frag_out_node
            self.add_edge(edge)

    def add_node(self, node: Node):
        if node.name in self._nodes:
            raise ValueError("node already in graph")
        self._nodes[node.name] = node

    def inputs(self):
        return [self._nodes[node_name] for node_name in self._nodes\
            if node_name not in self._in_edges]

    def outputs(self):
        return [self._nodes[node_name] for node_name in self._nodes\
            if node_name not in self._out_edges]

    def __dfs(self, node, condition, visited_nodes, visited_edges, from_node, from_edge):
        if not node:
            return
        if isinstance(node, str):
            node = self._nodes[node]
        if node not in visited_nodes and\
            (from_node is None or\
                all((in_edge in visited_edges) for in_edge in self.in_edges(node.name))) and\
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

    def dfs(self, node_or_name=None, condition=None):
        if node_or_name is None:
            nodes = self.inputs()
        elif isinstance(node_or_name, str):
            nodes = [self._nodes[node_or_name]]
        elif isinstance(node_or_name, Iterable):
            nodes = [input_node if isinstance(input_node, Node) else\
                self[input_node] for input_node in node_or_name]
        else:
            raise TypeError()
        for node in nodes:
            yield from self.__dfs(node, condition, set(), set(), None, None)

    def match_fragment(self, fragment: 'GraphView', node_or_name: Node = None):
        '''looks for a fragment in the graph'''
        edges_in_order = []

        def collect_edges(unused1, from_node, to_node, unused2):
            del unused1, unused2
            nonlocal edges_in_order
            edges_in_order.append((from_node, to_node, False))
            return True

        _ = list(fragment.dfs(condition=collect_edges))


        inputs = fragment.inputs()
        start_points = []
        def match_start_points(G, from_node, to_node, unused1):
            del unused1
            nonlocal inputs, start_points
            edge = None if from_node is None else G.edge(from_node.name, to_node.name)
            if any(isinstance(_input, MatchNode) and _input._match(G, to_node, edge)\
                for _input in inputs):
                start_points.append(to_node.name)
            return True

        _ = list(self.dfs(condition=match_start_points, node_or_name=node_or_name))

        if not start_points:
            return []

        matched_graphview = GraphView()
        order_idx = 0
        has_matched = False

        def match_nodes(G, from_node, to_node, unused1):
            del unused1
            nonlocal edges_in_order, matched_graphview, order_idx, has_matched, fragment
            if has_matched:
                return False
            match = edges_in_order[order_idx]
            matched_this = True

            edge = None if from_node is None else G.edge(from_node.name, to_node.name)
            frag_out = fragment.num_out_edges(match[1].name)

            if match[0]:
                # if this is an input node to the fragment don't match the incoming edge count
                matched_this = fragment.num_in_edges(match[1].name) == G.num_in_edges(to_node.name)
            # if this is an output node from the fragment don't match the outgoing edge count
            if matched_this and frag_out:
                matched_this =\
                    fragment.num_out_edges(match[1].name) == G.num_out_edges(to_node.name)
            # if there is a condition node then run its condition
            if matched_this and isinstance(match[1], MatchNode):
                matched_this = match[1]._match(G, to_node, edge)

            if matched_this:
                if edge:
                    matched_graphview.add_edge(edge)
                else:
                    matched_graphview.add_node(to_node)
                order_idx += 1
                if order_idx >= len(edges_in_order):
                    has_matched = True
            else:
                matched_graphview.clear()
                order_idx = 0
                return False
            # In the case we matched don't bother going further
            # In the case that the fragment has no out nodes then
            # don't proceed (remember this is already one of the only points
            # where the first node matched. All will be explored so it is ok to
            # give up now)
            return not has_matched and frag_out > 0

        matches = []
        for start_point in start_points:
            _ = list(self.dfs(condition=match_nodes, node_or_name=start_point))
            if has_matched:
                matches.append(matched_graphview.clone())
                has_matched = False
            matched_graphview.clear()
            order_idx = 0

        return matches

    def __len__(self):
        return len(self._nodes)

    def __getitem__(self, key):
        return self._nodes[key]

    def __iter__(self):
        return self._nodes.__iter__()

class Graph(GraphView):
    pass
