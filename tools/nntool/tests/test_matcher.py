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

from importer.tflite.new_tflite_graph_all import TfliteImporter
from utils.graph import Edge, Graph, Node
from utils.graph_matcher import (GraphMatcher, MatchEdgeByIdx,
                                 MatchEdgeInputsGroupFactory, MatchNodeByName,
                                 NodeMatch)
from graph.matches.matscale import MatScalePairMatchFactory, FuseMatScalePair, MatScaleNodeMatch


def test_match1():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test3', 'test4'))
    assert not G.verify_edges()

    fragment = GraphMatcher()
    fragment.add_edge(MatchEdgeByIdx(
        from_node=MatchNodeByName("test1"),
        to_node=MatchNodeByName('test2'))
    )

    res = fragment.match_graph(G)
    assert len(res) == 1
    assert len(res[0]) == 2
    assert res[0].num_edges() == 1


def test_match2(caplog):
    caplog.set_level(logging.DEBUG)
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4'))

    fragment = GraphMatcher()
    n1 = MatchNodeByName('test1')
    fragment.add_edge(MatchEdgeByIdx(from_node=n1, to_node=MatchNodeByName('test2')))
    fragment.add_edge(MatchEdgeByIdx(from_node=n1, to_node=MatchNodeByName('test3')))

    res = fragment.match_graph(G)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2


def test_match3(caplog):
    caplog.set_level(logging.DEBUG)
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    # not the same - look here

    fragment = GraphMatcher()
    n1 = MatchNodeByName('test1')
    fragment.add_edge(MatchEdgeByIdx(from_node=n1, to_node=MatchNodeByName('test2')))
    fragment.add_edge(MatchEdgeByIdx(from_node=n1, to_node=MatchNodeByName('test3')))

    res = fragment.match_graph(G)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2


def test_match4():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_node(Node('test3'))
    G.add_node(Node('test4'))
    G.add_node(Node('test5'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4', to_idx=1))
    G.add_edge(Edge('test4', 'test5'))

    fragment = GraphMatcher()
    m2 = MatchNodeByName("test2")
    m3 = MatchNodeByName('test3')
    m4 = MatchNodeByName('test4')
    fragment.add_edge(MatchEdgeByIdx(from_node=m2, to_node=m4))
    fragment.add_edge(MatchEdgeByIdx(from_node=m3, to_node=m4, to_idx=1))

    res = fragment.match_graph(G)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2


def test_match4alt():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_node(Node('test3'))
    G.add_node(Node('test4'))
    G.add_node(Node('test5'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4', to_idx=1))
    G.add_edge(Edge('test4', 'test5'))

    fragment = GraphMatcher()
    m2 = MatchNodeByName("test2")
    m3 = MatchNodeByName('test3')
    m4 = MatchNodeByName('test4')
    e1 = MatchEdgeInputsGroupFactory()

    fragment.add_edge(e1.get_edge(from_node=m2, to_node=m4))
    fragment.add_edge(e1.get_edge(from_node=m3, to_node=m4))

    res = fragment.match_graph(G)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2


def test_match_fail(caplog):
    caplog.set_level(logging.DEBUG)

    class MatchSuffNode(NodeMatch):
        def __init__(self, suff):
            self._suff = suff
            self._has_matched = False

        def match(self, G, node, state):
            if self._has_matched:
                return False
            if node.name.endswith(self._suff):
                self._has_matched = True
                return True
            return False

        def commit_match(self, G, node, state):
            pass

        def reset_match(self, G, state, node=None, init=False):
            self._has_matched = False

    G = Graph()
    G.add_node(Node("test1a"))
    G.add_edge(Edge('test1a', Node('test2a')))
    G.add_edge(Edge('test2a', Node('test3b')))
    G.add_edge(Edge('test3b', Node('test4a')))

    fragment = GraphMatcher()
    fragment.add_edge(MatchEdgeByIdx(from_node=MatchSuffNode('a'), to_node=MatchSuffNode('b')))

    res = fragment.match_graph(G)
    assert len(res) == 1
    assert res[0].num_nodes() == 2
    assert res[0].num_edges() == 1
    assert set([node.name for node in res[0].nodes()]) == set(['test2a', 'test3b'])


def test_mn3_match1(mn3_graph, caplog):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn3_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    fac = MatScalePairMatchFactory()
    caplog.set_level(logging.DEBUG)
    res = fac.get_matcher().match_graph(G)
    assert len(res) == 8

def test_mn3_fusion1(mn3_graph, caplog):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn3_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    match = FuseMatScalePair()
    caplog.set_level(logging.DEBUG)
    match.match(G)
    G.verify_edges()

def test_mn3_match2(mn3_graph, caplog):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn3_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    caplog.set_level(logging.DEBUG)
    fragment = GraphMatcher(match_function=lambda state, frag: (frag, state['match']))
    fragment.add_node(MatScaleNodeMatch())
    res = fragment.match_graph(G)
    assert len(res) == 16
