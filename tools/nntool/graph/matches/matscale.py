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

from graph.types import MatrixMulParameters, MatScaleFusionParameters

from utils.graph import GraphView, Edge, Node
from utils.graph_matcher import NodeMatch, MatchNodeByClass, EdgeMatch, GraphMatcher, MatchFinishSuccess

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


def is_channel_only(node, idx):
    dim = node.in_dims[idx]
    return dim.is_named and dim.has_keys(['c', 'h', 'w']) and dim.h == 1 and dim.w == 1


def opposite(edge_idx):
    return 0 if edge_idx == 1 else 1


def is_scalar(node, idx):
    dim = node.in_dims[idx]
    return dim.size() == 1


class MatScalePairMatchFactory():
    class MatchMatMulTwice(MatchNodeByClass):
        def __init__(self, factory):
            super(MatScalePairMatchFactory.MatchMatMulTwice, self).__init__(MatrixMulParameters)
            self._factory = factory

        def match(self, G, node, state):
            if super().match(G, node, state):
                self._factory.num_matches += 1
                if self._factory.num_matches == 2:
                    raise MatchFinishSuccess()
                return True
            return False

        def reset_match(self, G, state, node=None, init=False):
            self._factory.num_matches = 0
            super().reset_match(G, state, node=node, init=init)

    class MatScaleEdgeMatch(EdgeMatch):
        def __init__(self, factory, *args, **kwargs):
            super(MatScalePairMatchFactory.MatScaleEdgeMatch, self).__init__(*args, **kwargs)
            self._factory = factory

        @staticmethod
        def scale_compatible(node_edges):
            """node_edges is list of (node, edge_idx)"""
            scalars = set()
            vectors = set()
            node_edges = set(node_edges)
            for node_edge in node_edges:
                if is_channel_only(node_edge[0], node_edge[1]):
                    vectors.add(node_edge)
                if is_scalar(node_edge[0], node_edge[1]):
                    scalars.add(node_edge)
            if len(scalars) == 3:
                return [scalars.pop(), scalars.pop(), scalars.pop()]
            elif len(scalars) == 2:
                non_scalar = node_edges - scalars
                return [non_scalar.pop(), scalars.pop(), scalars.pop()]
            elif len(scalars) == 1 and len(vectors) == 3:
                vectors = vectors - scalars
                return [vectors.pop(), vectors.pop(), scalars.pop()]
            elif len(scalars) == 1 and len(vectors) == 2:
                non_scalar_vec = node_edges - vectors
                non_scalar = vectors - scalars
                return [non_scalar_vec.pop(), non_scalar.pop(), scalars.pop()]
            else:
                return False

        def match(self, edge: Edge, state):
            LOG.debug("matching mul edge from %s to %s has %s", edge.from_node.name,
                      edge.to_node.name, self._factory.has_matched_pair_edge)
            if self._factory.has_matched_pair_edge:
                LOG.debug("match fail")
                return False
            node_1 = edge.from_node
            node_2 = edge.to_node

            match = self.scale_compatible(
                [(node_1, 0), (node_1, 1), (node_2, opposite(edge.to_idx))])
            if match:
                state['match'] = match
                self._factory.has_matched_pair_edge = True
                LOG.debug("match success")
                return True
            LOG.debug("match fail")
            return False

        def match_down(self, G, edge: Edge, state):
            if not isinstance(edge.to_node, MatrixMulParameters):
                return False
            return self.match(edge, state)

        def match_up(self, G, edge: Edge, state):
            return False

        @property
        def has_matched(self):
            return self._factory.has_matched_pair_edge

        def reset_match(self, G, state, edge=None, init=False):
            self._factory.has_matched_pair_edge = False
            state['match'] = None

    def __init__(self):
        self.has_matched_pair_edge = False
        self.has_matched_other_edge = False

    def get_matcher(self):
        fragment = GraphMatcher(match_function=lambda state, frag: (frag, state['match']))
        node_1 = MatScalePairMatchFactory.MatchMatMulTwice(MatrixMulParameters)
        node_2 = MatScalePairMatchFactory.MatchMatMulTwice(MatrixMulParameters)
        fragment.add_edge(MatScalePairMatchFactory.MatScaleEdgeMatch(self,
                                                                     from_node=node_1,
                                                                     to_node=node_2))
        return fragment


class MatScaleNodeMatch(NodeMatch):
    def __init__(self):
        self._has_matched = False

    def match(self, G, node: Node, state):
        if not isinstance(node, MatrixMulParameters) or self._has_matched:
            return False
        scalars = set()
        vectors = set()
        node_edges = set((node, idx) for idx in [0, 1])
        for node_edge in node_edges:
            if is_channel_only(node_edge[0], node_edge[1]):
                vectors.add(node_edge)
            if is_scalar(node_edge[0], node_edge[1]):
                scalars.add(node_edge)
        self._has_matched = True
        if len(scalars) == 2:
            state['match'] = {'type': 'scalar', 'inputs': [scalars.pop(), scalars.pop()]}
            return True
        if len(scalars) == 1:
            vectors = vectors - scalars
            state['match'] = {'type': 'scalar', 'inputs': [vectors.pop(), scalars.pop()]}
            return True
        if len(vectors) == 2:
            state['match'] = {'type': 'vector', 'inputs': [vectors.pop(), vectors.pop()]}
            return True
        if len(vectors) == 1:
            node_edges = node_edges - vectors
            state['match'] = {'type': 'vector', 'inputs': [node_edges.pop(), vectors.pop()]}
            return True
        self._has_matched = False
        return False

    def reset_match(self, G, state, node=None, init=False):
        self._has_matched = False
        state['match'] = None


class FuseMatScalePair(Matcher):
    NAME = 'fuse_matscale_pair'
    DESCRIPTION = 'Finds pair of Matrix piecewise multiplies and checks if they are compatible ' + \
                  'with a MatScale kernel'
    NEEDS_VALID_DIMENSION = True

    def match(self, G: GraphView, set_identity: bool = True):
        fac = MatScalePairMatchFactory()
        has_modified_graph = False

        for frag, match in fac.get_matcher().match_graph(G):
            match_edges = [G.indexed_in_edges(node.name)[idx] for node, idx in match]
            first_node = frag.inputs()[0]
            last_node = frag.outputs()[0]
            out_edges = G.out_edges(last_node.name)
            for node in frag.nodes():
                G.remove(node)

            input_mapping = MatScaleFusionParameters.get_mapping_from_edges(match_edges)

            fnode = MatScaleFusionParameters(
                "{}_{}_fusion".format(first_node.name, last_node.name),
                fusion_type="vec_scalar", subgraph=frag,
                input_mapping=MatScaleFusionParameters.convert_input_mapping(input_mapping))
            has_modified_graph = True
            G.add_node(fnode)
            fnode.in_dims_hint = [None] * 3

            for idx, edge in enumerate(match_edges):
                edge.to_idx = list(input_mapping[edge.to_node].keys())[0]
                edge.to_node = fnode
                if edge.from_node.out_dims_hint:
                    fnode.in_dims_hint[idx] = edge.from_node.out_dims_hint[edge.from_idx]
                G.add_edge(edge)
            for edge in out_edges:
                edge.from_node = fnode
                G.add_edge(edge)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph


class FuseMatScale(Matcher):
    NAME = 'fuse_matscale'
    DESCRIPTION = 'Finds Matrix piecewise multiply and checks if it is compatible ' + \
                  'with a MatScale kernel'

    def match(self, G: GraphView, set_identity: bool = True):
        fragment = GraphMatcher(match_function=lambda state, frag: (frag, state['match']))
        fragment.add_node(MatScaleNodeMatch())
        has_modified_graph = False
        for frag, match in fragment.match_graph(G):
            match_edges = [G.indexed_in_edges(node.name)[idx] for node, idx in match['inputs']]
            matched_node = list(frag.nodes())[0]
            out_edges = G.out_edges(matched_node.name)
            has_modified_graph = True
            G.remove(matched_node)
            fnode = MatScaleFusionParameters("{}_fusion".format(
                matched_node.name), fusion_type=match['type'], subgraph=frag,
                input_mapping=[[(matched_node, 0)], [(matched_node, 1)]])
            G.add_node(fnode)
            for idx, edge in enumerate(match_edges):
                edge.to_node = fnode
                edge.to_idx = idx
                G.add_edge(edge)
            for edge in out_edges:
                edge.from_node = fnode
                G.add_edge(edge)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
