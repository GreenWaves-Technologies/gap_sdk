# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from graph.manipulations import add_dimensions, calculate_liveness
from graph.matches.matches import get_std_match_group
from graph.types import Parameters, Transposable
from importer.tflite.new_tflite_graph_all import create_graph
from reports.graph_reporter import GraphReporter
from utils.tabular import TextTableRenderer


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


def test_load5(kws_graph):
    G = create_graph(kws_graph, {})
    assert G


def test_load6(vww_graph):
    G = create_graph(vww_graph, {})
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


def test_add_dimension5(kws_graph):
    G = create_graph(kws_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 9)


def test_add_dimension6(vww_graph):
    G = create_graph(vww_graph, {})
    steps = add_dimensions(G)
    verify_steps(steps, 122)


def test_liveness1(mnist_graph):
    G = create_graph(mnist_graph, {})
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 9  # no record for 1 output


def test_liveness2(ir_graph):
    G = create_graph(ir_graph, {})
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 23  # no record for 8 outputs


def test_liveness3(ssd_graph):
    G = create_graph(ssd_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 39  # no record for 1 output


def test_liveness4(cifar10_graph):
    G = create_graph(cifar10_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 15  # no record for 1 output


def test_liveness5(kws_graph):
    G = create_graph(kws_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 8  # no record for 1 output


def test_liveness6(vww_graph):
    G = create_graph(vww_graph, {})
    assert G
    steps = add_dimensions(G)
    liveness = calculate_liveness(G, steps)
    assert len(liveness) == 121  # no record for 1 output


def test_adjust1(mnist_graph):
    G = create_graph(mnist_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust2(ir_graph):
    G = create_graph(ir_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust3(ssd_graph):
    G = create_graph(ssd_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()


def test_adjust4(cifar10_graph):
    G = create_graph(cifar10_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust5(kws_graph):
    G = create_graph(kws_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust6(vww_graph):
    G = create_graph(vww_graph, {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()


def test_adjust7(concat_test_graph):
    G = create_graph(concat_test_graph, {'load_tensors': True})
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
