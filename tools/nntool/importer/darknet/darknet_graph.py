# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from graph.nngraph import NNGraph
from graph.types import FcParameters, ActivationParameters, Conv2DParameters,\
    PoolingParameters, ConcatParameters, MatrixAddParameters, YoloParameters,\
    UpsampleParameters
from graph.dim import PadDim, Conv2DFilterDim, FcFilterDim, StrideDim, PoolFilterDim, ScaleDim

def split_pad(pad):
    pad_oth = pad//2
    pad_full = pad - pad_oth
    return PadDim(pad_full, pad_oth, pad_full, pad_oth)

def read_option(l, current):
    if '=' in l:
        l = l.split('=')
        current[l[0].strip()] = l[1].strip()

def read_cfg(filename):
    file = open(filename, 'r')
    options = []
    current = {}
    for i in file.readlines():
        if i[0] == '[':
            options.append(current.copy())
            current.clear()
            current['type'] = i.split(']')[0][1:]
            continue
        elif(i[0] in ['\0', '#', ';']):
            pass
        else:
            read_option(i, current)

    options.append(current)
    file.close()
    return options[1:]

DARKNET_ACTIVATION_TYPES = {
    "leaky": "leaky",
    "linear": "linear",
    "relu": "relu",
    "logistic": "logistic"
}

def get_str(l, key, default=None, mandatory=True):
    if key not in l:
        if mandatory and default is None:
            raise AttributeError()
        val = default
    else:
        val = l[key]
    return val

def get_int(l, key, default=None, mandatory=True):
    val = get_str(l, key, default, mandatory)
    if val is not None:
        val = int(val)
    return val

def add_fully_connected(out_graph, routes, idx, l):
    activation = get_str(l, 'activation', default="logistic")
    filter_c = get_int(l, 'output')
    node_name = "{}_{}".format(l['type'], idx)
    if activation is None:
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operator(node_name, FcParameters(FcFilterDim(filter_c), has_bias=True))
    else:
        activation = DARKNET_ACTIVATION_TYPES[activation]
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operators(
                node_name,
                [FcParameters(FcFilterDim(filter_c), has_bias=True),\
                    ActivationParameters(activation)]
            )

    return True

def add_convolution(out_graph, routes, idx, l):
    activation = get_str(l, 'activation', default="logistic")
    node_name = "{}_{}".format(l['type'], idx)
    routes['in'][idx] = node_name
    padding = l.get("padding")
    pad = l.get("pad")
    size = get_int(l, 'size', 1)
    groups = get_int(l, 'groups', 1)
    filters_c = get_int(l, 'filters', 1)
    stride = get_int(l, 'stride', 1)
    batch_normalize = get_int(l, 'batch_normalize', 0)
    flipped = get_int(l, 'flipped', 0)
    custom = {'batch_normalize': batch_normalize == 1, 'flipped': flipped == 1}

    assert 'binary' not in l, "Binary convolutions are not implemented"
    assert 'xnor' not in l, "XNOR convolutions are not implemented"
    assert 'dot' not in l, "dot is not implemented"

    # padding calculation as per Darknet code
    if pad is not None:
        padding = int(size/2)
    if padding is None:
        padding = 0

    if activation is None:
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operator(node_name,\
                Conv2DParameters(Conv2DFilterDim(size, size, filters_c),\
                    StrideDim(stride), PadDim(padding), groups=groups,\
                        custom=custom, has_bias=True))
    else:
        activation = DARKNET_ACTIVATION_TYPES[activation]
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operators(
                node_name,
                [
                    Conv2DParameters(Conv2DFilterDim(size, size, filters_c),\
                        StrideDim(stride), PadDim(padding), groups=groups,\
                            custom=custom, has_bias=True),
                    ActivationParameters(activation)
                ]
            )

    return True


DARKNET_POOL_TYPES = {
    "maxpool": "max",
    "avgpool": "average"
}

def add_pool(out_graph, routes, idx, l):
    pool_type = DARKNET_POOL_TYPES[l['type']]
    node_name = "{}_{}".format(l['type'], idx)
    if pool_type == "average":
        # Darknet average pool averages entire channel
        # Pool_w and _h set to None indicates that they must be computed
        # when the network sizes are computed
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operator(node_name,\
                PoolingParameters(None, PoolFilterDim(1), StrideDim(0), pool_type=pool_type))
    else:
        stride = get_int(l, 'stride', default=1)
        size = get_int(l, 'size', default=stride)
        padding = get_int(l, 'padding', default=size-1)
        pad = split_pad(padding)
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operator(node_name,\
                PoolingParameters(PoolFilterDim(size), StrideDim(stride), pad, pool_type=pool_type))

    return True

def add_network_parameters(out_graph, routes, idx, l):
    routes['out'][idx] = out_graph.add_input(get_int(l, 'channels'),\
        get_int(l, 'width'), get_int(l, 'height'), ['c', 'h', 'w'])
    routes['net'] = l
    return False

def add_route(out_graph, routes, idx, l):
    bottoms = l['layers'].split(',')
    node_name = "{}_{}".format(l['type'], idx)
    routes['in'][idx], routes['out'][idx] =\
        out_graph.add_operator(node_name, ConcatParameters('c'))

    for bottom in bottoms:
        bottom = bottom.strip()
        routes['edges'].append([idx+int(bottom) if '-' in bottom else int(bottom)+1, idx])
    return False

def add_default(out_graph, routes, idx, l):
    node_name = "{}_{}".format(l['type'], idx)
    out_graph.add_node(
        node_name,
        darknet_op_name=l['type'],
        darknet_op=l
    )
    routes['in'][idx] = routes['out'][idx] = node_name
    return True

def add_shortcut(out_graph, routes, idx, l):
    node_name = "{}_{}".format(l['type'], idx)
    activation = l.get('activation')
    if activation is None:
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operator(node_name, MatrixAddParameters())
    else:
        activation = DARKNET_ACTIVATION_TYPES[activation]
        routes['in'][idx], routes['out'][idx] =\
            out_graph.add_operators(node_name,\
                [MatrixAddParameters(), ActivationParameters(activation)])

    routes['edges'].append([idx-1, idx])
    routes['edges'].append([int(l['from'])+idx, idx])
    return False

DARKNET_RESIZE_TYPES = {
    "upsample": "bilinear",
}

def add_resize(out_graph, routes, idx, l):
    node_name = "{}_{}".format(l['type'], idx)
    algo = DARKNET_RESIZE_TYPES[l['type']]
    factor = get_int(l, 'stride')
    routes['in'][idx], routes['out'][idx] =\
        out_graph.add_operator(node_name, UpsampleParameters(algo, ScaleDim(factor)))
    return True

def add_yolo(out_graph, routes, idx, l):
    node_name = "{}_{}".format(l['type'], idx)
    classes = get_int(l, 'classes', default=20)
    total = get_int(l, 'num', default=1)
    mask = list(map(int, get_str(l, "mask", default="").split(',')))
    max_boxes = get_int(l, "max", default=90)
    routes['in'][idx], routes['out'][idx] =\
        out_graph.add_operator(node_name, YoloParameters(classes, total, mask, max_boxes))
    return True

SWITCH_ADD_FUNCTIONS = {
    "convolutional": add_convolution,
    "maxpool": add_pool,
    "net": add_network_parameters,
    "route": add_route,
    "shortcut": add_shortcut,
    "connected": add_fully_connected,
    "upsample": add_resize,
    "yolo": add_yolo
}

def get_link(routes, idx):
    node_name = routes[idx]
    if isinstance(node_name, str):
        return node_name

    return get_link(routes, node_name)

def create_subgraph(out_graph, cfg):
    routes = {"in":{}, "out":{}, "edges":[]}
    for i, layer in enumerate(cfg):
        ltype = layer['type']

        if ltype in SWITCH_ADD_FUNCTIONS:
            add_edge = SWITCH_ADD_FUNCTIONS[ltype](out_graph, routes, i, layer)
        else:
            add_edge = add_default(out_graph, routes, i, layer)

        if add_edge:
            routes['edges'].append([i-1, i])

    edge_order = {}
    for edge in routes['edges']:
        # add an order attribute to edges so that we know for
        # concat nodes what order the incoming tensors should
        # be added. Networkx does not guarantee edge order
        order = edge_order.get(edge[1])
        if order is None:
            order = 0
        else:
            order += 1
        edge_order[edge[1]] = order
        out_graph.add_edge(get_link(routes['out'], edge[0]),\
            get_link(routes['in'], edge[1]), order=order)

def create_graph(filename, opts):
    cfg = read_cfg(filename)
    out_graph = NNGraph(model=cfg, filename=filename, name=opts.get('name'),
                        value_cache=opts.get('value_cache'))
    create_subgraph(out_graph, cfg)
    leaf_nodes = list([n for n in out_graph.nodes()\
        if out_graph.out_degree(n) == 0 and out_graph.in_degree(n) > 0])
    for node in leaf_nodes:
        out_graph.add_edge(node, out_graph.add_output(), order=0)
    return out_graph
