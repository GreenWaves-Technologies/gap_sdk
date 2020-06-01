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
import os
from copy import deepcopy
from functools import reduce

import numpy as np

from graph.constant_store import ConstantStore
from graph.dim import (Conv2DFilterDim, Dim, FcFilterDim, PadDim,
                       PoolFilterDim, StrideDim)
from graph.nngraph import NNGraph
from graph.types import (ActivationParameters, ConcatParameters,
                         Conv2DParameters, FcParameters, GlobalPoolParameters,
                         MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters, NNEdge,
                         NoOPParameters, PadParameters, PoolingParameters,
                         ReshapeParameters, SoftMaxParameters,
                         UnconvertedOpParameters, UnknownOpParameters)
from quantization.multiplicative.asymmetric.asymmetric_mult_qtype import \
    AsymmetricMultQType
from quantization.multiplicative.mult_quantization import (
    MultAddQuantizationRecord, MultConstantQuantizationRecord,
    MultQuantizationRecord, MultQuantizationRecordBase,
    MultScalableFilterQuantizationRecord)
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import \
    MultMulBiasScaleQType
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype_wrapper import \
    SymmetricMultQTypeWrapper
from quantization.quantization_set import QuantizationSet
from utils.add_sys_path import add_sys_path
from utils.graph import Node
from utils.node_id import NodeId
from utils.sparse_list import SparseList

from ..importer_base import ImporterBase
from . import utils
from .propagate_hints import propagate_hints
from .tflite_schema_head import (ActivationFunctionType, AddOptions,
                                 ConcatenationOptions, Conv2DOptions,
                                 DepthwiseConv2DOptions, DivOptions,
                                 FullyConnectedOptions, Model, MulOptions,
                                 Padding, Pool2DOptions, ReshapeOptions,
                                 SoftmaxOptions, SubOptions, TensorType)


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
    "LOGISTIC": "hsigmoid",
    "RELU": "relu",
    "RELU6": "relu6",
    "TANH": "tanh",
    "HARD_SWISH": "hswish"
}

UNDIGNED_TO_SIGNED = {
    np.uint8: np.int8,
    np.uint16: np.int16,
    np.uint32: np.int32
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
        return (node, node)
    G.add_node(anode)
    G.add_edge(NNEdge(node, anode))
    return (node, anode)


def aname(name):
    return name + "_activation"


def get_tensor_at_subgraph_idx(model, tensors, tf_idx):
    tensor = tensors[tf_idx]
    tensor.used = True
    return tensor.get_value(model)


def get_tensor(model, tensors, elem, idx):
    check(elem.InputsLength() >= idx + 1, "Not enough input tensors")
    tf_idx = elem.Inputs(idx)
    return get_tensor_at_subgraph_idx(model, tensors, tf_idx)


def get_shape(subgraph, tf_idx, order):
    if order is None:
        shape = []
        tf_tensor = subgraph.Tensors(tf_idx)
        for i in range(tf_tensor.ShapeLength()):
            shape.append(tf_tensor.Shape(i))
        return shape

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
        tensors[tf_idx].used = True
    return get_shape(subgraph, tf_idx, order)


def get_input_tensors(tensors, elem):
    return [tensors[tf_idx] for tf_idx in elem.InputsAsNumpy()]


def get_output_tensors(tensors, elem):
    return [tensors[tf_idx] for tf_idx in elem.OutputsAsNumpy()]


def get_broadcasted_shape(model, other):
    idx_model = 0
    idx_other = 0
    res_other = []
    while idx_model < len(model):
        if len(other) > idx_other and (model[idx_model] == other[idx_other] or other[idx_other] == 1):
            res_other.append(other[idx_other])
            idx_other += 1
        else:
            res_other.append(1)
        idx_model += 1
    assert len(model) == len(res_other), "{} and {} cannot be broadcasted".format(
        ",".join(model), ",".join(other))
    return res_other


def get_all_const_broadcasted_inputs(G, model, tensors, subgraph, elem, load_tensors=False):
    """Special version for nodes that can have constant inputs and support broadcasting"""
    shapes = []
    constant_nodes = []
    max_len = 0
    max_len_idx = -1
    for idx in range(elem.InputsLength()):
        tf_idx = elem.Inputs(idx)
        shape = get_shape(subgraph, tf_idx, None)
        # if shape is empty then it is a scalar
        if len(shape) == 0:
            shape = [1]
        tensor = tensors[tf_idx]
        if tensor.is_constant(model):
            constant_nodes.append(tensor)
        else:
            constant_nodes.append(None)
        shapes.append(shape)
        if len(shape) > max_len:
            max_len = len(shape)
            max_len_idx = idx
    assert constant_nodes[max_len_idx] is None, "can't handle broadcasting to a constant node"
    largest = shapes[max_len_idx]
    if len(largest) == 4 and largest[0] == 1:
        largest = largest[1:]

    for idx in range(elem.InputsLength()):
        if idx == max_len_idx:
            shapes[idx] = largest
            continue
        shape = shapes[idx]
        # its not constant so should be a standard activation
        if constant_nodes[idx] is None:
            if len(shape) == 4 and shape[0] == 1:
                shapes[idx] = shape[1:]
            continue

        # strip all ones off the start of the shape
        # this has the side effect of getting rid of the batch dimension if there
        while len(shape) > 1 and shape[0] == 1:
            shape = shape[1:]
        shape = get_broadcasted_shape(largest, shape)
        tensor = constant_nodes[idx]
        constant_node = G.add_constant(Dim.unnamed(shape))
        constant_nodes[idx] = constant_node
        if load_tensors:
            constant_node.value = np.reshape(get_tensor_at_subgraph_idx(
                model, tensors, elem.Inputs(idx)), shape)
        shapes[idx] = shape

    return list(zip(shapes, constant_nodes))


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


class TfliteTensorWrapper():
    TF_TO_NUMPY_TYPE = {
        TensorType.TensorType.FLOAT32: np.float32,
        TensorType.TensorType.FLOAT16: np.float16,
        TensorType.TensorType.INT32: np.int32,
        TensorType.TensorType.UINT8: np.uint8,
        TensorType.TensorType.INT8: np.int8,
        TensorType.TensorType.INT64: np.int64
    }

    def __init__(self, tensor):
        self._tensor = tensor
        self._name = tensor.Name().decode('ascii')
        self._used = False
        self._inputs = []
        self._output = None

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, val):
        self._name = val

    @property
    def used(self):
        return self._used

    @used.setter
    def used(self, val):
        self._used = val

    @property
    def inputs(self):
        return self._inputs

    @property
    def output(self):
        return self._output

    @output.setter
    def output(self, val):
        self._output = val

    @property
    def buffer_idx(self):
        return self._tensor.Buffer()

    @property
    def tf_tensor(self):
        return self._tensor

    @property
    def is_variable(self):
        return self._tensor.IsVariable()

    @property
    def shape(self):
        return self._tensor.ShapeAsNumpy()

    @property
    def dtype(self):
        return self.TF_TO_NUMPY_TYPE[self._tensor.Type()]

    @property
    def scale(self):
        return self._tensor.Quantization().ScaleAsNumpy()

    @property
    def zero_point(self):
        return self._tensor.Quantization().ZeroPointAsNumpy()

    @property
    def min_val(self):
        return self._tensor.Quantization().MinAsNumpy()

    @property
    def max_val(self):
        return self._tensor.Quantization().MaxAsNumpy()

    @property
    def is_uint_symmetric(self):
        quant = self._tensor.Quantization()
        if quant is not None:
            return (self.dtype == np.uint8 or self.dtype == np.uint16 or self.dtype == np.uint32) and \
                np.all(quant.ZeroPointAsNumpy() == 128)
        return False

    @property
    def qtype(self):
        quant = self._tensor.Quantization()
        if quant is not None:
            if quant.ScaleLength() == 0 and quant.MinLength() == 0 and\
                    quant.MaxLength() == 0 and quant.ZeroPointLength() == 0:
                return None
            if self.dtype == np.uint8 or self.dtype == np.uint16 or self.dtype == np.uint32:
                if np.all(quant.ZeroPointAsNumpy() == 128):
                    return SymmetricMultQType.from_tflite(quant, self.dtype)
                return SymmetricMultQTypeWrapper(AsymmetricMultQType.from_tflite(quant,
                                                                                 self.dtype))
            elif self.dtype == np.int8 or self.dtype == np.int16 or self.dtype == np.int32:
                if np.all(quant.ZeroPointAsNumpy() == 0):
                    return SymmetricMultQType.from_tflite(quant, self.dtype)
                return SymmetricMultQTypeWrapper(AsymmetricMultQType.from_tflite(quant,
                                                                                 self.dtype))
            return None
        return None

    def is_constant(self, model):
        return self.buffer_idx != 0 and model.Buffers(self.buffer_idx).DataLength() != 0

    def get_value(self, model):
        tf_buffer = model.Buffers(self.buffer_idx)
        np_buffer = np.frombuffer(tf_buffer.DataAsNumpy(), dtype=self.dtype().newbyteorder('L'))
        np_buffer = np.resize(np_buffer, self.shape)
        return np_buffer

    def shape_as(self, order):
        assert len(order) == len(self.shape), "tensor does not have correct number of dimensions"
        return {k: v for k, v in zip(order, self.shape)}


class NoQuantizationError(Exception):
    pass


class TfliteImporter(ImporterBase):

    def __init__(self):
        self.G = None
        self.model = None
        self.tensors = None
        self.load_quantization = False
        self.load_tensors = False
        self.load_dequantized = False
        self.qrecs = QuantizationSet()
        self.rescale_perchannel = True

    def fuse_activation(self, tfl_opts, name: str, node: Node):
        if NodeId(node) in self.qrecs:
            node_qrec = self.qrecs[NodeId(node)]
        else:
            node_qrec = None
        if tfl_opts.FusedActivationFunction() == ActivationFunctionType.ActivationFunctionType.NONE:
            if node_qrec is not None and isinstance(node_qrec, MultQuantizationRecordBase):
                # here we have no activation in an asymmetric qtype -> may be an omitted relu
                if node_qrec.out_qs[0].min_val == 0:
                    if np.all(np.round(node_qrec.out_qs[0].max_val) == 6):
                        anode = ActivationParameters.get_activation('relu6', aname(name))
                    else:
                        anode = ActivationParameters.get_activation('relu', aname(name))
                else:
                    return add_node(self.G, node)
            else:
                return add_node(self.G, node)
        else:
            anode = ActivationParameters.get_activation(TF_ACTIVATIONS[tfl_opts.FusedActivationFunction()],
                                                        aname(name))

        if self.load_quantization:
            # In between the fused operation and activation the
            # transfer is in int32 representation
            node_qrec = self.qrecs[NodeId(node)]
            outa_qtype = deepcopy(node_qrec.out_qs[0])
            #node_qrec.out_qs[0].dtype = np.int32
            ina_qtype = deepcopy(node_qrec.out_qs[0])
            self.qrecs[NodeId(anode)] = MultQuantizationRecord(
                in_qs=[ina_qtype], out_qs=[outa_qtype])
        return add_node(self.G, node, anode=anode)

    def add_unconverted(self, name, subgraph, op_name, op):
        LOG.warning("graph has unknown operator %s and cannot be properly processed", op_name)
        node = add_node(self.G,
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

    def make_weights_symmetric(self, node, input_tensors):
        biases_scales = input_tensors[2].scale if node.has_bias else np.array([1], dtype=np.int32)
        # already symmetric or something we don't know
        if input_tensors[1].dtype != np.uint8:
            return input_tensors[1].scale, biases_scales, None, None
        weights_scales = input_tensors[1].scale
        # symmetric unsigned. just change zero point scale stays the same
        if np.all(input_tensors[1].zero_point == 128):
            node.weights = (node.weights.astype(np.int64) - 128).astype(np.int8)
            return weights_scales, biases_scales, None, None
        # asymmetric unsigned. change zero point and rescale
        if self.rescale_perchannel:
            return self.scale_weights_by_channel(node, weights_scales, biases_scales,
                                                 input_tensors[0].qtype.scale,
                                                 zero_point=input_tensors[1].zero_point)
        else:
            return self.scale_weights_by_tensor(node, weights_scales, biases_scales,
                                                input_tensors[0].qtype.scale,
                                                zero_point=input_tensors[1].zero_point)

    def scale_weights_by_tensor(self, node, weights_scales, biases_scales, in_scale, zero_point=None):
        if zero_point is None:
            zero_point = np.array([0])
        if node.has_bias:
            dq_biases = node.biases * biases_scales
        else:
            dq_biases = np.array([0] * node.filter.out_c, dtype=np.float32)

        if len(weights_scales) > 1:
            raise ValueError('You should not rescale perchannel weights to pertensor format')

        dq_weights = (node.weights.astype(np.float32) - zero_point) * weights_scales
        w_min = min(np.min(dq_weights), 0)
        w_max = max(np.max(dq_weights), 0)
        w_max = w_max if w_min != w_max and w_max == 0 else 1

        w_abs_max = max(w_max, np.abs(w_min))
        new_weights_scale = w_abs_max / 127
        int8_iinfo = np.iinfo(np.int8)
        int32_iinfo = np.iinfo(np.int32)
        new_biases_scale = new_weights_scale * in_scale
        node.weights = np.clip(np.floor(dq_weights / new_weights_scale + 0.5),
                               int8_iinfo.min,
                               int8_iinfo.max).astype(np.int8)
        node.biases = np.clip(np.floor(dq_biases / new_biases_scale + 0.5),
                              int32_iinfo.min,
                              int32_iinfo.max).astype(np.int32)
        return np.array([new_weights_scale]), np.array([new_biases_scale]),\
            np.array([w_min]), np.array([w_max])

    def scale_weights_by_channel(self, node, weights_scales, biases_scales, in_scale, zero_point=None):
        # scale weights by channel optionally correcting zero point
        if zero_point is None:
            zero_point = np.array([0])

        out_idx = node.filter.get_order_idx('out_c')
        actual_len = len(node.filter.actual_shape)
        ones_shape = tuple(node.filter.out_c if idx == out_idx else 1 for idx in range(actual_len))
        filter_axis = tuple(idx for idx in range(actual_len) if idx != out_idx)

        if node.has_bias:
            dq_biases = node.biases * biases_scales
        else:
            dq_biases = np.array([0] * node.filter.out_c, dtype=np.float32)

        if len(weights_scales) > 1:
            weights_scales = weights_scales.reshape(ones_shape)
        if len(zero_point) > 1:
            zero_point = zero_point.reshape(ones_shape)
        dq_weights = (node.weights.astype(np.float32) - zero_point) * weights_scales

        w_mins = np.minimum(np.min(dq_weights, axis=filter_axis), 0)
        w_maxes = np.maximum(np.max(dq_weights, axis=filter_axis), 0)

        w_zero_cond = np.logical_and(w_mins == w_maxes, w_maxes == 0)
        w_maxes = np.where(w_zero_cond, 1, w_maxes)

        w_abs_maxes = np.maximum(np.abs(w_mins), w_maxes)
        new_weights_scales = w_abs_maxes / 127
        int8_iinfo = np.iinfo(np.int8)
        int32_iinfo = np.iinfo(np.int32)
        new_biases_scales = new_weights_scales * in_scale
        np.seterr(all='raise')
        node.weights = np.clip(np.floor(dq_weights / new_weights_scales.reshape(ones_shape) + 0.5),
                               int8_iinfo.min,
                               int8_iinfo.max).astype(np.int8)
        node.biases = np.clip(np.floor(dq_biases / new_biases_scales + 0.5),
                              int32_iinfo.min,
                              int32_iinfo.max).astype(np.int32)
        return new_weights_scales, new_biases_scales, w_mins, w_maxes

    def detect_small_scales(self, node, weights_scales, biases_scales, in_scale):
        # at this point all tensors are in expected formats
        # weights int8 biases int32 channel scaled
        tiny_weight_scales = weights_scales < SymmetricMultQType.kNearZeroTolerance
        if np.count_nonzero(tiny_weight_scales) == 0:
            return weights_scales, biases_scales

        out_idx = node.filter.get_order_idx('out_c')
        shape = tuple(slice(None) if idx !=
                      out_idx else tiny_weight_scales for idx in range(len(node.weights.shape)))

        node.weights[shape] = 0
        dq_biases = node.biases * biases_scales
        weights_scales = np.where(tiny_weight_scales, 1, weights_scales)
        biases_scales = in_scale * weights_scales
        int32_iinfo = np.iinfo(np.int32)
        node.biases = np.clip(np.floor(dq_biases / biases_scales + 0.5),
                              int32_iinfo.min,
                              int32_iinfo.max).astype(np.int32)
        return weights_scales, biases_scales

    def fix_weights_and_biases(self, node, input_tensors):
        weights_scales, biases_scales, w_mins, w_maxes = self.make_weights_symmetric(
            node, input_tensors)
        if self.rescale_perchannel:
            if len(weights_scales) != node.filter.out_c:
                weights_scales, biases_scales, w_mins, w_maxes = self.scale_weights_by_channel(
                    node, weights_scales, biases_scales, input_tensors[0].qtype.scale)
            weights_scales, biases_scales = self.detect_small_scales(
                node, weights_scales, biases_scales, input_tensors[0].scale)
        if w_mins is None:
            w_mins = input_tensors[1].min_val
            w_maxes = input_tensors[1].max_val
        return weights_scales, biases_scales, w_mins, w_maxes

    def load_filter_parameters(self, node, input_tensors, output_tensors, converted_to_conv=False):
        if self.load_tensors or self.load_quantization:
            node.weights = input_tensors[1].get_value(self.model)
            if converted_to_conv:
                node.weights = node.weights.transpose(TF_LITE_DW_FILTER_TRANSPOSE)
            if node.has_bias:
                node.biases = input_tensors[2].get_value(self.model)

        if self.load_quantization:
            if input_tensors[0].qtype is None:
                raise NoQuantizationError("quantization not present in tflite file")
            weights_scales, biases_scales, w_mins, w_maxes = self.fix_weights_and_biases(
                node, input_tensors)
            biases_q = SymmetricMultBiasesQType(dtype=np.int32, scale=biases_scales)
            weights_q = SymmetricMultQType(
                dtype=np.int8, narrow_range=True, scale=weights_scales, min_val=w_mins, max_val=w_maxes)
            in_q = input_tensors[0].qtype
            out_q = output_tensors[0].qtype
            mulbiases_q = MultMulBiasScaleQType.from_filter(in_q, weights_q, out_q, node)
            qrec = MultScalableFilterQuantizationRecord(in_qs=[in_q],
                                                        out_qs=[out_q],
                                                        mul_biases_q=mulbiases_q,
                                                        weights_q=weights_q,
                                                        biases_q=biases_q)
            self.qrecs[NodeId(node)] = qrec

    def load_dequantized_filter_parameters(self, node, input_tensors, converted_to_conv=False, is_dw=False):
        weights_scales = input_tensors[1].scale
        in_scale = input_tensors[0].scale
        weights_quant = input_tensors[1].get_value(self.model)
        # save in the node the dequantized values
        if len(weights_scales) > 1:  # tf2 conv and dw (fully connected should be per-tensor)
            if is_dw:
                # depthwise
                shape_pc = tuple(size if idx == 3 else 1  # always along axis 3 from tflite quantization spec
                                 for idx, size in enumerate(weights_quant.shape))
            else:
                # normal convolution
                shape_pc = tuple(size if idx == 0 else 1  # always along axis 0 from tflite quantization spec
                                 for idx, size in enumerate(weights_quant.shape))
            node.weights = (weights_quant.astype(np.int64) - input_tensors[1].zero_point.reshape(shape_pc)) \
                * weights_scales.reshape(shape_pc)
        else:
            node.weights = (weights_quant - input_tensors[1].zero_point) * weights_scales
        if converted_to_conv:
            node.weights = node.weights.transpose(TF_LITE_DW_FILTER_TRANSPOSE)
        if node.has_bias:
            biases_scales = weights_scales * in_scale
            node.biases = input_tensors[2].get_value(self.model) * biases_scales

    def add_convolution(self, name, subgraph, _, op):
        del subgraph
        conv_opts = Conv2DOptions.Conv2DOptions()
        conv_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

        input_tensors = get_input_tensors(self.tensors, op)
        output_tensors = get_output_tensors(self.tensors, op)

        # get filter dimensions
        filt = input_tensors[1].shape_as(TF_LITE_FILTER_ORDER)
        input_tensors[1].used = True
        filt = Conv2DFilterDim(filt['h'], filt['w'],
                               filt['out_c'], in_c=filt['in_c'])
        filt = filt.impose_order(TF_LITE_FILTER_ORDER)
        # compute padding
        pad = get_tf_padding(conv_opts.Padding())

        # does it have biases
        has_bias = op.InputsLength() > 2
        if has_bias:
            input_tensors[2].used = True

        node = Conv2DParameters(name,
                                filt=filt,
                                stride=StrideDim(conv_opts.StrideH(),
                                                 conv_opts.StrideW()),
                                padding=pad,
                                has_bias=has_bias,
                                in_dims_hint=SparseList([['h', 'w', 'c']]),
                                out_dims_hint=SparseList([['h', 'w', 'c']]),
                                constant_store=self.G.constant_store)

        if self.load_dequantized:
            self.load_dequantized_filter_parameters(node, input_tensors)
        else:
            self.load_filter_parameters(node, input_tensors, output_tensors)

        return self.fuse_activation(conv_opts, name, node)

    def add_depthwise_convolution(self, name, subgraph, _, op):
        del subgraph
        conv_opts = DepthwiseConv2DOptions.DepthwiseConv2DOptions()
        conv_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

        input_tensors = get_input_tensors(self.tensors, op)
        output_tensors = get_output_tensors(self.tensors, op)

        # get filter dimensions
        inp = input_tensors[0].shape_as(TF_LITE_IN_OUT_ORDER)
        filt = input_tensors[1].shape_as(TF_LITE_DW_FILTER_ORDER)
        input_tensors[1].used = True
        filt = Conv2DFilterDim(filt['h'], filt['w'],
                               filt['out_c'], in_c=1)

        # multiplier should match filter
        check(filt.out_c == conv_opts.DepthMultiplier() * inp['c'],
              "invalid multiplier")

        groups = filt.out_c // conv_opts.DepthMultiplier()

        # compute padding
        pad = get_tf_padding(conv_opts.Padding())

        # does it have biases
        has_bias = op.InputsLength() > 2
        if has_bias:
            input_tensors[2].used = True

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
                                    constant_store=self.G.constant_store)
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
                                    constant_store=self.G.constant_store)

        if self.load_dequantized:
            self.load_dequantized_filter_parameters(
                node, input_tensors, convert_to_conv, is_dw=True)
        else:
            self.load_filter_parameters(node, input_tensors, output_tensors,
                                        converted_to_conv=convert_to_conv)

        return self.fuse_activation(conv_opts, name, node)

    TF_LITE_FC_ORDER = ['out_c', 'sz']
    TF_LITE_FC_EXP_ORDER = ['out_c', 'h', 'w', 'in_c']

    def add_fully_connected(self, name, subgraph, _, op):
        del subgraph
        fc_opts = FullyConnectedOptions.FullyConnectedOptions()
        fc_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

        input_tensors = get_input_tensors(self.tensors, op)
        output_tensors = get_output_tensors(self.tensors, op)
        # get filter dimensions
        inp = input_tensors[0].shape
        check(inp[0] == 1,
              "Multi batch not supported")

        filt = input_tensors[1].shape_as(self.TF_LITE_FC_ORDER)
        input_tensors[1].used = True
        check(filt['sz'] == reduce(lambda i, j: i * j, inp, 1), "filter doesn't match input size")
        # in the case we get an input of 1 batch with everything flattened fill h and w with 1
        if len(inp) == 2:
            inp = {'h': 1, 'w': 1, 'c': inp[1]}
        elif len(inp) == 3:
            inp = {'h': 1, 'w': inp[1], 'c': inp[2]}
        elif len(inp) == 4:
            inp = {'h': inp[1], 'w': inp[2], 'c': inp[3]}
        else:
            raise NotImplementedError('FC input size not implemented')

        filt_dim = FcFilterDim(inp['h'], inp['w'], filt['out_c'],
                               in_c=inp['c'], order=self.TF_LITE_FC_EXP_ORDER)

        # does it have biases
        has_bias = op.InputsLength() > 2
        if has_bias:
            input_tensors[2].used = True

        node = FcParameters(name, filt=filt_dim, has_bias=has_bias,
                            in_dims_hint=SparseList([['h', 'w', 'c']]),
                            out_dims_hint=SparseList([['c']]),
                            constant_store=self.G.constant_store)

        if self.load_dequantized:
            self.load_dequantized_filter_parameters(node, input_tensors)
        else:
            self.load_filter_parameters(node, input_tensors, output_tensors)

        return self.fuse_activation(fc_opts, name, node)

    # map to internal pool layer types
    TF_POOL_OPS = {
        "AVERAGE_POOL_2D": "average",
        "MAX_POOL_2D": "max"
    }

    def load_tf_quantization(self, input_tensors, output_tensors, qrec_class=None):
        if qrec_class is None:
            qrec_class = MultQuantizationRecord
        qrec = qrec_class(in_qs=[tensor.qtype for tensor in input_tensors],
                          out_qs=[tensor.qtype for tensor in output_tensors])
        return qrec

    # pylint: disable=unused-argument

    def add_pool(self, name, subgraph, op_name, op):
        pool_opts = Pool2DOptions.Pool2DOptions()
        pool_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)
        pad = get_tf_padding(pool_opts.Padding())
        pool_type = self.TF_POOL_OPS[op_name]

        input_tensors = get_input_tensors(self.tensors, op)

        inp = input_tensors[0].shape_as(TF_LITE_IN_OUT_ORDER)

        check(inp['n'] == 1,
              "Multi batch not supported")

        filter_matches_input = inp['h'] == pool_opts.FilterHeight(
        ) and inp['w'] == pool_opts.FilterWidth()
        stride_is_one = pool_opts.StrideH() == 1 and pool_opts.StrideW() == 1

        if filter_matches_input and stride_is_one:
            node = GlobalPoolParameters(name,
                                        pool_type=pool_type,
                                        in_dims_hint=SparseList([['h', 'w', 'c']]),
                                        out_dims_hint=SparseList([['h', 'w', 'c']]))
        else:
            node = PoolingParameters(name,
                                     filt=PoolFilterDim(pool_opts.FilterHeight(),
                                                        pool_opts.FilterWidth()),
                                     stride=StrideDim(pool_opts.StrideH(),
                                                      pool_opts.StrideW()),
                                     padding=pad,
                                     pool_type=pool_type,
                                     in_dims_hint=SparseList([['h', 'w', 'c']]),
                                     out_dims_hint=SparseList([['h', 'w', 'c']]))

        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(
                input_tensors, get_output_tensors(self.tensors, op))

        return self.fuse_activation(pool_opts, name, node)

    # pylint: disable=unused-argument

    def add_softmax(self, name, subgraph, _, op):
        softmax_opts = SoftmaxOptions.SoftmaxOptions()
        softmax_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)
        node = SoftMaxParameters(name, softmax_opts.Beta())
        if self.load_quantization:
            input_tensors = get_input_tensors(self.tensors, op)
            iqtype = input_tensors[0].qtype
            iqtype.scale_to_pow2()
            oqtype = SymmetricMultQType(min_val=-1, max_val=1, dtype=np.int16, scale=2**(-15))
            qrec = MultQuantizationRecord(in_qs=[iqtype],
                                          out_qs=[oqtype])
            self.qrecs[NodeId(node)] = qrec

        return add_node(self.G, node)

    def add_noop(self, name, subgraph, op_name, op):
        node = NoOPParameters(name, desc=op_name)
        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(get_input_tensors(self.tensors, op),
                                                                 get_output_tensors(self.tensors, op))
        return add_node(self.G, node)

    # pylint: disable=unused-argument

    def add_concatenation(self, name, subgraph, _, op):
        concat_opts = ConcatenationOptions.ConcatenationOptions()
        concat_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

        input_tensors = get_input_tensors(self.tensors, op)
        output_tensors = get_output_tensors(self.tensors, op)

        buffer_idxes = [tensor.buffer_idx for tensor in input_tensors]
        if len(set(buffer_idxes)) != len(buffer_idxes):
            raise NotImplementedError("concats with multiple versions of the same input are not supported. This is normally a graph design problem.")

        axis_hint = None
        axis = None
        # nasty hack to try to figure out how the axis relates to our
        # internal axis representation
        if concat_opts.Axis() == 0:
            if len(output_tensors[0].shape) == 2:
                axis_hint = 'c'
                axis = 0
            elif len(output_tensors[0].shape) == 4:
                axis_hint = 'h'
                axis = 0
        elif concat_opts.Axis() == 1:
            if len(output_tensors[0].shape) == 2:
                axis_hint = 'c'
                axis = 0
            elif len(output_tensors[0].shape) == 3:
                axis = 0
            elif len(output_tensors[0].shape) == 4:
                axis_hint = 'h'
                axis = 0
        elif concat_opts.Axis() == 2:
            if all(tensor.shape[1] == 1 for tensor in input_tensors):
                axis_hint = 'w'
                axis = 1
        elif concat_opts.Axis() == 3:
            if len(output_tensors[0].shape) == 4:
                axis_hint = 'c'
                axis = 2
        if axis is None:
            axis = concat_opts.Axis() - 1
        node = ConcatParameters(name, axis=axis, axis_hint=axis_hint)
        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(input_tensors,
                                                                 output_tensors)
        return self.fuse_activation(concat_opts, name, node)

    # pylint: disable=unused-argument

    def add_reshape(self, name, subgraph, _, op):
        reshape_opts = ReshapeOptions.ReshapeOptions()
        reshape_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)

        input_tensors = get_input_tensors(self.tensors, op)

        inp = input_tensors[0].shape
        set_shape_tensor = input_tensors[1]
        set_shape_tensor.used = True
        # TODO - Which to use? Attribute or input? TFLITE seems to set both
        new_shape = list(reshape_opts.NewShapeAsNumpy())
        if -1 in new_shape:
            new_shape_size = reduce(lambda x, y: x * 1 if y == -1 else x * y, new_shape, 1)
            inp_size = reduce(lambda x, y: x * y, inp, 1)
            new_shape[new_shape.index(-1)] = inp_size // new_shape_size

        old_shape = Dim.unnamed(remove_batch_dim(inp), is_ordered=True)
        new_shape = Dim.unnamed(remove_batch_dim(new_shape), is_ordered=True)
        node = ReshapeParameters(name, old_shape=old_shape, shape=new_shape)
        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(
                [input_tensors[0]], get_output_tensors(self.tensors, op))
        return add_node(self.G, node)

    # pylint: disable=unused-argument

    def add_activation(self, name, subgraph, op_name, op):
        check(op.InputsLength() == 1,
              "Very odd " + str(op.InputsAsNumpy()))
        node = ActivationParameters.get_activation(TF_ACTIVATION_OPERATORS[op_name], name)
        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(get_input_tensors(self.tensors, op),
                                                                 get_output_tensors(self.tensors, op))
        return add_node(self.G, node)

    def add_pad(self, name, subgraph, op_name, op):
        check(op.InputsLength() == 2,
              "Very odd " + str(op.InputsAsNumpy()))
        pad_dim = get_tensor(self.model, self.tensors, op, 1)
        assert np.all(pad_dim[3] == 0), "channel padding not supported"
        pad_dim = [int(pad_dim[i][j]) for i in range(1, 3) for j in range(2)]
        node = PadParameters(name,
                             PadDim(*pad_dim))
        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(get_input_tensors(self.tensors, op),
                                                                 get_output_tensors(self.tensors, op))
        return add_node(self.G, node)

    def add_broadcasted_op(self, name, subgraph, op_name, op, tf_opts, params, qrec_class=None):
        tf_opts.Init(op.BuiltinOptions().Bytes, op.BuiltinOptions().Pos)
        inputs = get_all_const_broadcasted_inputs(
            self.G, self.model, self.tensors, subgraph, op, load_tensors=self.load_tensors)
        check(len(inputs) == 2,
              "broadcasted ops should only have 2 inputs " + str(op.InputsAsNumpy()))
        if self.load_quantization:
            self.qrecs[NodeId(params)] = self.load_tf_quantization(get_input_tensors(self.tensors, op),
                                                                   get_output_tensors(
                                                                       self.tensors, op),
                                                                   qrec_class=qrec_class)
        node_pair = self.fuse_activation(tf_opts, name, params)
        for idx, input_node in enumerate(inputs):
            if input_node[1] is not None:
                if self.load_quantization:
                    node_qrec = self.qrecs[NodeId(params)]
                    self.qrecs[NodeId(input_node[1])] = MultConstantQuantizationRecord(
                        in_qs=[node_qrec.in_qs[idx]],
                        out_qs=[node_qrec.in_qs[idx]])
                self.G.add_edge(NNEdge(input_node[1], node_pair[0], to_idx=idx))
        return node_pair

    def add_add(self, name, subgraph, op_name, op):
        return self.add_broadcasted_op(name, subgraph, op_name, op,
                                       AddOptions.AddOptions(),
                                       MatrixAddParameters(name),
                                       MultAddQuantizationRecord)

    def add_div(self, name, subgraph, op_name, op):
        return self.add_broadcasted_op(name, subgraph, op_name, op,
                                       DivOptions.DivOptions(),
                                       MatrixDivParameters(name))

    def add_mul(self, name, subgraph, op_name, op):
        return self.add_broadcasted_op(name, subgraph, op_name, op,
                                       MulOptions.MulOptions(),
                                       MatrixMulParameters(name))

    def add_sub(self, name, subgraph, op_name, op):
        return self.add_broadcasted_op(name, subgraph, op_name, op,
                                       SubOptions.SubOptions(),
                                       MatrixSubParameters(name),
                                       MultAddQuantizationRecord)

    def add_mean(self, name, subgraph, op_name, op):
        check(op.InputsLength() == 2,
              "Very odd " + str(op.InputsAsNumpy()))
        mean_dims = get_tensor(self.model, self.tensors, op, 1)

        if len(mean_dims) != 2 or mean_dims[0] != 1 or mean_dims[1] != 2:
            LOG.warning("MEAN operator seen but can't convert to global average pool")
            return self.add_unconverted(name, subgraph, op_name, op)

        LOG.info("MEAN operator converted to global average pool")

        inp = get_input_size(None, subgraph, op, 0, order=TF_LITE_IN_OUT_ORDER)
        check(inp['n'] == 1,
              "Multi batch not supported")

        node = GlobalPoolParameters(name,
                                    in_dims_hint=SparseList([['h', 'w', 'c']]),
                                    out_dims_hint=SparseList([['h', 'w', 'c']]))
        if self.load_quantization:
            self.qrecs[NodeId(node)] = self.load_tf_quantization(get_input_tensors(self.tensors, op),
                                                                 get_output_tensors(self.tensors, op))

        return add_node(self.G, node)

    # pylint: disable=unused-argument

    def add_custom(self, name, subgraph, op_name, op):
        return add_node(self.G,
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
        "MUL": add_mul,
        "SUB": add_sub,
        "DIV": add_div,
        "MEAN": add_mean,
        "QUANTIZE": add_noop,
        "DEQUANTIZE": add_noop
    }

    for operator in TF_ACTIVATION_OPERATORS:
        SWITCH_ADD_FUNCTIONS[operator] = add_activation

    def add_operator(self, subgraph, subgraph_idx, op, op_idx):
        op_name, is_custom = utils.get_operator_name(self.model, op.OpcodeIndex())

        node_name = "{}_{}_{}".format(op_name, subgraph_idx, op_idx)

        if is_custom:
            return self.add_custom(node_name, subgraph, op_name, op)

        if op_name in self.SWITCH_ADD_FUNCTIONS:
            return self.SWITCH_ADD_FUNCTIONS[op_name](self, node_name, subgraph, op_name, op)

        return self.add_unconverted(node_name, subgraph, op_name, op)

    def create_subgraph(self, graph_index):
        graph = self.model.Subgraphs(graph_index)
        self.tensors = []
        for i in range(graph.TensorsLength()):
            tensor = TfliteTensorWrapper(graph.Tensors(i))
            self.tensors.append(tensor)

        for i in range(graph.InputsLength()):
            dims = get_input_size(None, graph, graph, i, order=None)
            node = self.G.add_input(Dim.unnamed(remove_batch_dim(dims)))
            tensor = self.tensors[graph.Inputs(i)]
            tensor.output = node.name
            if self.load_quantization and tensor.qtype:
                self.qrecs[NodeId(node)] = MultQuantizationRecord(in_qs=[], out_qs=[tensor.qtype])

        for i in range(graph.OutputsLength()):
            node = self.G.add_output()
            tensor = self.tensors[graph.Outputs(i)]
            tensor.inputs.append((node.name, 0))
            if self.load_quantization and tensor.qtype:
                self.qrecs[NodeId(node)] = MultQuantizationRecord(
                    in_qs=[tensor.qtype], out_qs=[tensor.qtype])

        for i in range(graph.OperatorsLength()):
            op = graph.Operators(i)
            in_node, out_node =\
                self.add_operator(graph, graph_index, op, i)
            # keep track of which input the tensor is attached to
            for j in range(op.InputsLength()):
                self.tensors[op.Inputs(j)].inputs.append((in_node.name, j))
            for j in range(op.OutputsLength()):
                self.tensors[op.Outputs(j)].output = out_node.name

        for tensor in self.tensors:
            if tensor.output is not None:
                for in_rec in tensor.inputs:
                    self.G.add_edge(NNEdge(tensor.output, in_rec[0], to_idx=in_rec[1]))
            elif self.load_tensors and not tensor.used:
                LOG.warning("unused tensors in graph")

    def create_graph(self, filename, opts):
        add_sys_path(os.path.dirname(__file__))
        buf = open(filename, "rb").read()
        self.model = Model.Model.GetRootAsModel(buf, 0)
        self.load_quantization = opts.get('load_quantization')
        self.load_tensors = opts.get('load_tensors')
        self.load_dequantized = opts.get('load_dequantized')
        LOG.info("Importing TFLITE model version %s", self.model.Version())
        check(self.model.Version() == 3, "Only support version 3 graphs at present")
        check(self.model.SubgraphsLength() == 1, "Only supports one subgraph at present")
        self.G = NNGraph(model=self.model, filename=filename, name=opts.get('name'),
                         constant_store=ConstantStore())
        self.create_subgraph(0)
        if self.load_quantization:
            self.G.quantization = self.qrecs
            self.G.has_quantized_parameters = True
            self.G.graph_identity.quantization_type = 'SQ8'

        propagate_hints(self.G)
        return self.G
