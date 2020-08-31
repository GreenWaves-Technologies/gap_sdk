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
import os
import pickle
import tempfile

import numpy as np

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from generation.code_generator import CodeGenerator
from generation.default_template import default_template
from generation.naming_convension import DefaultNamingConvension
from graph.matches.matches import get_pow2_match_group, get_scale8_match_group
from graph.matches.match_rnn_unpack import MatchRnnUnpack
from graph.types import InputParameters, OutputParameters, RNNParameters
from importer.importer import create_graph
from quantization.symmetric.symmetric_quantizer import SymmetricQuantizer
from quantization.multiplicative.mult_quantizer import MultQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.activation_ranges_collector import ActivationRangesCollector
from stats.filter_stats_collector import FilterStatsCollector
from utils.data_importer import import_data
from utils.new_param_state import load_state
from interpreter.commands.imageformat import insert_formatter


def test_graph_calc(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    normal_steps = 0
    fusion_steps = 0
    # pylint: disable=unused-variable
    executer = GraphExecuter(G)
    for step_idx, pnode, fnode, output_tensors, details in\
        executer.execute_iterator([input_tensor]):
        if fnode is not None:
            fusion_steps += 1
        else:
            normal_steps += 1
    assert normal_steps == 10 and fusion_steps == 0

def test_graph_calc_quantized16(mnist_unfused_16bit_state, mnist_images):
    G = load_state(mnist_unfused_16bit_state)
    input_tensor = import_data(mnist_images[0], height=28, width=28, divisor=255, offset=0)
    input_tensor = input_tensor.reshape((28, 28, 1))
    executer = GraphExecuter(G, qrecs=G.quantization)
    output1 = executer.execute([input_tensor], step_idx_limit=7)
    input_tensor = import_data(mnist_images[0], height=28, width=28, divisor=255, offset=0)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = executer.execute([input_tensor], qmode=QuantizationMode.all_dequantize(), step_idx_limit=7)
    diffs = []
    for i in range(8):
        diffs.append(output1[i][0] - output2[i][0])
    assert np.max(np.abs(diffs[7])) < 0.03

def test_graph_calc_quantized8(mnist_unfused_8bit_state, mnist_images):
    G = load_state(mnist_unfused_8bit_state)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    executer = GraphExecuter(G, qrecs=G.quantization)
    output1 = executer.execute([input_tensor], step_idx_limit=7)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = executer.execute([input_tensor], qmode=QuantizationMode.all_dequantize(), step_idx_limit=7)
    diffs = []
    for i in range(8):
        diffs.append(output1[i][0] - output2[i][0])
    assert np.max(np.abs(diffs[7])) < 9

def test_graph_calc_quantize_one(mnist_unfused_16bit_state, mnist_images):
    G = load_state(mnist_unfused_16bit_state)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    executer = GraphExecuter(G, qrecs=G.quantization)
    output1 = executer.execute([input_tensor])
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = executer.execute([input_tensor], qmode=QuantizationMode.step(1))
    diffs = []
    for i, out1 in enumerate(output1):
        diffs.append(out1[0] - output2[i][0])
    assert np.min(diffs[7]) > -2 and np.max(diffs[7]) < 2

def test_graph_calc_quantize_one_2(mnist_unfused_16bit_state, mnist_images):
    G = load_state(mnist_unfused_16bit_state)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    executer = GraphExecuter(G, qrecs=G.quantization)
    output1 = executer.execute([input_tensor])
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = executer.execute([input_tensor], qmode=QuantizationMode.step(4))
    diffs = []
    for i, out1 in enumerate(output1):
        diffs.append(out1[0] - output2[i][0])
    assert np.min(diffs[7]) > -2 and np.max(diffs[7]) < 2

def test_graph_calc_quantized8_qnoq(mnist_unfused_8bit_state, mnist_images):
    G = load_state(mnist_unfused_8bit_state)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    executer = GraphExecuter(G, qrecs=G.quantization)
    diffs = []
    for step_idx, pnode, output, details, qoutput, qdetails, fnode in\
        executer.execute_qnoq_iterator([input_tensor]):
        del step_idx, pnode, details, qdetails, fnode
        diffs.append(output[0] - qoutput[0])
    assert np.max(np.abs(diffs[7])) < 9

def test_graph_execute_complex(ir_graph, ir_images):
    G = create_graph(ir_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(ir_images[0], offset=0, divisor=255)
    input_tensor = input_tensor.reshape((80, 80, 1))
    executer = GraphExecuter(G)
    executer.execute([input_tensor])

def test_graph_kws(kws_graph, kws_sounds):
    G = create_graph(kws_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(kws_sounds[0], offset=0, divisor=128, nptype='int16')
    normal_steps = 0
    fusion_steps = 0
    # pylint: disable=unused-variable
    executer = GraphExecuter(G)
    for step_idx, node, fnode, output_tensors, details in\
        executer.execute_iterator([input_tensor]):
        if fnode is not None:
            fusion_steps += 1
        else:
            normal_steps += 1
    assert normal_steps == 9 and fusion_steps == 0

def test_graph_kws_auto_quant(kws_graph, kws_sounds):
    G = create_graph(kws_graph, opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    get_pow2_match_group().match(G)
    G.add_dimensions()
    stats_collector = ActivationStatsCollector()
    for input_file in kws_sounds:
        data = import_data(input_file, offset=0, divisor=256, nptype='int16')
        stats_collector.collect_stats(G, [data])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SymmetricQuantizer(astats, fstats, force_width=16)
    qrecs = quantizer.quantize(G)
    G.quantization = qrecs

def test_graph_imu_auto_quant_and_execute_quant():
    G = create_graph("tests/graph/imu.tflite", opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    get_pow2_match_group().match(G)
    G.add_dimensions()
    stats_collector = ActivationStatsCollector()
    for input_file in ['tests/images/imu0.pgm']:
        input_tensor = import_data(input_file, offset=0, divisor=256, nptype='int16')
        stats_collector.collect_stats(G, [input_tensor])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SymmetricQuantizer(astats, fstats, force_width=16)
    qrecs = quantizer.quantize(G)
    G.quantization = qrecs
    executer = GraphExecuter(G, qrecs=qrecs)
    for input_file in ['tests/images/imu0.pgm']:
        input_tensor = import_data(input_file, offset=0, divisor=256, nptype='int16')
        output_ = executer.execute([input_tensor], qmode=QuantizationMode.all())

def test_fake_values_concat(concat_test_graph):
    G = create_graph(concat_test_graph, opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_pow2_match_group()
    matcher.match(G)
    G.add_dimensions()
    G.constant_store.fake = True
    stats_collector = ActivationStatsCollector()
    stats_collector.collect_stats(G, [np.random.rand(*node.dims.shape) for node in G.input_nodes()])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SymmetricQuantizer(astats, fstats, force_width=8)
    qrecs = quantizer.quantize(G)
    G.quantization = qrecs
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'at_ver': 3,
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        print(default_template(G, code_generator=code_gen))
        code_gen.write_constants()

# # This test requires make test_files to be run in the sample project
# # directory. With the 8 bit config

def test_equivalence(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28,
                               divisor=255, offset=0, transpose=False)
    executer = GraphExecuter(G)
    output_ = executer.execute([input_tensor])
    with open("tests/h5_pickles/weights.pickle", 'rb') as fp:
        verif_weights = pickle.load(fp)
    assert np.array_equal(verif_weights[0]['weights'], G.graph_state.steps[1]['node'].weights)
    assert np.array_equal(verif_weights[0]['biases'], G.graph_state.steps[1]['node'].biases)
    assert np.array_equal(verif_weights[3]['weights'], G.graph_state.steps[4]['node'].weights)
    assert np.array_equal(verif_weights[3]['biases'], G.graph_state.steps[4]['node'].biases)
    assert np.array_equal(verif_weights[7]['weights'], G.graph_state.steps[7]['node'].weights)
    assert np.array_equal(verif_weights[7]['biases'], G.graph_state.steps[7]['node'].biases)
    with open(os.path.join("tests/h5_pickles",
                           os.path.basename(mnist_images[0])+'.pickle'), 'rb') as fp:
        verif = pickle.load(fp)
    assert all([np.max(np.abs(verif[idx][0] - output_[idx][0])) < 0.00001 for idx in range(7)])
    # checking the Flatten layer doesn't work because the layout was not changed in the run tool
    # the layout for the output of the linear layer is a little different
    assert np.max(np.abs(verif[8][0] - output_[7][0].flatten())) < 0.00001
    assert np.array_equal(np.round(output_[-1][0].flatten()), [1, 0, 0, 0, 0, 0, 0, 0, 0, 0])

def test_rnn_quantize(caplog):
    caplog.set_level(logging.INFO)
    G = create_graph("tests/graph/rnn.tflite", opts={"load_tensors":True})
    G.add_dimensions()

    for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
        node.fixed_order = 1
    
    G.adjust_order()
    input_node = G.inputs()[0]
    out_edge = G.out_edges(input_node.name)[0]
    G.changes.image_format(input_node.name, "bw8", "shift_int8")
    insert_formatter(G, out_edge, "bw8", "shift_int8")
    G.add_dimensions()
    get_scale8_match_group().match(G)
    G.add_dimensions()
    stats_collector = ActivationRangesCollector()
    for input_file in ['tests/images/1270_4.pgm']:
        input_tensor = import_data(input_file)
        res = stats_collector.collect_stats(G, [input_tensor])
    quantizer = MultQuantizer(res, force_width=8, quantized_dimension='channel')
    qrecs = G.quantization = quantizer.quantize(G)
    executer = GraphExecuter(G, qrecs=qrecs)
    for input_file in ['tests/images/1270_4.pgm']:
        input_tensor = import_data(input_file)
        output_ = executer.execute([input_tensor], qmode=QuantizationMode.all())

    for node in [n for n in G.nodes() if isinstance(n, RNNParameters)]:
        node.at_options.PARALLELFEATURES = 1
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'at_ver': 3,
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        print(default_template(G, code_generator=code_gen))
        code_gen.write_constants()

def test_lstm_quantize():
    G = create_graph("tests/graph/lstm.tflite", opts={"load_tensors":True})
    G.add_dimensions()

    for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
        node.fixed_order = 1
    
    G.adjust_order()
    input_node = G.inputs()[0]
    out_edge = G.out_edges(input_node.name)[0]
    G.changes.image_format(input_node.name, "bw8", "shift_int8")
    insert_formatter(G, out_edge, "bw8", "shift_int8")
    G.add_dimensions()
    get_scale8_match_group().match(G)
    G.add_dimensions()
    stats_collector = ActivationRangesCollector()
    for input_file in ['tests/images/1270_4.pgm']:
        input_tensor = import_data(input_file)
        res = stats_collector.collect_stats(G, [input_tensor])
    quantizer = MultQuantizer(res, force_width=8, quantized_dimension='channel')
    qrecs = G.quantization = quantizer.quantize(G)
    executer = GraphExecuter(G, qrecs=qrecs)
    for input_file in ['tests/images/1270_4.pgm']:
        input_tensor = import_data(input_file)
        output_ = executer.execute([input_tensor], qmode=QuantizationMode.all())

    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'at_ver': 3,
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        print(default_template(G, code_generator=code_gen))
        code_gen.write_constants()
