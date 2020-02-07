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

VER_MAJOR = 0
VER_MINOR = 1
VER_REV = 2

def read_floats(file, num):
    return np.fromfile(file, dtype=np.float32, count=num)

def read_ints(file, num):
    return np.fromfile(file, dtype=np.int32, count=num)

def load_connected(file, node, version):
    transpose = version[VER_MAJOR] > 1000 and version[VER_MINOR] > 1000
    params = node['params']
    params.biases = read_floats(file, params.filter.c)
    params.weights = read_floats(file, node['in'].size() * params.filter.c)
    if transpose:
        params.weights.resize(params.filter.c, node['in'].size)
        params.weights = params.weights.transpose()
    else:
        params.weights = params.weights.resize(node['in'].size, params.filter.c)
    params.weights_order = ['sz', 'c']
    params.sz_order = ['c', 'h', 'w']

def load_conv(file, node, _):
    params = node['params']
    params.biases = read_floats(file, params.filter.out_c)
    if params.custom.get('batch_normalize'):
        params.custom['scales'] = read_floats(file, params.filter.out_c)
        params.custom['rolling_mean'] = read_floats(file, params.filter.out_c)
        params.custom['rolling_variance'] = read_floats(file, params.filter.out_c)

    params.weights = read_floats(file, params.get_weights_count())

    if params.custom.get('flipped'):
        params.weights.resize(params.filter.in_c, params.filter.out_c,\
            params.filter.h, params.filter.w)
        params.weights = params.weights.transpose((1, 0, 2, 3))
    else:
        params.weights.resize(params.filter.out_c, params.filter.in_c, params.filter.h,\
            params.filter.w)

    params.weights_order = ['c_out', 'c_in', 'h', 'w']

SWITCH_LOAD_FUNCTIONS = {
    "convolutional": load_conv,
    "connected": load_connected
}

def load_tensors(G, file):
    assert file, "No Darknet tensor file specified"
    version = read_ints(file, 3)

    if (version[VER_MAJOR] * 10 + version[VER_MINOR]) >= 2:
        G.set_attr('darknet_seen', np.fromfile(file, dtype=np.uint64, count=1))
    else:
        G.set_attr('darknet_seen', np.fromfile(file, dtype=np.uint32, count=1))
    G.add_dimensions()

    for i, layer in enumerate(G.model):
        ltype = layer['type']

        if ltype in SWITCH_LOAD_FUNCTIONS:
            node_name = "{}_{}".format(ltype, i)
            SWITCH_LOAD_FUNCTIONS[ltype](file, G.nodes[node_name], version)
