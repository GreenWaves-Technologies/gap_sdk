# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import pickle
import os

import numpy as np

from execution.execute_graph import execute, execute_iterator
from execution.quantization_mode import QuantizationMode
from importer.importer import create_graph
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
    with open(os.path.join("tests/h5_pickles", os.path.basename(mnist_images[0])+'.pickle'), 'rb') as fp:
        verif = pickle.load(fp)
    assert all([np.max(np.abs(verif[idx][0] - output_[idx][0])) < 0.00001 for idx in range(7)])
    # checking the Flatten layer doesn't work because the layout was not changed in the run tool
    # the layout for the output of the linear layer is a little different
    assert np.max(np.abs(verif[8][0] - output_[7][0].flatten())) < 0.00001
    assert np.array_equal(np.round(output_[-1][0].flatten()), [1, 0, 0, 0, 0, 0, 0, 0, 0, 0])
