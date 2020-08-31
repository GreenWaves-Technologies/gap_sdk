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

import numpy as np

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from graph.manipulations import add_dimensions, calculate_liveness
from graph.matches.matches import (MatchInsertCopies, MatchRnnUnpack,
                                   get_fusion, get_pow2_match_group,
                                   get_scale8_match_group)
from graph.types import (InputParameters, OutputParameters, Parameters,
                         Transposable)
from importer.tflite.new_tflite_graph_all import TfliteImporter
from reports.graph_reporter import GraphReporter
from utils.node_id import NodeId
from utils.tabular import TextTableRenderer


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


def test_load8(mn2_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn2_graph, {'load_tensors': True, 'load_quantization': True})
    for node in G.nodes():
        assert NodeId(node) in G.quantization, "node %s doesn't have a qrec" % (node.name)
    assert G


def test_load9(mn1q_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn1q_graph, {'load_tensors': True, 'load_quantization': True})
    assert G


def test_load10():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/xor.tflite", {'load_tensors': True})
    steps = add_dimensions(G)
    verify_steps(steps, 6)
    assert G


def test_load11():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/ring.tflite", {'load_tensors': True})
    steps = add_dimensions(G)
    verify_steps(steps, 11)
    assert G


def test_load12():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/imu.tflite", {'load_tensors': True})
    steps = add_dimensions(G)
    verify_steps(steps, 8)
    assert G


def test_load13():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/vs_ssd_mobv1_forNNtool_with_detect.tflite",
                         {'load_tensors': True, 'load_quantization': True})
    steps = add_dimensions(G)
    verify_steps(steps, 100)
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


def test_adjust6():
    tfi = TfliteImporter()
    try:
        G = tfi.create_graph("tests/graph/character_recogniction_cnn_ocr.tflite",
                             {'load_tensors': True})
        # This graph has an insance concat which multiplies the output of a linear
        # layer. It will never be supported.
        G.add_dimensions()
        error = False
        G.adjust_order()
    except NotImplementedError:
        error = True
    assert error


def test_adjust_new():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/ocr_cnn_notile_fquant.tflite",
                         {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()


def test_adjust_new2():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/ssdlite_v2_quant_ocr_nopostprocess.tflite",
                         {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G['output_1'].fixed_order = True
    G['output_2'].fixed_order = True
    G.adjust_order()


def test_adjust7(concat_test_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(concat_test_graph, {'load_tensors': True})
    G.node('input_1').fixed_order = True
    G.node('output_1').fixed_order = True
    G.node('output_2').fixed_order = True
    G.add_dimensions()
    G.adjust_order()
    matcher = get_pow2_match_group()
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


def test_adjust9(mn3q_graph, caplog):
    caplog.set_level(logging.INFO)
    tfi = TfliteImporter()
    G = tfi.create_graph(mn3q_graph, {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()


def test_adjust10(caplog):
    caplog.set_level(logging.INFO)
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/ssdlite_v2_quant_ocr_nopostprocess.tflite",
                         {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()


def test_adjust11():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/imu.tflite", {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert all([not (node.transpose_in or node.transpose_out)
                for node in G.nodes() if isinstance(node, Transposable)]), "shouldn't have transposes"


def test_adjust_lstm():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/lstm.tflite", {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert G

def test_adjust_lstm2():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/lstm2.tflite", {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert G

def test_adjust_lstm3():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/lstm3.tflite", {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    assert G

def test_adjust_lstm4():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/lstm4.tflite", {'load_tensors': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    assert G

def test_adjust_lstm5():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/lstm5.tflite", {'load_tensors': True})
    G.add_dimensions()
    for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
        node.fixed_order = 1
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    assert G

def test_adjust_rnn():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/rnn.tflite", {'load_tensors': True})
    G.add_dimensions()
    for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
        node.fixed_order = 1
    G.adjust_order()
    MatchRnnUnpack().match(G)
    G.add_dimensions()
    assert G


def test_adjust_rnn2():
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/lstm3.tflite", {'load_tensors': True})
    G.add_dimensions()
    for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
        node.fixed_order = 1
    G.adjust_order()
    MatchRnnUnpack().match(G)
    G.add_dimensions()
    assert G

def test_adjust_split_concat(split_concat_graph):
    G = split_concat_graph
    G.add_dimensions()
    for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
        node.fixed_order = 1
    G.adjust_order()
    MatchInsertCopies().match(G)
    G.add_dimensions()
    assert G

def test_validate_mn1_float(mn1f_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn1f_graph, {'load_tensors': True})
    G.add_dimensions()
    matcher = get_pow2_match_group()
    matcher.match(G)
    G.add_dimensions()
    input_tensor = np.load('tests/mobv1_valid/COCO_val2014_000000362331_0.npy')
    input_tensor = input_tensor.reshape((224, 224, 3))
    executer = GraphExecuter(G, qrecs=G.quantization)
    routput_tensors = executer.execute([input_tensor])
    output_tensor = np.load('tests/mobv1_valid/output_COCO_val2014_000000362331_0_float.npy')
    assert np.max(np.abs(routput_tensors[-1][0] - output_tensor[0])) < 0.0001


def test_min(mn1q_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn1q_graph, {'load_tensors': True, 'load_quantization': True})


def test_validate_mn1_quantized1(mn1q_graph, mn1f_graph):
    tfi = TfliteImporter()
    Gf = tfi.create_graph(mn1f_graph, {'load_tensors': True})
    Gf.add_dimensions()
    Gf.adjust_order()
    matcher = get_pow2_match_group()
    matcher.match(Gf)
    Gf.add_dimensions()

    tfi = TfliteImporter()
    G = tfi.create_graph(mn1q_graph, {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_pow2_match_group()
    matcher.match(G)
    G.add_dimensions()

    fpnode = Gf.graph_state.steps[2]['node']
    fpcnode = fpnode.contained_filters()[0]
    qpnode = G.graph_state.steps[2]['node']
    qpcnode = qpnode.contained_filters()[0]
    nid = NodeId(qpnode, qpcnode)
    qrec = G.quantization[nid]
    dqbiases = qrec.biases_q.get_dequantized(qpcnode.biases)
    assert np.max(np.abs(fpcnode.biases - dqbiases)) < 0.1
    input_tensor = np.load('tests/mobv1_valid/COCO_val2014_000000362331_0.npy')
    input_tensor = input_tensor.reshape((224, 224, 3)).transpose((2, 0, 1))

    executer = GraphExecuter(Gf)
    foutput_tensors = executer.execute([input_tensor])
    foutput_tensor = np.load('tests/mobv1_valid/output_COCO_val2014_000000362331_0_float.npy')
    assert np.max(np.abs(foutput_tensors[-1][0] - foutput_tensor[0])) < 0.0001

    executer = GraphExecuter(G, qrecs=G.quantization)
    qfroutput_tensors = executer.execute([input_tensor], qmode=QuantizationMode.none())
    assert np.max(np.abs(qfroutput_tensors[-1][0] - foutput_tensor[0])) < 0.2

    executer = GraphExecuter(G, qrecs=G.quantization)
    qroutput_tensors = executer.execute([input_tensor], qmode=QuantizationMode.all_dequantize())

    output_tensor = np.load('tests/mobv1_valid/output_COCO_val2014_000000362331_0_quant.npy')
    # assert np.max(np.abs(qroutput_tensors[-1][0] - output_tensor[0])) < 0.16
    assert np.max(np.abs(qroutput_tensors[-1][0] - output_tensor[0])) < 0.28


def test_validate_mn1_quantized2(mn1q_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mn1q_graph, {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_pow2_match_group()
    matcher.match(G)
    G.add_dimensions()


def test_validate_mn1_dequant_quantfloat(mn1q_graph):
    # load dequantized graph same results as quant graph and float execution
    tfi = TfliteImporter()
    G = tfi.create_graph(mn1q_graph, {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_pow2_match_group()
    matcher.match(G)
    G.add_dimensions()

    Gdq = tfi.create_graph(mn1q_graph, {'load_tensors': True, 'load_dequantized': True})
    Gdq.add_dimensions()
    Gdq.adjust_order()
    matcher = get_pow2_match_group()
    matcher.match(Gdq)
    Gdq.add_dimensions()

    input_tensor = np.load('tests/mobv1_valid/COCO_val2014_000000362331_0.npy')
    input_tensor = input_tensor.reshape((224, 224, 3)).transpose((2, 0, 1))

    executer = GraphExecuter(G, qrecs=G.quantization)
    qfoutput_tensors = executer.execute([input_tensor], qmode=QuantizationMode.none())

    executer = GraphExecuter(Gdq)
    dfoutput_tensors = executer.execute([input_tensor])

    diff_list = [np.abs(df[0] - qf[0]) for df, qf in zip(dfoutput_tensors, qfoutput_tensors)]
    max_diff = [np.max(elem) for elem in diff_list]
    assert max(max_diff) < 0.003


def test_mobv2_quant_asym_tf1_15_vwwvehicle():
    graph = 'tests/mobv2_valid/mobv2_vwwvehicle_quant_asym.tflite'
    tfi = TfliteImporter()
    G = tfi.create_graph(graph, {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
