# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
