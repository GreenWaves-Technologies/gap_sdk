
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

import hashlib
from copy import deepcopy

import numpy as np
from nntool.graph.dim import Dim
from nntool.graph.types import (ActivationNodeBase, ConstantInputNode, NNEdge,
                         ReshapeNode)
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType

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
        if cls._is_constant(inp):
            return cls._get_constant(inp)
        raise NotImplementedError(f"NNTOOL requires node {inp[0].name} to be constant input.")

    @classmethod
    def _is_constant(cls, inp):
        return isinstance(inp[0], ConstantInputNode)

    @classmethod
    def _get_constant(cls, inp):
        return inp[0].value

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
        ext = hashlib.sha1(name.encode(
            "UTF-8")).hexdigest()[:8] if opts.get('anonymise') else 'activation'
        if opts.get('load_quantization') and params.name in G.quantization:
            node_qrec = G.quantization[params.name]
        else:
            node_qrec = None
        # if node_qrec is not None and None in node_qrec.in_qs + node_qrec.out_qs:
        #     # one of the input is a constant or strange behaviour -> may be is something fusions will get rid of
        #     return add_node(self.G, node)
        aparams = None
        if tfl_opts.FusedActivationFunction() == ActivationFunctionType.NONE:
            if node_qrec is not None and node_qrec.ktype.startswith('scaled'): # and opts.get('insert_relus'):
                # here we have no activation in an asymmetric qtype -> may be an omitted relu
                if node_qrec.out_qs[0] is not None and node_qrec.out_qs[0].min_val == 0:
                    if np.all(np.round(node_qrec.out_qs[0].max_val) == 6):
                        aparams = ActivationNodeBase.get_activation(
                            'relu6', name + f"_{ext}")
                    else:
                        aparams = ActivationNodeBase.get_activation(
                            'relu', name + f"_{ext}")
        else:
            aparams = ActivationNodeBase.get_activation(cls.TF_ACTIVATIONS[tfl_opts.FusedActivationFunction()],
                                                          name + f"_{ext}")
        if aparams:
            G.add_edge(NNEdge(from_node=params, to_node=aparams))

            if opts.get('load_quantization'):
                # In between the fused operation and activation the
                # transfer is in int32 representation
                node_qrec = G.quantization[params.name]
                ina_qtype = deepcopy(node_qrec.out_qs[0])
                outa_qtype = deepcopy(ina_qtype)
                G.quantization[aparams.name] = QRec.scaled(
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
                             load_quantization_if_present=False,
                             skip_empty_tensors=True):
        if exclude is None:
            exclude = []
        if names is None:
            names = [None] * len(node.inputs)
        if short_names is None:
            short_names = [None] * len(node.inputs)
        const_params = []

        # TODO - this should just be picking up the existing constant nodes not creating new ones.
        for idx, tensor in enumerate(node.input):
            if tensor is None or idx in exclude or (skip_empty_tensors and not tensor.is_constant):
                const_params.append(None)
                continue

            tensor.used = True

            if tensor not in all_nodes:
                # this can occur for RNN/LSTM state nodes that have a buffer idx of 0
                const_param = ConstantInputNode(
                    tensor.name,
                    dims=Dim.unnamed(tensor.shape),
                    value=tensor.value)
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
                const_param.is_mutated = node.is_mutated(idx)
                const_param.is_intermediate = node.is_intermediate(idx)
                const_param.short_name = short_names[idx]

                const_param.value = np.reshape(tensor.value, tensor.shape)

                if opts.get('load_quantization'):
                    G.quantization[const_param.name] = QRec.scaled(
                        in_qs=[tensor.qtype],
                        out_qs=[tensor.qtype])

            if load_quantization_if_present and tensor.qtype:
                const_param.value_quantization = tensor.qtype

            const_params.append(const_param)
            G.add_edge(NNEdge(const_param, params, to_idx=idx))

        return const_params

    @classmethod
    def remove_none_from_constants(cls, inputs, model):
        if None not in model:
            return
        for inp in inputs:
            if not isinstance(inp[0], ConstantInputNode):
                continue
            val = inp[0].value
            if val is None or len(val.shape) != len(model):
                continue
            assert all(val.shape[idx] == 1 for idx, dim in enumerate(model) if dim is None),\
                "value has axis that is larger than one in an unknown dimension"
            new_shape = [dim for idx, dim in enumerate(
                val.shape) if model[idx] is not None]
            inp[0].value = np.reshape(inp[0].value, new_shape)
            inp[0].dims = Dim.unnamed(new_shape)

    @classmethod
    def convert_to_symmetric(cls, qtypes):
        return [QType.from_min_max_sq(qtype.min_val, qtype.max_val)
                if qtype is not None and (qtype.asymmetric or not qtype.signed) else qtype for qtype in qtypes]

    @classmethod
    def load_tf_quantization(cls, input_tensors, output_tensors, in_qs=None, out_qs=None, qrec_class=None):
        if qrec_class is None:
            qrec = QRec.scaled(
                in_qs=cls.convert_to_symmetric(
                    in_qs if in_qs is not None else [tensor.qtype if tensor is not None else None for tensor in input_tensors]),
                out_qs=cls.convert_to_symmetric(
                    out_qs if out_qs is not None else [tensor.qtype for tensor in output_tensors]))
        else:
            qrec = qrec_class(
                in_qs=cls.convert_to_symmetric(
                    in_qs if in_qs is not None else [tensor.qtype if tensor is not None else None for tensor in input_tensors]),
                out_qs=cls.convert_to_symmetric(
                    out_qs if out_qs is not None else [tensor.qtype for tensor in output_tensors]))
        return qrec

    @classmethod
    def remove_known_batch_dimension(cls, G, x, node, batch_axis=0):
        x_shape = x[2].shape
        if x_shape[batch_axis] is not None:
            if x_shape[0] > 1:
                raise ValueError(
                    f'multi batch (n={x_shape[batch_axis]}) operations are not supported by {node.name}')
            rparams = ReshapeNode(
                f'{node.name}_batch',
                old_shape=Dim.unnamed(x_shape),
                shape=Dim.unnamed(x_shape[0:batch_axis:]+x_shape[batch_axis+1::]))
            if G.quantization:
                qrec = G.quantization[x[0].name]
                G.quantization[rparams.name] = QRec.copy_ktype(
                    qrec,
                    in_qs=[qrec.out_qs[0]],
                    out_qs=[qrec.out_qs[0]])
            G.add_edge(
                NNEdge(from_node=x[0], to_node=rparams, from_idx=x[1], to_idx=0))
            return (rparams, 0, ProvisionalDim(x_shape[0:batch_axis:]+[None]+x_shape[batch_axis+1::]))
        else:
            return x
