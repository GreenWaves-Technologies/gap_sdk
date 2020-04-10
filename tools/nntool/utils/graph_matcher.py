import logging
from abc import ABC, abstractmethod, abstractproperty
from copy import copy

from .graph import Edge, GraphView, Node

LOG = logging.getLogger("nntool." + __name__)

class MatchFinishSuccess(Exception):
    pass

class MatchFinishFail(Exception):
    pass

class NodeMatch(ABC):

    @abstractmethod
    def match(self, G, node: Node, state):
        pass

    def commit_match(self, G, node: Node, state):
        pass

    @abstractmethod
    def reset_match(self, G, state, node=None, init=False):
        pass

class MatchAnyNode(NodeMatch):
    def __init__(self):
        self._has_matched = False

    def match(self, G, node: Node, state):
        LOG.debug("match any %s", self._has_matched)
        if self._has_matched:
            return False
        self._has_matched = True
        return True

    def commit_match(self, G, node: Node, state):
        pass

    def reset_match(self, G, state, node=None, init=False):
        self._has_matched = False

class MatchNodeByName(NodeMatch):
    def __init__(self, name):
        self._name = name
        self._has_matched = False

    def match(self, G, node, state):
        LOG.debug("match node %s=%s has %s", node.name, self._name, self._has_matched)
        if self._has_matched:
            return False
        if self._name == node.name:
            self._has_matched = True
            return True
        return False

    def commit_match(self, G, node: Node, state):
        pass

    def reset_match(self, G, state, node=None, init=False):
        self._has_matched = False

class MatchNodeByClass(NodeMatch):
    def __init__(self, cls):
        self._cls = cls
        self._has_matched = False

    def match(self, G, node, state):
        LOG.debug("match node %s=%s has %s", node.__class__.__name__, self._cls.__name__, self._has_matched)
        if self._has_matched:
            LOG.debug("match fail")
            return False
        if isinstance(node, self._cls):
            self._has_matched = True
            LOG.debug("match success")
            return True
        LOG.debug("match fail")
        return False

    def commit_match(self, G, node: Node, state):
        pass

    def reset_match(self, G, state, node=None, init=False):
        self._has_matched = False

class MatchNodeByNameSet(NodeMatch):
    def __init__(self, names):
        self._names = names
        self._seen_names = set()

    def match(self, G, node, state):
        if node.name in self._seen_names:
            return False
        self._seen_names.add(node.name)
        return True

    def commit_match(self, G, node: Node, state):
        pass

    def reset_match(self, G, state, node=None, init=False):
        if node:
            self._seen_names.remove(node.name)
        else:
            self._seen_names = set()

class EdgeMatch(ABC):
    def __init__(self, *args, from_node: NodeMatch = None, to_node: NodeMatch = None, **kwargs):
        self._from_node = from_node
        self._to_node = to_node

    @property
    def from_node(self):
        return self._from_node

    @property
    def to_node(self):
        return self._to_node

    @abstractmethod
    def match_down(self, G, edge: Edge, state):
        pass

    def commit_match_down(self, G, edge: Edge, state):
        pass

    @abstractmethod
    def match_up(self, G, edge: Edge, state):
        pass

    def commit_match_up(self, G, edge: Edge, state):
        pass

    @abstractproperty
    def has_matched(self):
        pass

    @abstractmethod
    def reset_match(self, G, state, edge=None, init=False):
        pass


class MatchEdgeByIdx(EdgeMatch):
    def __init__(self, *args, from_idx=0, to_idx=0, **kwargs):
        self._from_idx = from_idx
        self._to_idx = to_idx
        self._has_matched = False
        super(MatchEdgeByIdx, self).__init__(*args, **kwargs)

    def match(self, edge, state):
        LOG.debug("match edge from %s=%s to %s=%s has %s", edge.from_idx, self._from_idx, edge.to_idx, self._to_idx, self._has_matched)
        res = not self._has_matched and edge.from_idx == self._from_idx and edge.to_idx == self._to_idx
        self._has_matched = True
        return res

    def match_down(self, G, edge, state):
        return self.match(edge, state)

    def match_up(self, G, edge, state):
        return self.match(edge, state)

    @property
    def has_matched(self):
        return self._has_matched

    def reset_match(self, G, state, edge=None, init=False):
        self._has_matched = False


class MatchEdgeInputsGroupFactory():
    class MatchEdgeInputs(EdgeMatch):
        def __init__(self, factory, *args, from_idx=0, **kwargs):
            self._from_idx = from_idx
            self._factory = factory
            super(MatchEdgeInputsGroupFactory.MatchEdgeInputs, self).__init__(*args, **kwargs)

        def match(self, edge, state):
            if edge.from_idx != edge.from_idx or self._factory.has_matched(edge.to_idx):
                return False
            self._factory.add_match(edge.to_idx)
            return True

        def match_down(self, G, edge, state):
            return self.match(edge, state)

        def match_up(self, G, edge, state):
            return self.match(edge, state)

        @property
        def has_matched(self):
            return self._factory.has_matched_all

        def reset_match(self, G, state, edge=None, init=False):
            if edge:
                self._factory.remove_match(edge.to_idx)
            else:
                self._factory.reset_match(state, init)

    def __init__(self, *args, num_inputs=2, **kwargs):
        self._num_inputs = num_inputs
        self._in_matches = set()
        self._num_matches = 0

    def has_matched(self, idx):
        return idx in self._in_matches

    @property
    def has_matched_all(self):
        return self._num_matches == self._num_inputs

    def add_match(self, idx):
        self._in_matches.add(idx)
        self._num_matches += 1

    def remove_match(self, idx):
        self._in_matches.remove(idx)
        self._num_matches -= 1

    def get_edge(self, *args, **kwargs):
        return MatchEdgeInputsGroupFactory.MatchEdgeInputs(self, *args, **kwargs)

    def reset_match(self, state, init):
        self._in_matches = set()
        self._num_matches = 0

class GraphMatcher():
    class GraphMatcherState():
        def __init__(self, G):
            self.G = G
            self.real_edges = set()
            self.real_nodes = set()
            self.match_state = {}

    def __init__(self, match_function=None):
        self._nodes = []
        self._edges = []
        self._out_edges = {}
        self._in_edges = {}
        self._match_function = match_function

    def add_node(self, node: NodeMatch):
        self._nodes.append(node)

    def add_edge(self, edge: EdgeMatch):
        if edge.from_node not in self._nodes:
            self._nodes.append(edge.from_node)
        if edge.to_node not in self._nodes:
            self._nodes.append(edge.to_node)
        edges = self._out_edges.get(edge.from_node)
        if edges is None:
            edges = set()
            self._out_edges[edge.from_node] = edges
        edges.add(edge)
        edges = self._in_edges.get(edge.to_node)
        if edges is None:
            edges = set()
            self._in_edges[edge.to_node] = edges
        edges.add(edge)
        if edge not in self._edges:
            self._edges.append(edge)

    @property
    def inputs(self):
        return [node for node in self._nodes if node not in self._in_edges]

    def reset_match(self, G, match_state):
        for node in self._nodes:
            node.reset_match(G, match_state, init=True)
        for edge in self._edges:
            edge.reset_match(G, match_state, init=True)

    def match_graph(self, G: GraphView):
        res = []
        visited = list(G.nodes())
        for match_node in self.inputs:
            visited_idx = 0
            while visited_idx < len(visited):
                real_node = visited[visited_idx]
                state = GraphMatcher.GraphMatcherState(G)
                self.reset_match(G, state.match_state)
                match = self.match_down_node(state, match_node, real_node)
                if match:
                    LOG.debug("match succeeded on %s nodes", len(state.real_nodes))
                    frag = GraphView()
                    for node in state.real_nodes:
                        if node in visited:
                            visited.remove(node)
                        frag.add_node(node)
                    for edge in state.real_edges:
                        frag.add_edge(edge)
                    if self._match_function:
                        frag = self._match_function(state.match_state, frag)
                    res.append(frag)
                else:
                    visited_idx += 1
        return list(res)

    def match_one_edge_down(self, state, match_edges, real_edge):
        state.real_edges.add(real_edge)
        LOG.debug("match edge down to %s", real_edge.to_node.name)
        for match_edge in match_edges:
            if match_edge.match_down(state.G, real_edge, state.match_state):
                if self.match_down_node(state, match_edge.to_node, real_edge.to_node):
                    match_edge.commit_match_down(state.G, real_edge, state.match_state)
                    return True
                else:
                    match_edge.reset_match(state.G, state.match_state, edge=real_edge)
        return False

    def match_one_edge_up(self, state, match_edges, real_edge):
        state.real_edges.add(real_edge)
        LOG.debug("match edge up to %s", real_edge.from_node.name)
        for match_edge in match_edges:
            if match_edge.match_up(state.G, real_edge, state.match_state):
                if self.match_up_node(state, match_edge.from_node, real_edge.from_node):
                    match_edge.commit_match_up(state.G, real_edge, state.match_state)
                    return True
                else:
                    match_edge.reset_match(state.G, state.match_state, edge=real_edge)
        return False

    def match_up_edges(self, state, match_edges, real_edges):
        real_edges = set(real_edges) - state.real_edges
        return all(self.match_one_edge_up(state, match_edges, real_edge)
                   for real_edge in real_edges) and all(match_edge.has_matched for match_edge in match_edges)

    def match_down_edges(self, state, match_edges, real_edges):
        real_edges = set(real_edges) - state.real_edges
        return all(self.match_one_edge_down(state, match_edges, real_edge)
                   for real_edge in real_edges) and all(match_edge.has_matched for match_edge in match_edges)

    def match_up_node(self, state: GraphMatcherState, match_node: NodeMatch, real_node: Node):
        try:
            if not match_node.match(state.G, real_node, state.match_state):
                return False
            LOG.debug("match up %s", real_node.name)
            state.real_nodes.add(real_node)
            try:
                if match_node in self._in_edges:
                    match_edges = copy(self._in_edges[match_node])
                    in_edges = state.G.in_edges(real_node.name)
                    if not self.match_up_edges(state, match_edges, in_edges):
                        LOG.debug("match up up %s failed", real_node.name)
                        match_node.reset_match(state.G, state.match_state, node=real_node)
                        return False
                if match_node in self._out_edges:
                    match_edges = copy(self._out_edges[match_node])
                    if not self.match_down_edges(state, match_edges, state.G.out_edges(real_node.name)):
                        LOG.debug("match up down %s failed", real_node.name)
                        match_node.reset_match(state.G, state.match_state, node=real_node)
                        return False
            except MatchFinishSuccess:
                pass
        except MatchFinishSuccess:
            state.real_nodes.add(real_node)
        except MatchFinishFail:
            match_node.reset_match(state.G, state.match_state, node=real_node)
            return False

        LOG.debug("match up %s succeeded", real_node.name)
        match_node.commit_match(state.G, real_node, state.match_state)
        return True

    def match_down_node(self, state: GraphMatcherState, match_node: NodeMatch, real_node: Node):
        try:
            if not match_node.match(state.G, real_node, state.match_state):
                return False

            state.real_nodes.add(real_node)
            LOG.debug("match down %s", real_node.name)
            try:
                if match_node in self._out_edges:
                    match_edges = copy(self._out_edges[match_node])
                    if not self.match_down_edges(state, match_edges, state.G.out_edges(real_node.name)):
                        LOG.debug("match down down %s failed", real_node.name)
                        match_node.reset_match(state.G, state.match_state, node=real_node)
                        return False

                if match_node in self._in_edges:
                    match_edges = copy(self._in_edges[match_node])
                    if not self.match_up_edges(state, match_edges, state.G.in_edges(real_node.name)):
                        LOG.debug("match up up %s failed", real_node.name)
                        match_node.reset_match(state.G, state.match_state, node=real_node)
                        return False
            except MatchFinishSuccess:
                pass
        except MatchFinishSuccess:
            state.real_nodes.add(real_node)
        except MatchFinishFail:
            match_node.reset_match(state.G, state.match_state, node=real_node)
            return False

        LOG.debug("match down %s succeeded", real_node.name)
        match_node.commit_match(state.G, real_node, state.match_state)
        return True
