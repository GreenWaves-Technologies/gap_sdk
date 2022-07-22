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

import copy

from nntool.graph.dim import Dim
from nntool.graph.types import BroadcastableNodeBase, ConstantInputNode, NNEdge
from nntool.importer.common.broadcast_mixin import BroadcastMixin
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.tflite2.common import LOG, TFLiteImportException


class BasicMathMixin(ConstantMixin):
    TF_ACTIVATION_OPERATORS = {
        "LOGISTIC": "hsigmoid",
        "RELU": "relu",
        "RELU6": "relu6",
        "TANH": "tanh",
        "HARD_SWISH": "hswish"
    }

    @classmethod
    def _common(cls, node, **kwargs):
        params_class = kwargs.get('params_class')
        params_args = kwargs.get('params_args', {})
        flatten = kwargs.get('flatten')
        if params_args is None:
            params_args = {}
        all_nodes = kwargs['all_nodes']
        opts = kwargs['opts']
        G = kwargs['G']
        inputs = [all_nodes[inp] for inp in node.input]
        assert len(inputs) == 1
        x = inputs[0]
        pout = x[2].flatten if flatten else copy.deepcopy(x[2])
        constant_operation = kwargs.get('constant_operation')
        if cls.is_constant(x) and constant_operation:
            res = constant_operation(cls.get_constant(x))
            if res.size < 10:
                LOG.info("reducing %s to a constant %s", node.name, res)
            else:
                LOG.info("reducing %s to a constant", node.name)
            params = ConstantInputNode(node.name, value=res)
        elif params_class is None:
            raise TFLiteImportException(
                f'{node.name} - {node.op} is only supported for constant folding')
        else:
            params = params_class(node.name, **params_args)
            if opts.get('load_quantization'):
                in_qs = kwargs['in_qs'] if "in_qs" in kwargs else None
                out_qs = kwargs['out_qs'] if "out_qs" in kwargs else None
                G.quantization[params.name] = cls.load_tf_quantization([node.input[0]], node.output,
                                                                        in_qs=in_qs, out_qs=out_qs)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pout)
        return params


class ArithmeticMixin(ConstantMixin, BroadcastMixin):
    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        opts = kwargs['opts']
        params_class = kwargs.get('params_class')
        node_opts = kwargs.get("node_opts", None)
        params_args = kwargs.get('params_args', {})
        constant_operation = kwargs.get('constant_operation')
        inputs = [all_nodes[inp] for inp in node.input]
        assert len(inputs) == 2
        if all(cls.is_constant(inp) for inp in inputs) and constant_operation:
            LOG.info("reducing %s to a constant", node.name)
            values = [cls.get_constant(inp) for inp in inputs]
            output_shapes = cls.implied_broadcast(inputs)
            params = ConstantInputNode(node.name, value=constant_operation(*values),
                                             dims=Dim.unnamed(output_shapes[0].known_shape))
        else:
            if params_class is None:
                raise TFLiteImportException(
                    f'{node.name} - {node.op} is only supported for constant folding')
            if not isinstance(params_class, type) and callable(params_class):
                params, in_links = params_class(G, node.name, inputs)
            else:
                params = params_class(node.name, **params_args)
                in_links = [{
                    'from_node': inp[0],
                    'from_idx': inp[1],
                    'to_node': params,
                    'to_idx': idx}
                    for idx, inp in enumerate(inputs)]

            output_shapes = cls.implied_broadcast(inputs)
            shapes = [inp[2].known_shape for inp in inputs]

            for link in in_links:
                G.add_edge(NNEdge(**link))

            if not callable(params_class) and isinstance(params, BroadcastableNodeBase):
                for idx, shape in enumerate(shapes.copy()):
                    len_diff = len(shape) - len(output_shapes[0].known_shape)
                    if len_diff > 0:
                        if not all(dim is None or dim == 1 for dim in shape[:len_diff:]):
                            in_shapes = ",".join(str(shape)
                                                 for shape in shapes)
                            raise ValueError(
                                f'strange broadcast {in_shapes} -> {output_shapes[0].shape}')
                        shapes[idx] = shape[len_diff::]
                params.set_broadcast(shapes)
        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization(
                node.input, node.output)
        if node_opts is not None:
            params = cls.fuse_activation(
                node_opts, node.name, params, **kwargs)
        all_nodes[node.output[0]] = (params, 0, output_shapes[0])
        return params


# class ReductionMixin(BroadcastMixin):
#     @classmethod
#     def _common(cls, node, **kwargs):
#         pass
#         # attrs = copy.deepcopy(node.attrs)
#         # axis = attrs.pop("axes", None)
#         # if isinstance(axis, (list, tuple)) and len(axis) == 1:
#         #     axis = axis[0]
#         # attrs["axis"] = axis
#         # # https://github.com/onnx/onnx/issues/585
#         # attrs["keepdims"] = attrs.pop("keepdims", 1) == 1
#         # return [cls.make_tensor_from_onnx_node(node, attrs=attrs, **kwargs)]
