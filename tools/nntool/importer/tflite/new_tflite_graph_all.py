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

# Copied from TOCO code to reference orders
# // Helper to deal with TensorFlow arrays using a different ordering of
# // dimensions
# // ("axes") than our own.
# // we should have associative arrays mapping symbolic axes identifiers (like
# // "output_depth") to dimensions. We would then not need this anymore.
# enum class AxesOrder {
#   kOneAxis,  // one-dimensional array, one unique axis.
#   kCR,       // column-major matrix storage order. Our standard.
#   kRC,       // row-major matrix storage order. TensorFlow default.
#   kOHWI,     // Our standard for conv weights
#   kHWIO,     // TensorFlow conv weights
#   k1HWO,     // Our standard for DepthwiseConv weights
#   kHWIM,     // TensorFlow DepthwiseConv weights
#   kNHWC,     // TensorFlow activations
#   kHWOI,     // TensorFlow back-prop conv weights
# };

import logging
from functools import reduce

import numpy as np

from graph.types import (ActivationParameters, ConcatParameters,
                         Conv2DParameters, FcParameters, PoolingParameters,
                         ReshapeParameters, SoftMaxParameters,
                         UnknownOpParameters, UnconvertedOpParameters,
                         NNEdge, PadParameters, MatrixAddParameters)
from graph.dim import (Conv2DFilterDim, Dim, FcFilterDim, PadDim,
                       PoolFilterDim, StrideDim)
from graph.nngraph import NNGraph
from graph.constant_store import ConstantStore
from utils.graph import Node
from utils.sparse_list import SparseList

from . import utils
from .tflite_schema_head import\
    (ActivationFunctionType, ConcatenationOptions,
     Conv2DOptions, DepthwiseConv2DOptions,
     FullyConnectedOptions, Model, Padding, Pool2DOptions,
     ReshapeOptions, SoftmaxOptions, TensorType)

from .propagate_hints import propagate_hints

class TFLiteImportException(Exception):
    pass

LOG = logging.getLogger('nntool.' + __name__)

# The tensor shape. The meaning of each entry is operator-specific but
# builtin ops use: [batch size, number of channels, height, width] (That's
# Tensorflow's NHWC).

# Map to internal activation function names
TF_ACTIVATIONS = {
    ActivationFunctionType.ActivationFunctionType.RELU: "relu",
    ActivationFunctionType.ActivationFunctionType.RELU6: "relu6",
    ActivationFunctionType.ActivationFunctionType.SIGN_BIT: "sign_bit",
    ActivationFunctionType.ActivationFunctionType.TANH: "tanh"
}

TF_ACTIVATION_OPERATORS = {
    "LOGISTIC": "sigmoid",
    "RELU": "relu",
    "RELU6": "relu6",
    "TANH": "tanh",
}

TF_TO_NUMPY_TYPE = {
    TensorType.TensorType.FLOAT32: np.dtype('<f4'),
    TensorType.TensorType.FLOAT16: np.dtype('<f2'),
    TensorType.TensorType.INT32: np.dtype('<i4'),
    TensorType.TensorType.UINT8: np.dtype('<u1'),
    TensorType.TensorType.INT64: np.dtype('<i8')
}

def check(condition, message):
    if not condition:
        raise TFLiteImportException(message)

# Switch from same/valid padding to actual padding dimensions
def get_tf_padding(padding):
    if padding == Padding.Padding.SAME:
        return PadDim.same()
    if padding == Padding.Padding.VALID:
        return PadDim.valid()
    raise ValueError("Strange padding type")

TF_LITE_IN_OUT_ORDER = ['n', 'h', 'w', 'c']
TF_LITE_RED_IN_OUT_ORDER = ['h', 'w', 'c']
TF_LITE_FILT_IN_C = 0
TF_LITE_FILT_H = 1
TF_LITE_FILT_W = 2
TF_LITE_FILT_OUT_C = 3
TF_LITE_FILTER_ORDER = ['out_c', 'h', 'w', 'in_c']
# in_c should always be 1 in this case
# Why does TFLite use this weird order for depthwise?
TF_LITE_DW_FILTER_ORDER = ['in_c', 'h', 'w', 'out_c']
TF_LITE_DW_FILTER_TRANSPOSE = [TF_LITE_DW_FILTER_ORDER.index(dim) for dim in TF_LITE_FILTER_ORDER]

def add_node(G: NNGraph, node: Node, anode: Node = None) -> str:
    G.add_node(node)
    if not anode:
        return (node.name, node.name)
    G.add_node(anode)
    G.add_edge(NNEdge(node, anode))
    return (node.name, anode.name)

def aname(name):
    return name + "_activation"

def get_tensor(model, tensors, subgraph, elem, idx, dequantize=False):
    check(elem.InputsLength() >= idx + 1, "Not enough input tensors")
    tf_idx = elem.Inputs(idx)
    tensors[tf_idx]['used'] = True
    tf_tensor = subgraph.Tensors(tf_idx)
    dtype = TF_TO_NUMPY_TYPE[tf_tensor.Type()]
    tf_buffer_idx = tf_tensor.Buffer()
    tf_buffer = model.Buffers(tf_buffer_idx)
    np_buffer = np.frombuffer(tf_buffer.DataAsNumpy(), dtype=dtype)
    np_buffer = np.resize(np_buffer, tf_tensor.ShapeAsNumpy())
    if not dequantize or tf_tensor.Type() != TensorType.TensorType.UINT8:
        return np_buffer
    tf_quant = tf_tensor.Quantization()
    if tf_quant is None:
        return np_buffer
    np_buffer = tf_quant.ScaleAsNumpy() * (np_buffer - tf_quant.ZeroPointAsNumpy())
    return np_buffer

def get_shape(subgraph, tf_idx, order):
    if order is None:
        shape = []
        tf_tensor = subgraph.Tensors(tf_idx)
        for i in range(tf_tensor.ShapeLength()):
            shape.append(tf_tensor.Shape(i))
        return shape
    else:
        shape = {}
        tf_tensor = subgraph.Tensors(tf_idx)
        check(tf_tensor.ShapeLength() == len(order), "Shape incorrect")
        for i, j in enumerate(order):
            shape[j] = tf_tensor.Shape(i)
        return shape

def remove_batch_dim(dim):
    check(dim[0] == 1, "batch dimension should be 1")
    return dim[1:]

def get_input_size(tensors, subgraph, elem, idx, order=None):
    check(elem.InputsLength() >= idx + 1, "Not enough input tensors")
    tf_idx = elem.Inputs(idx)
    if tensors:
        tensors[tf_idx]['used'] = True
    return get_shape(subgraph, tf_idx, order)

def get_all_input_dims(subgraph, elem, order=None):
    inputs = []
    for idx in range(elem.InputsLength()):
        tf_idx = elem.Inputs(idx)
        shape = get_shape(subgraph, tf_idx, order)
        if len(shape) == 0:
            continue
        inputs.append(Dim.unnamed(remove_batch_dim(shape)))
    return inputs

def get_all_output_dims(subgraph, elem, order=None):
    outputs = []
    for idx in range(elem.OutputsLength()):
        tf_idx = elem.Outputs(idx)
        outputs.append(Dim.unnamed(remove_batch_dim(get_shape(subgraph, tf_idx, order))))
    return outputs

def get_fin_cput_size(subgraph, elem, idx):
    shape = {}
    check(elem.InputsLength() >= idx + 1, "Not enough input tensors")
    tf_idx = elem.Inputs(idx)
    tf_tensor = subgraph.Tensors(tf_idx)

    check(tf_tensor.ShapeLength() >= 2, "Shape incorrect")
    shape['n'] = tf_tensor.Shape(0)

    tf_idx = elem.Inputs(idx + 1)
    tf_tensor = subgraph.Tensors(tf_idx)
    shape['filter_c'] = tf_tensor.Shape(0)
    shape['filter_sz'] = tf_tensor.Shape(1)

    return shape

def fuse_activation(G: NNGraph, tfl_opts, name: str, node: Node):
    if tfl_opts.FusedActivationFunction() == ActivationFunctionType.ActivationFunctionType.NONE:
        return add_node(G, node)

    activation = TF_ACTIVATIONS[tfl_opts.FusedActivationFunction()]
    anode = ActivationParameters(aname(name), activation)
    return add_node(G, node, anode=anode)

def add_unconverted(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    del tensors, load_tensors, dequantize
    node = add_node(G,
                    UnconvertedOpParameters(
                        name,
                        op_name,
                        get_all_input_dims(subgraph, op),
                        get_all_output_dims(subgraph, op),
                        {
                            "tflite_op_name": op_name,
                            "tflite_op": op,
                            "tflite_subgraph": subgraph
                        }
                    ))
    return node

def add_convolution(G, tensors, name, subgraph, _, op, load_tensors=False, dequantize=False):
    conv_opts = Conv2DOptions.Conv2DOptions()
    conv_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

    # get filter dimensions
    filt = get_input_size(tensors, subgraph, op, 1, order=TF_LITE_FILTER_ORDER)
    filt = Conv2DFilterDim(filt['h'], filt['w'],\
        filt['out_c'], in_c=filt['in_c'])
    filt = filt.impose_order(TF_LITE_FILTER_ORDER)
    # compute padding
    pad = get_tf_padding(conv_opts.Padding())

    # does it have biases
    has_bias = op.InputsLength() > 2

    node = Conv2DParameters(name,
                            filt=filt,
                            stride=StrideDim(conv_opts.StrideH(),
                                             conv_opts.StrideW()),
                            padding=pad,
                            has_bias=has_bias,
                            in_dims_hint=SparseList([['h', 'w', 'c']]),
                            out_dims_hint=SparseList([['h', 'w', 'c']]),
                            constant_store=G.constant_store)

    if load_tensors:
        node.weights = get_tensor(G.model, tensors, subgraph, op, 1, dequantize=dequantize)
        if has_bias:
            node.biases = get_tensor(G.model, tensors, subgraph, op, 2, dequantize=dequantize)
    return fuse_activation(G, conv_opts, name, node)

def add_depthwise_convolution(G, tensors, name, subgraph, _, op,
                              load_tensors=False, dequantize=False):
    conv_opts = DepthwiseConv2DOptions.DepthwiseConv2DOptions()
    conv_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

    # get filter dimensions
    inp = get_input_size(tensors, subgraph, op, 0, order=TF_LITE_IN_OUT_ORDER)
    filt = get_input_size(tensors, subgraph, op, 1, order=TF_LITE_DW_FILTER_ORDER)
    filt = Conv2DFilterDim(filt['h'], filt['w'],\
        filt['out_c'], in_c=1)

    # multiplier should match filter
    check(filt.out_c == conv_opts.DepthMultiplier() * inp['c'],
          "invalid multiplier")

    groups = filt.out_c // conv_opts.DepthMultiplier()

    # compute padding
    pad = get_tf_padding(conv_opts.Padding())

    # does it have biases
    has_bias = op.InputsLength() > 2

    # TFLITE produces single channel input DW convolutions with the
    # multiplier equal to the number of out channels. This is just
    # a normal convolution and since we don't handle the channel
    # multiplier at present (but can) just convert them to normal
    # convolutions
    convert_to_conv = inp['c'] == 1 and groups == 1

    if convert_to_conv:
        filt.impose_order(TF_LITE_FILTER_ORDER)
        node = Conv2DParameters(name,
                                filt=filt,
                                stride=StrideDim(conv_opts.StrideH(), conv_opts.StrideW()),
                                padding=pad,
                                has_bias=has_bias,
                                in_dims_hint=SparseList([['h', 'w', 'c']]),
                                out_dims_hint=SparseList([['h', 'w', 'c']]),
                                constant_store=G.constant_store)
    else:
        filt.impose_order(TF_LITE_DW_FILTER_ORDER)
        node = Conv2DParameters(name,
                                filt=filt,
                                stride=StrideDim(conv_opts.StrideH(), conv_opts.StrideW()),
                                padding=pad,
                                groups=groups,
                                multiplier=conv_opts.DepthMultiplier(),
                                has_bias=has_bias,
                                tf_depthwise=True,
                                in_dims_hint=SparseList([['h', 'w', 'c']]),
                                out_dims_hint=SparseList([['h', 'w', 'c']]),
                                constant_store=G.constant_store)

    if load_tensors:
        node.weights = get_tensor(G.model, tensors, subgraph, op, 1, dequantize=dequantize)
        # If we've converted to a normal conv then change the weight order
        if convert_to_conv:
            node.weights = node.weights.transpose(TF_LITE_DW_FILTER_TRANSPOSE)
        if has_bias:
            node.biases = get_tensor(G.model, tensors, subgraph, op, 2, dequantize=dequantize)

    return fuse_activation(G, conv_opts, name, node)

TF_LITE_FC_ORDER = ['out_c', 'sz']
TF_LITE_FC_EXP_ORDER = ['out_c', 'h', 'w', 'in_c']

def add_fully_connected(G, tensors, name, subgraph, _, op, load_tensors=False, dequantize=False):
    fc_opts = FullyConnectedOptions.FullyConnectedOptions()
    fc_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

    # get filter dimensions
    inp = get_input_size(tensors, subgraph, op, 0)
    check(inp[0] == 1,
          "Multi batch not supported")
    filt = get_input_size(tensors, subgraph, op, 1, order=TF_LITE_FC_ORDER)
    check(filt['sz'] == reduce(lambda i, j: i * j, inp, 1), "filter doesn't match input size")
    # in the case we get an input of 1 batch with everything flattened fill h and w with 1
    if len(inp) == 2:
        inp = {'h': 1, 'w': 1, 'c': inp[1]}
    elif len(inp) == 4:
        inp = {'h': inp[1], 'w': inp[2], 'c': inp[3]}
    else:
        raise NotImplementedError('FC input size not implemented')

    filt_dim = FcFilterDim(inp['h'], inp['w'], filt['out_c'],
                           in_c=inp['c'], order=TF_LITE_FC_EXP_ORDER)

    # does it have biases
    has_bias = op.InputsLength() > 2

    node = FcParameters(name, filt=filt_dim, has_bias=has_bias,
                        in_dims_hint=SparseList([['h', 'w', 'c']]),
                        out_dims_hint=SparseList([['c']]),
                        constant_store=G.constant_store)

    if load_tensors:
        node.weights = get_tensor(G.model, tensors, subgraph, op, 1, dequantize=dequantize)
        if has_bias:
            node.biases = get_tensor(G.model, tensors, subgraph, op, 2, dequantize=dequantize)

    return fuse_activation(G, fc_opts, name, node)

# map to internal pool layer types
TF_POOL_OPS = {
    "AVERAGE_POOL_2D": "average",
    "MAX_POOL_2D": "max"
}

# pylint: disable=unused-argument
def add_pool(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    pool_opts = Pool2DOptions.Pool2DOptions()
    pool_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)
    pad = get_tf_padding(pool_opts.Padding())
    pool_type = TF_POOL_OPS[op_name]

    inp = get_input_size(None, subgraph, op, 0, order=TF_LITE_IN_OUT_ORDER)
    check(inp['n'] == 1,
          "Multi batch not supported")

    node = PoolingParameters(name,
                             filt=PoolFilterDim(pool_opts.FilterHeight(),
                                                pool_opts.FilterWidth()),
                             stride=StrideDim(pool_opts.StrideH(),
                                              pool_opts.StrideW()),
                             padding=pad,
                             pool_type=pool_type,
                             in_dims_hint=SparseList([['h', 'w', 'c']]),
                             out_dims_hint=SparseList([['h', 'w', 'c']]))

    return fuse_activation(G, pool_opts, name, node)

# pylint: disable=unused-argument
def add_softmax(G, tensors, name, subgraph, _, op, load_tensors=False, dequantize=False):
    softmax_opts = SoftmaxOptions.SoftmaxOptions()
    softmax_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)
    return add_node(G, SoftMaxParameters(name, softmax_opts.Beta()))

# pylint: disable=unused-argument
def add_concatenation(G, tensors, name, subgraph, _, op, load_tensors=False, dequantize=False):
    concat_opts = ConcatenationOptions.ConcatenationOptions()
    concat_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

    node = ConcatParameters(name, axis=max(concat_opts.Axis() - 1, 0))
    return fuse_activation(G, concat_opts, name, node)

# pylint: disable=unused-argument
def add_reshape(G, tensors, name, subgraph, _, op, load_tensors=False, dequantize=False):
    reshape_opts = ReshapeOptions.ReshapeOptions()
    reshape_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)
    inp = get_input_size(tensors, subgraph, op, 0)
    set_shape = get_tensor(G.model, tensors, subgraph, op, 1)
    # TODO - Which to use? Attribute or input? TFLITE seems to set both
    del set_shape
    new_shape = list(reshape_opts.NewShapeAsNumpy())
    if -1 in new_shape:
        new_shape_size = reduce(lambda x, y: x * 1 if y == -1 else x * y, new_shape, 1)
        inp_size = reduce(lambda x, y: x * y, inp, 1)
        new_shape[new_shape.index(-1)] = inp_size // new_shape_size

    old_shape = Dim.unnamed(remove_batch_dim(inp), is_ordered=True)
    new_shape = Dim.unnamed(remove_batch_dim(new_shape), is_ordered=True)
    node = ReshapeParameters(name, old_shape=old_shape, shape=new_shape)
    return add_node(G, node)

# pylint: disable=unused-argument
def add_activation(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    check(op.InputsLength() == 1,\
        "Very odd " + str(op.InputsAsNumpy()))
    activation = TF_ACTIVATION_OPERATORS[op_name]
    return add_node(G, ActivationParameters(name, activation))

def add_pad(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    check(op.InputsLength() == 2,\
        "Very odd " + str(op.InputsAsNumpy()))
    pad_dim = get_tensor(G.model, tensors, subgraph, op, 1, dequantize=False)
    assert np.all(pad_dim[3] == 0), "channel padding not supported"
    pad_dim = [int(pad_dim[i][j]) for i in range(1, 3) for j in range(2)]
    return add_node(G,
                    PadParameters(name,
                                  PadDim(*pad_dim)))

def add_add(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    check(op.InputsLength() == 2,\
        "Very odd " + str(op.InputsAsNumpy()))
    assert len(get_input_size(None, subgraph, op, 0)) ==\
        len(get_input_size(None, subgraph, op, 1)),\
        "Broadcasting nor supported"
    return add_node(G,
                    MatrixAddParameters(name))

def add_mean(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    check(op.InputsLength() == 2,\
        "Very odd " + str(op.InputsAsNumpy()))
    mean_dims = get_tensor(G.model, tensors, subgraph, op, 1, dequantize=False)
    if len(mean_dims) != 2 or mean_dims[0] != 1 or mean_dims[1] != 2:
        LOG.warning("MEAN operator seen but can't convert to global average pool")
        return add_unconverted(G, name, subgraph, op_name, op, load_tensors, dequantize)
    else:
        LOG.info("MEAN operator converted to global average pool")

    inp = get_input_size(None, subgraph, op, 0, order=TF_LITE_IN_OUT_ORDER)
    check(inp['n'] == 1,
          "Multi batch not supported")

    return add_node(G,
                    PoolingParameters(name,
                                      filt=PoolFilterDim(inp['h'],
                                                         inp['w']),
                                      stride=StrideDim(1, 1),
                                      padding=PadDim.valid(),
                                      pool_type="average",
                                      in_dims_hint=SparseList([['h', 'w', 'c']]),
                                      out_dims_hint=SparseList([['h', 'w', 'c']])))

# pylint: disable=unused-argument
def add_custom(G, tensors, name, subgraph, op_name, op, load_tensors=False, dequantize=False):
    return add_node(G,
                    UnknownOpParameters(
                        name,
                        {
                            "tflite_op_name": op_name,
                            "tflite_op": op,
                            "tflite_subgraph": subgraph
                        }
                    ))

SWITCH_ADD_FUNCTIONS = {
    "CONV_2D": add_convolution,
    "AVERAGE_POOL_2D": add_pool,
    "MAX_POOL_2D": add_pool,
    "DEPTHWISE_CONV_2D": add_depthwise_convolution,
    "FULLY_CONNECTED": add_fully_connected,
    "SOFTMAX": add_softmax,
    "CONCATENATION": add_concatenation,
    "RESHAPE": add_reshape,
    "PAD": add_pad,
    "ADD": add_add,
    "MEAN": add_mean
}

for __op in TF_ACTIVATION_OPERATORS:
    SWITCH_ADD_FUNCTIONS[__op] = add_activation

def add_operator(G, model, tensors, subgraph, subgraph_idx, op, op_idx,
                 load_tensors=False, dequantize=False):
    op_name, is_custom = utils.get_operator_name(model, op.OpcodeIndex())

    node_name = "{}_{}_{}".format(op_name, subgraph_idx, op_idx)

    if is_custom:
        return add_custom(G, tensors, node_name, subgraph, op_name, op)

    if op_name in SWITCH_ADD_FUNCTIONS:
        return SWITCH_ADD_FUNCTIONS[op_name](G, tensors, node_name, subgraph, op_name,\
            op, load_tensors=load_tensors, dequantize=dequantize)

    return add_unconverted(G, tensors, node_name, subgraph, op_name, op, load_tensors=load_tensors)

def create_subgraph(G, model, graph_index, load_tensors=False, dequantize=False):
    graph = model.Subgraphs(graph_index)
    tensors = []
    for i in range(graph.TensorsLength()):
        tensor = graph.Tensors(i)
        tensors.append({'name': tensor.Name().decode('ascii'), 'in':[], 'out':None, 'used': False})

    for i in range(graph.InputsLength()):
        dims = get_input_size(None, graph, graph, i, order=None)
        tensors[graph.Inputs(i)]['out'] =\
            G.add_input(Dim.unnamed(remove_batch_dim(dims)))

    for i in range(graph.OutputsLength()):
        tensors[graph.Outputs(i)]['in'].append((G.add_output(), 0))

    for i in range(graph.OperatorsLength()):
        op = graph.Operators(i)
        in_node_name, out_node_name =\
            add_operator(G, model, tensors, graph, graph_index, op, i,\
                load_tensors=load_tensors, dequantize=dequantize)
        # keep track of which input the tensor is attached to
        for j in range(op.InputsLength()):
            tensors[op.Inputs(j)]['in'].append((in_node_name, j))
        for j in range(op.OutputsLength()):
            tensors[op.Outputs(j)]['out'] = out_node_name

    for tensor in tensors:
        if tensor['out'] is not None:
            for in_rec in tensor['in']:
                G.add_edge(NNEdge(tensor['out'], in_rec[0], to_idx=in_rec[1]))
        else:
            # TODO - Load other constant tensors add constant inputs to node parameters
            # The way to do this is to add a constant inputs parameter to the node type
            # base and then read those in as part of the base class get output size
            if load_tensors and not tensor['used']:
                print("WARNING - Constant tensor %s detected - \
                    constant tensors are not implemented" % tensor['name'])

def create_graph(filename, opts):
    buf = open(filename, "rb").read()
    model = Model.Model.GetRootAsModel(buf, 0)
    LOG.info("Importing TFLITE model version %s", model.Version())
    check(model.Version() == 3, "Only support version 3 graphs at present")
    check(model.SubgraphsLength() == 1, "Only supports one subgraph at present")
    G = NNGraph(model=model, filename=filename, name=opts.get('name'),
                value_cache=opts.get('value_cache'), constant_store=ConstantStore())
    create_subgraph(G, model, 0, load_tensors=opts.get('load_tensors'),\
        dequantize=opts.get('dequantize'))
    propagate_hints(G)
    return G
