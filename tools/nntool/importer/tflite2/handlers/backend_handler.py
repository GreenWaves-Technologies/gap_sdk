
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

from copy import deepcopy

import numpy as np
from numpy.lib.arraysetops import isin
from numpy.lib.function_base import append
from graph.dim import Dim
from graph.types.activations import ActivationParameters
from graph.types.base import NNEdge
from graph.types.input_output import ConstantInputParameters
from importer.common.provisional_dim import ProvisionalDim
from quantization.multiplicative.mult_quantization import (
    MultConstantQuantizationRecord, MultQuantizationRecord,
    MultQuantizationRecordBase)
from quantization.qtype import QType
from utils.node_id import NodeId

from ..tflite_schema_head.ActivationFunctionType import ActivationFunctionType
from .handler import Handler


class BackendHandler(Handler):
    """ This class is base backend handler class.
    All backend operator handler class MUST inherit this class.
    In backend, operator handler class's name should be pascal case of file name
    which should be snake case.
    Use ONNX operator name as class name.
    """

    VAR_COUNT = 0
    TF_ACTIVATIONS = {
        ActivationFunctionType.RELU: "relu",
        ActivationFunctionType.RELU6: "relu6",
        ActivationFunctionType.SIGN_BIT: "sign_bit",
        ActivationFunctionType.TANH: "tanh"
    }

    @classmethod
    def _get_real_dim(cls, shape):
        return np.array([elem for elem in shape if elem is not None])

    @classmethod
    def _get_real_dims(cls, dims):
        return [cls._get_real_dim(dim.shape) for dim in dims]

    @classmethod
    def _verify_constant(cls, inp):
        if isinstance(inp[0], ConstantInputParameters):
            return inp[0].value
        raise ValueError("expected node %s to be constant input" % inp[0].name)

    @classmethod
    def _slice_len(cls, vstart, vend, vstep):
        if vstep < 0:
            vstart, vend = vend, vstart
            vstep = -vstep
        return (vend - vstart - 1) // vstep + 1

    @classmethod
    def fuse_activation(cls, tfl_opts, name, params, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        if opts.get('load_quantization') and NodeId(params) in G.quantization:
            node_qrec = G.quantization[NodeId(params)]
        else:
            node_qrec = None
        # if node_qrec is not None and None in node_qrec.in_qs + node_qrec.out_qs:
        #     # one of the input is a constant or strange behaviour -> may be is something fusions will get rid of
        #     return add_node(self.G, node)
        aparams = None
        if tfl_opts.FusedActivationFunction() == ActivationFunctionType.NONE:
            if node_qrec is not None and isinstance(node_qrec, MultQuantizationRecordBase):
                # here we have no activation in an asymmetric qtype -> may be an omitted relu
                if node_qrec.out_qs[0].min_val == 0:
                    if np.all(np.round(node_qrec.out_qs[0].max_val) == 6):
                        aparams = ActivationParameters.get_activation('relu6', name + "_activation")
                    else:
                        aparams = ActivationParameters.get_activation('relu', name + "_activation")
        else:
            aparams = ActivationParameters.get_activation(cls.TF_ACTIVATIONS[tfl_opts.FusedActivationFunction()],
                                                          name + "_activation")
        if aparams:
            G.add_edge(NNEdge(from_node=params, to_node=aparams))

            if opts.get('load_quantization'):
                # In between the fused operation and activation the
                # transfer is in int32 representation
                node_qrec = G.quantization[NodeId(params)]
                ina_qtype = deepcopy(node_qrec.out_qs[0])
                outa_qtype = deepcopy(ina_qtype)
                G.quantization[NodeId(aparams)] = MultQuantizationRecord(
                    in_qs=[ina_qtype], out_qs=[outa_qtype])
            params = aparams
        return params

    @classmethod
    def remove_unspecified_dim(cls, shape):
        return [dim for dim in shape if dim is not None]

    @classmethod
    def get_all_const_inputs(cls, G, all_nodes, opts, node, params,
                             exclude=None, names=None,
                             short_names=None,
                             adjust_transposes=None,
                             load_quantization_if_present=False,
                             skip_empty_tensors=True):
        if exclude is None:
            exclude = []
        if names is None:
            names = [None] * len(node.inputs)
        if short_names is None:
            short_names = [None] * len(node.inputs)
        if adjust_transposes is None:
            adjust_transposes = [None] * len(node.nputs)
        const_params = []

        # TODO - this should just be picking up the existing constant nodes not creating new ones.
        for idx, tensor in enumerate(node.input):
            if tensor is None or idx in exclude or (skip_empty_tensors and not tensor.is_constant):
                const_params.append(None)
                continue

            tensor.used = True

            if tensor not in all_nodes:
                # this can occur for RNN/LSTM state nodes that have a buffer idx of 0
                const_param = ConstantInputParameters(
                    tensor.name,
                    dims=Dim.unnamed(tensor.shape),
                    value=tensor.value,
                    constant_store=G.constant_store)
                all_nodes[tensor] = (
                    const_param,
                    0,
                    ProvisionalDim.from_tflite_shape(tensor.shape)
                )
            else:
                const_param = all_nodes[tensor][0]

            # some constant nodes can be connected to multiple nodes
            # changing their name is not a good idea
            if const_param not in G.nodes():
                const_param.name = names[idx]
                const_param.adjust_transpose = adjust_transposes[idx]
                const_param.is_mutated = node.is_mutated(idx)
                const_param.is_intermediate = node.is_intermediate(idx)
                const_param.short_name = short_names[idx]

                const_param.value = np.reshape(tensor.value, tensor.shape)

                # if opts.get('load_quantization'):
                #     G.quantization[NodeId(const_param)] = MultConstantQuantizationRecord(
                #         in_qs=[tensor.qtype],
                #         out_qs=[tensor.qtype])

            # if load_quantization_if_present and tensor.qtype:
            #     const_param.value_quantization = tensor.qtype

            const_params.append(const_param)
            G.add_edge(NNEdge(const_param, params, to_idx=idx))

        return const_params

    @classmethod
    def remove_none_from_constants(cls, inputs, model):
        if None not in model:
            return
        for inp in inputs:
            if not isinstance(inp[0], ConstantInputParameters):
                continue
            val = inp[0].value
            if val is None or len(val.shape) != len(model):
                continue
            assert all(val.shape[idx] == 1 for idx, dim in enumerate(model) if dim is None),\
                "value has axis that is larger than one in an unknown dimension"
            new_shape = [dim for idx, dim in enumerate(val.shape) if model[idx] is not None]
            inp[0].value = np.reshape(inp[0].value, new_shape)
            inp[0].dims = Dim.unnamed(new_shape)

    @classmethod
    def convert_to_symmetric(cls, qtypes):
        return [QType.from_min_max_sq(qtype.min_val, qtype.max_val)
                if qtype is not None and qtype.is_asymmetric else qtype for qtype in qtypes]

    @classmethod
    def load_tf_quantization(cls, input_tensors, output_tensors, in_qs=None, out_qs=None, qrec_class=None):
        if qrec_class is None:
            qrec_class = MultQuantizationRecord

        qrec = qrec_class(
            in_qs=cls.convert_to_symmetric(
                in_qs if in_qs is not None else [tensor.qtype for tensor in input_tensors]),
            out_qs=cls.convert_to_symmetric(
                out_qs if out_qs is not None else [tensor.qtype for tensor in output_tensors]))
        return qrec
