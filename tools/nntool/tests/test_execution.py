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

import os
import pickle
import tempfile

import numpy as np

from execution.execute_graph import execute, execute_iterator
from execution.quantization_mode import QuantizationMode
from generation.code_generator import CodeGenerator
from generation.default_template import default_template
from generation.naming_convension import DefaultNamingConvension
from graph.matches.matches import get_std_match_group
from importer.importer import create_graph
from quantization.simple_auto_quantify import SimpleQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.filter_stats_collector import FilterStatsCollector
from utils.data_importer import import_data
from utils.new_param_state import load_state


def test_graph_calc(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    normal_steps = 0
    fusion_steps = 0
    # pylint: disable=unused-variable
    for step_idx, step, node, output, fusion_op_name, fusion_params, details in\
        execute_iterator(G, [input_tensor]):
        if fusion_op_name is not None:
            fusion_steps += 1
        else:
            normal_steps += 1
    assert normal_steps == 10 and fusion_steps == 0

def test_graph_calc_iterator_cached(value_cache, mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    normal_steps = 0
    fusion_steps = 0
    # pylint: disable=unused-variable
    for step_idx, step, node, output, fusion_op_name, fusion_params, details in\
        execute_iterator(G, [input_tensor], value_cache=value_cache):
        if fusion_op_name is not None:
            fusion_steps += 1
        else:
            normal_steps += 1
    assert normal_steps == 10 and fusion_steps == 0

def test_graph_calc_quantized16(value_cache, mnist_unfused_16bit_state, mnist_images):
    G = load_state(mnist_unfused_16bit_state, value_cache=value_cache)
    input_tensor = import_data(mnist_images[0], height=28, width=28, divisor=255, offset=0)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output1 = execute(G, [input_tensor], limit=7)
    input_tensor = import_data(mnist_images[0], height=28, width=28, divisor=255, offset=0)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = execute(G, [input_tensor], qrecs=G.quantization, limit=7, dequantize=True)
    diffs = []
    for i in range(8):
        diffs.append(output1[i][0] - output2[i][0])
    assert np.max(np.abs(diffs[7])) < 0.03

def test_graph_calc_quantized8(value_cache, mnist_unfused_8bit_state, mnist_images):
    G = load_state(mnist_unfused_8bit_state, value_cache=value_cache)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output1 = execute(G, [input_tensor], limit=7)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = execute(G, [input_tensor], qrecs=G.quantization, limit=7, dequantize=True)
    diffs = []
    for i in range(8):
        diffs.append(output1[i][0] - output2[i][0])
    assert np.max(np.abs(diffs[7])) < 9

def test_graph_calc_quantize_one(value_cache, mnist_unfused_16bit_state, mnist_images):
    G = load_state(mnist_unfused_16bit_state, value_cache=value_cache)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output1 = execute(G, [input_tensor])
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = execute(G, [input_tensor], qmode=QuantizationMode.step(1), qrecs=G.quantization)
    diffs = []
    for i, out1 in enumerate(output1):
        diffs.append(out1[0] - output2[i][0])
    assert np.min(diffs[7]) > -2 and np.max(diffs[7]) < 2

def test_graph_calc_quantize_one_2(value_cache, mnist_unfused_16bit_state, mnist_images):
    G = load_state(mnist_unfused_16bit_state, value_cache=value_cache)
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output1 = execute(G, [input_tensor])
    input_tensor = import_data(mnist_images[0], height=28, width=28, offset=0, divisor=255)
    input_tensor = input_tensor.reshape((28, 28, 1))
    output2 = execute(G, [input_tensor], qmode=QuantizationMode.step(4), qrecs=G.quantization)
    diffs = []
    for i, out1 in enumerate(output1):
        diffs.append(out1[0] - output2[i][0])
    assert np.min(diffs[7]) > -2 and np.max(diffs[7]) < 2

def test_graph_execute_complex(ir_graph, ir_images):
    G = create_graph(ir_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(ir_images[0], offset=0, divisor=255)
    input_tensor = input_tensor.reshape((80, 80, 1))
    execute(G, [input_tensor])

def test_graph_kws(kws_graph, kws_sounds):
    G = create_graph(kws_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(kws_sounds[0], offset=0, divisor=128, nptype='int16')
    normal_steps = 0
    fusion_steps = 0
    # pylint: disable=unused-variable
    for step_idx, step, node, output, fusion_op_name, fusion_params, details in\
        execute_iterator(G, [input_tensor]):
        if fusion_op_name is not None:
            fusion_steps += 1
        else:
            normal_steps += 1
    assert normal_steps == 9 and fusion_steps == 0

def test_graph_kws_auto_quant(kws_graph, kws_sounds):
    G = create_graph(kws_graph, opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    get_std_match_group().match(G)
    G.add_dimensions()
    stats_collector = ActivationStatsCollector()
    for input_file in kws_sounds:
        data = import_data(input_file, offset=0, divisor=256, nptype='int16')
        stats_collector.collect_stats(G, [data])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SimpleQuantizer(astats, fstats, force_width=16)
    qrecs = quantizer.quantize(G)
    G.quantization = qrecs

def test_fake_values_concat(concat_test_graph):
    G = create_graph(concat_test_graph, opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_std_match_group()
    matcher.match(G)
    G.add_dimensions()
    G.constant_store.fake = True
    stats_collector = ActivationStatsCollector()
    stats_collector.collect_stats(G, [np.random.rand(*node.dims.shape) for node in G.input_nodes()])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SimpleQuantizer(astats, fstats, force_width=8)
    qrecs = quantizer.quantize(G)
    G.quantization = qrecs
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': '0',
            'at_ver': 3,
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        print(default_template(G, code_generator=code_gen))
        code_gen.write_constants()

# This test requires make test_files to be run in the sample project
# directory. With the 8 bit config

def test_equivalence(mnist_graph, mnist_images):
    G = create_graph(mnist_graph, opts={"load_tensors":True})
    G.add_dimensions()
    G.adjust_order()
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28,
                               divisor=255, offset=0, transpose=False)
    output_ = execute(G, [input_tensor])
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
