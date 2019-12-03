from importer.tflite.new_tflite_graph_all import create_graph
from graph.matches.match_gap_conv import MatchAllGapConv
from graph.matches.matches import get_std_match_group

def test_fusions1(mnist_graph):
    G = create_graph(mnist_graph, {})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()


def test_fusions2(ir_graph):
    G = create_graph(ir_graph, {})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()

def test_fusions3(ssd_graph):
    G = create_graph(ssd_graph, {})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()

def test_fusions4(ssd_graph):
    G = create_graph(ssd_graph, {})
    G.add_dimensions()
    matcher = get_std_match_group()
    matcher.match(G)
    G.add_dimensions()
