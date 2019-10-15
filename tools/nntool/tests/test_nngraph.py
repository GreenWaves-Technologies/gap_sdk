from importer.tflite.new_tflite_graph_all import create_graph
from graph.manipulations import add_dimensions, calculate_liveness
from graph.types import Parameters

def verify_steps(steps, cnt):
    assert len(steps) == cnt
    assert all(isinstance(step['node'], Parameters) for step in steps)

def test_load1(mnist_graph):
    G = create_graph(mnist_graph, {})
    assert G

def test_load2(ir_graph):
    G = create_graph(ir_graph, {})
    assert G

def test_load3(ssd_graph):
    G = create_graph(ssd_graph, {})
    assert G

def test_load4(cifar10_graph):
    G = create_graph(cifar10_graph, {})
    assert G

def test_add_dimension1(mnist_graph):
    G = create_graph(mnist_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 10)

def test_add_dimension2(ir_graph):
    G = create_graph(ir_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 31)

def test_add_dimension3(ssd_graph):
    G = create_graph(ssd_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 40)

def test_add_dimension4(cifar10_graph):
    G = create_graph(cifar10_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 16)

def test_liveness1(mnist_graph):
    G = create_graph(mnist_graph, {})
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 9 # no record for 1 output

def test_liveness2(ir_graph):
    G = create_graph(ir_graph, {})
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 23 # no record for 8 outputs

def test_liveness3(ssd_graph):
    G = create_graph(ssd_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 39 # no record for 1 output

def test_liveness4(cifar10_graph):
    G = create_graph(cifar10_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 15 # no record for 1 output

def test_adjust1(mnist_graph):
    G = create_graph(mnist_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()

def test_adjust2(ir_graph):
    G = create_graph(ir_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()

def test_adjust4(cifar10_graph):
    G = create_graph(cifar10_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()

# TODO - Implement Reshape and Concat for adjust
# def test_adjust3():
#     G = create_graph('examples/New_SSD_Model.tflite', {'load_tensors': True})
#     G.add_dimensions()
#     G.adjust_order()
