from importer.importer import create_graph
from graph.matches.match_gap_conv import MatchAllGapConv

from quantization import cross_layer_range_eq as cl

def test_cross_simple(mnist_graph):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    groups, neurons = cl.discover_groups(G)
    assert groups and neurons, "Nothing discovered"
    cl.process_groups(groups)
    cl.update_parameters(neurons)

def test_cross_fused(mnist_graph):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()
    groups, neurons = cl.discover_groups(G)
    assert groups and neurons, "Nothing discovered"
    cl.process_groups(groups)
    cl.update_parameters(neurons)
