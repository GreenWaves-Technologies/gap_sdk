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

from graph.manipulations import add_dimensions, calculate_liveness
from graph.matches.matches import get_std_match_group, get_fusion
from graph.types import Parameters, Transposable
from importer.tflite.new_tflite_graph_all import TfliteImporter
from reports.graph_reporter import GraphReporter
from utils.tabular import TextTableRenderer
from utils.node_id import NodeId


def verify_steps(steps, cnt):
    assert len(steps) == cnt
    assert all(isinstance(step['node'], Parameters) for step in steps)


def test_load1(mnist_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mnist_graph, {})
    assert G


def test_load2(ir_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ir_graph, {})
    assert G


def test_load3(ssd_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ssd_graph, {})
    assert G


def test_load4(cifar10_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(cifar10_graph, {})
    assert G


def test_load5(kws_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(kws_graph, {})
    assert G


def test_load6(vww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(vww_graph, {})
    assert G


def test_load7(qvww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(qvww_graph, {'load_tensors': True, 'load_quantization': True})
    for node in G.nodes():
        assert NodeId(node) in G.quantization, "node %s doesn't have a qrec" % (node.name)
    assert G


def test_load10():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/xor.tflite", {'load_tensors': True})
    steps = add_dimensions(G)
    assert G

def test_load12():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/imu.tflite", {'load_tensors': True})
    steps = add_dimensions(G)
    verify_steps(steps, 8)
    assert G


def test_load11():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/ring.tflite", {'load_tensors': True})
    steps = add_dimensions(G)
    verify_steps(steps, 11)
    assert G

def test_add_dimension1(mnist_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mnist_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 10)


def test_add_dimension2(ir_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ir_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 31)


def test_add_dimension3(ssd_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ssd_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 40)


def test_add_dimension4(cifar10_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(cifar10_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 16)


def test_add_dimension5(kws_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(kws_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 9)


def test_add_dimension6(vww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(vww_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 122)


def test_add_dimension7(qvww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(qvww_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 159)


def test_add_dimension8(mn3_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn3_graph, {})
    assert len(list(G.dfs())) == 160
    steps = add_dimensions(G)
    verify_steps(steps, 160)


def test_liveness1(mnist_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mnist_graph, {})
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 9  # no record for 1 output


def test_liveness2(ir_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ir_graph, {})
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 23  # no record for 8 outputs


def test_liveness3(ssd_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ssd_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 39  # no record for 1 output


def test_liveness4(cifar10_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(cifar10_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 15  # no record for 1 output


def test_liveness5(kws_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(kws_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 8  # no record for 1 output


def test_liveness6(vww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(vww_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 121  # no record for 1 output


def test_adjust1(mnist_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mnist_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust2(ir_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ir_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust3(ssd_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ssd_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()


def test_adjust4(cifar10_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(cifar10_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust5(kws_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(kws_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"

# TODO - fix when balance filter is rewritten
# def test_adjust6(vww_graph):
#     tfi = TfliteImporter()
#     G = tfi.create_graph(vww_graph, {'load_tensors': True})
#     G.add_dimensions()
#     G.adjust_order()
#     G.balance_filter(32)


def test_adjust7(concat_test_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(concat_test_graph, {'load_tensors': True})
    G.node('input_1').fixed_order = True
    G.node('output_1').fixed_order = True
    G.node('output_2').fixed_order = True
    G.add_dimensions()
    G.adjust_order()
    matcher = get_std_match_group()
    matcher.match(G)
    G.add_dimensions()
    report = GraphReporter().report(G, None)
    renderer = TextTableRenderer(maxwidth=200)
    print(report.render(renderer))
    report = GraphReporter(split_dims=True).report(G, None)


def test_adjust8(qvww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(qvww_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_fusion("fuse_external_bias")
    matcher.match(G)
    G.add_dimensions()

# def test_adjust9(mn3q2_graph):
#     tfi = TfliteImporter()
#     G = tfi.create_graph(mn3q2_graph, {'load_tensors': True})
#     G.add_dimensions()
#     G.adjust_order()
#     matcher = get_fusion("fuse_external_bias")
#     matcher.match(G)
#     G.add_dimensions()