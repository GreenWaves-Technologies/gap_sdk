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

from utils.graph import Graph, Node, Edge, MatchNameNode, MatchNode

def test_create1():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', Node('test3')))
    expect = ['test1', 'test2', 'test3']
    assert [node.name for node in G.dfs()] == expect
    expect.reverse()
    assert [node.name for node in G.dfs(reverse=True)] == expect
    assert G.num_in_edges('test1') == 0
    assert G.num_out_edges('test1') == 1
    assert not G.verify_edges()

def test_create2():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test2', Node('test3')))
    assert [node.name for node in G.dfs()] == ['test1', 'test2', 'test3']
    assert not G.verify_edges()

def test_create3():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    assert [node.name for node in G.dfs()] == ['test1', 'test2', 'test3']
    assert not G.verify_edges()

def test_remove_edge():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', Node('test3')))
    assert [node.name for node in G.dfs()] == ['test1', 'test2', 'test3']
    assert G.num_edges() == 2
    edge12 = G.edge('test1', 'test2')
    assert edge12.from_node.name == 'test1' and edge12.from_idx == 0
    assert edge12.to_node.name == 'test2' and edge12.to_idx == 0
    assert not G.verify_edges()
    G.remove_edge(edge12)
    assert G.num_edges() == 1
    assert G.num_in_edges('test1') == 0
    assert G.num_out_edges('test1') == 0
    assert G.num_in_edges('test2') == 0
    assert G.num_out_edges('test2') == 1
    assert G.num_in_edges('test3') == 1
    assert G.num_out_edges('test3') == 0
    assert not G.verify_edges()

def test_create4():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', Node('test3')))
    assert not G.verify_edges()
    G.replace_node('test2', Node('test4'))
    expect = ['test1', 'test4', 'test3']
    assert [node.name for node in G.dfs()] == expect
    expect.reverse()
    assert [node.name for node in G.dfs(reverse=True)] == expect
    assert not G.verify_edges()

def test_create5():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4'))
    assert G.num_in_edges('test4') == 2
    assert G.num_out_edges('test1') == 2
    expect = ['test1', 'test2', 'test3', 'test4']
    assert [node.name for node in G.dfs()] == expect
    expect.reverse()
    assert [node.name for node in G.dfs(reverse=True)] == expect
    assert not G.verify_edges()

def test_match1():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test2', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test3', 'test4'))
    assert not G.verify_edges()

    fragment = Graph()
    fragment.add_node(MatchNameNode("test1"))
    fragment.add_node(MatchNameNode('test2'))
    fragment.add_edge(Edge('test1', 'test2'))
    assert not fragment.verify_edges()

    res = G.match_fragment(fragment)
    assert len(res) == 1
    assert len(res[0]) == 2
    assert res[0].num_edges() == 1

def test_match2():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4'))

    fragment = Graph()
    fragment.add_node(MatchNameNode("test1"))
    fragment.add_node(MatchNameNode('test2'))
    fragment.add_edge(Edge('test1', 'test2'))
    fragment.add_edge(Edge('test1', MatchNameNode('test3')))

    res = G.match_fragment(fragment)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2

def test_match3():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    # not the same - look here

    fragment = Graph()
    fragment.add_node(MatchNameNode("test1"))
    fragment.add_node(MatchNameNode('test2'))
    fragment.add_edge(Edge('test1', 'test2'))
    fragment.add_edge(Edge('test1', MatchNameNode('test3')))

    res = G.match_fragment(fragment)
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

    fragment = Graph()
    fragment.add_node(MatchNameNode("test2"))
    fragment.add_node(MatchNameNode('test3'))
    fragment.add_node(MatchNameNode('test4'))
    fragment.add_edge(Edge('test2', 'test4'))
    fragment.add_edge(Edge('test3', 'test4', to_idx=1))

    res = G.match_fragment(fragment)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2

def test_match_and_replace():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4'))

    fragment = Graph()
    fragment.add_node(MatchNameNode("test1"))
    fragment.add_node(MatchNameNode('test2'))
    fragment.add_edge(Edge('test1', 'test2'))
    fragment.add_edge(Edge('test1', MatchNameNode('test3')))

    res = G.match_fragment(fragment)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2
    G.replace_fragment(res[0], Node('test5'))
    res = list(node.name for node in G.dfs())
    assert res == ['test5', 'test4']
    assert not G.verify_edges()

def test_match_fail():
    class MatchSuffNode(MatchNode):
        def __init__(self, name, suff):
            super().__init__(name)
            self.suff = suff

        def _match(self, G, node, edge):
            return node.name.endswith(self.suff)

    G = Graph()
    G.add_node(Node("test1a"))
    G.add_edge(Edge('test1a', Node('test2a')))
    G.add_edge(Edge('test2a', Node('test3b')))
    G.add_edge(Edge('test3b', Node('test4a')))

    fragment = Graph()
    fragment.add_node(MatchSuffNode('match1', 'a'))
    fragment.add_node(MatchSuffNode('match2', 'b'))
    fragment.add_edge(Edge('match1', 'match2'))

    res = G.match_fragment(fragment)
    assert len(res) == 1
    assert res[0].num_nodes() == 2
    assert res[0].num_edges() == 1
    assert [node.name for node in res[0].nodes()] == ['test2a', 'test3b']

def test_match_and_remove():
    G = Graph()
    G.add_node(Node("test0"))
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test0', 'test1'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4'))
    G.add_edge(Edge('test4', Node('test5')))

    fragment = Graph()
    fragment.add_node(MatchNameNode("test1"))
    fragment.add_node(MatchNameNode('test2'))
    fragment.add_edge(Edge('test1', 'test2'))
    fragment.add_edge(Edge('test1', MatchNameNode('test3')))

    res = G.match_fragment(fragment)
    assert len(res) == 1
    assert res[0].num_nodes() == 3
    assert res[0].num_edges() == 2
    G.remove_fragment(res[0])
    assert [node.name for node in G.dfs()] == ['test0', 'test4', 'test5']
    assert not G.verify_edges()

def test_all_predecessors():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4', to_idx=1))
    preds = set()
    for node in G.all_predecessors('test4'):
        assert node.name not in preds
        preds.add(node.name)
    test_fn = lambda name: set(node.name for node in G.all_predecessors(name))
    assert set(['test1', 'test2', 'test3']) == test_fn('test4')
    assert set(['test1']) == test_fn('test2')
    assert set(['test1']) == test_fn('test3')
    assert not test_fn('test1')

def test_reverse_dfs1():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    expect = ['test1', 'test2', 'test3']
    assert [node.name for node in G.dfs()] == expect
    expect.reverse()
    assert [node.name for node in G.dfs(reverse=True)] == expect

def test_insert():
    G = Graph()
    G.add_node(Node("test1"))
    G.add_node(Node('test2'))
    G.add_edge(Edge('test1', 'test2'))
    G.add_edge(Edge('test1', Node('test3')))
    G.add_node(Node('test4'))
    G.add_edge(Edge('test2', 'test4'))
    G.add_edge(Edge('test3', 'test4', to_idx=2))

    G.insert_node(Node('test5'), 'test2', 'test4')

    nodes = [node.name for node in G.dfs()]
    assert nodes == ['test1', 'test2', 'test5', 'test3', 'test4']
    suc = G.successor_names('test5')
    pred = G.predecessor_names('test5')
    assert len(suc) == 1 and 'test4' in suc
    assert len(pred) == 1 and 'test2' in pred
    assert not G.verify_edges()
