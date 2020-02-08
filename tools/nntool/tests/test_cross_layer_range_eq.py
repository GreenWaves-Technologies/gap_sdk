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

import numpy as np

from importer.importer import create_graph
from execution.execute_graph import execute
from graph.matches.match_gap_conv import MatchAllGapConv
from utils.data_importer import import_data
from quantization import cross_layer_range_eq as cl

def test_cross_mini(two_conv_graph):
    G = two_conv_graph
    output1 = execute(G, [np.full([10, 10, 2], 1)])
    groups, neurons = cl.discover_groups(G)
    assert groups and neurons, "Nothing discovered"
    cl.process_groups(groups)
    cl.update_parameters(neurons)
    output2 = execute(G, [np.full([10, 10, 2], 1)])
    assert np.max(np.abs(output1[3][0] - output2[3][0])) < 0.00001

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

def test_cross_large(vww_graph, vww_images):
    G = create_graph(vww_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(vww_images[4], offset=0, divisor=255)
    output1 = execute(G, [input_tensor])
    groups, neurons = cl.discover_groups(G, do_relun=True)
    group_inputs = [G.in_edges(grp[0][0]['name'])[0].from_node.step_idx for grp in groups]
    group_outputs = [grp[-1][-1]['node'].step_idx for grp in groups]
    assert groups and neurons, "Nothing discovered"
    cl.process_groups(groups, threshold=0.0001)
    cl.update_parameters(neurons)
    output2 = execute(G, [input_tensor])
    assert max([np.max(np.abs(output1[i][0] - output2[i][0])) for i in group_inputs]) < 0.0001
    assert max([np.max(np.abs(output1[i][0] - output2[i][0])) for i in group_outputs]) < 0.0001
    assert np.max(np.abs(output1[-1][0] - output2[-1][0])) < 0.0001
