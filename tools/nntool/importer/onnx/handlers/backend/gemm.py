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

import numpy as np
from graph.dim import Dim, FcFilterDim
from graph.types import (ConstantInputParameters, FcParameters, NNEdge,
                         TransposeParameters)
from graph.types.tensor_arithmetic import MatMulOpParameters
from importer.common.provisional_dim import ProvisionalDim

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description
from .promote_linear_mixin import PromoteLinearMixin


@onnx_op("Gemm")
@partial_support(True)
@ps_description("alpha and beta must be 1.0 for operators mapped onto matrix operations and alpha must be 1.0 for operators mapped onto a linear layer")
class Gemm(PromoteLinearMixin, BackendHandler):

    @classmethod
    def _import_as_matmul(cls, node, inputs, x, y, real_x_shape, real_y_shape, trans_a, trans_b, alpha, beta, **kwargs):
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        all_nodes = kwargs['all_nodes']
        if trans_a:
            tparams = TransposeParameters(G.unique_name(
                f'{valid_name}_tinx'), transpose=(1, 0))
            G.add_edge(NNEdge(from_node=x[0], to_node=tparams, from_idx=x[1]))
            x = (tparams, 0)
        if trans_b:
            tparams = TransposeParameters(G.unique_name(
                f'{valid_name}_tiny'), transpose=(1, 0))
            G.add_edge(NNEdge(from_node=y[0], to_node=tparams, from_idx=y[1]))
            y = (tparams, 0)
        params = MatMulOpParameters(G.unique_name(valid_name))
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        G.add_edge(
            NNEdge(from_node=y[0], to_node=params, from_idx=y[1], to_idx=1))

        out_dims = params.get_output_size(
            [Dim.unnamed(real_x_shape), Dim.unnamed(real_y_shape)])

        biases = cls.get_constant(inputs[2]) if len(inputs) > 2 else np.zeros(out_dims[0].shape[1])
        biases_params = ConstantInputParameters(
            G.unique_name(f'{valid_name}_biases'), dims=Dim.unnamed(biases.shape), value=biases)
        G.add_edge(
            NNEdge(from_node=biases_params, to_node=params, to_idx=2))
        cls.record_constant_qrec(inputs[2], biases_params, **kwargs)
        all_nodes[node.output[0]] = (params, 0, out_dims[0], None)
        return params

    @classmethod
    def _common(cls, node, **kwargs):

        all_nodes = kwargs['all_nodes']
        trans_a = node.attrs.get('transA', 0)
        trans_b = node.attrs.get('transB', 0)
        alpha = node.attrs.get('alpha', 1.0)
        beta = node.attrs.get('beta', 1.0)

        inputs = [all_nodes[inp] for inp in node.input]

        x = inputs[0]
        x_shape = x[2].shape
        y = inputs[1]
        y_shape = y[2].shape

        real_x_shape = cls._get_real_dim(x_shape)
        real_y_shape = cls._get_real_dim(y_shape)

        real_x_shape = [real_x_shape[1], real_x_shape[0]] if len(
            real_x_shape) == 2 and trans_a else real_x_shape
        real_y_shape = [real_y_shape[1], real_y_shape[0]] if len(
            real_y_shape) == 2 and trans_b else real_y_shape

        if not cls.is_linear(y, real_x_shape, real_y_shape) or trans_a:
            if alpha != 1.0 or beta != 1.0:
                raise NotImplementedError('Alpha and Beta not implemented on pure matmul GEMM')
            return cls._import_as_matmul(node, inputs, x, y, real_x_shape, real_y_shape,
                                         trans_a, trans_b, alpha, beta, **kwargs)

        G = kwargs['G']
        valid_name = kwargs['valid_name']
        if len(inputs) > 2:
            biases = cls.get_constant(inputs[2])
        else:
            biases = np.zeros([real_y_shape[1]], dtype=np.float32)

        filt_dim = FcFilterDim(real_y_shape[1], real_x_shape[0])

        # always create new constants since they may be modified by this not and could be linked elsewhere
        weights = cls.get_constant(y) * alpha
        if not trans_b:
            weights = np.transpose(weights, [1, 0])
        weights_params = ConstantInputParameters(
            f'{valid_name}_weights', dims=Dim.unnamed(weights.shape), value=weights)
        if y[3]:
            if alpha == 1.0:
                cls.record_constant_qrec(y, weights_params, **kwargs)
            else:
                raise NotImplementedError("qtype on Gemm with alpha != 1.0")

        biases = biases * beta
        biases_params = ConstantInputParameters(
            f'{valid_name}_biases', dims=Dim.unnamed(biases.shape), value=biases)
        if len(inputs) > 2 and inputs[2][3]:
            if beta == 1.0:
                cls.record_constant_qrec(inputs[2], biases_params, **kwargs)
            else:
                raise NotImplementedError("qtype on Gemm with beta != 1.0")

        params = FcParameters(valid_name, filt=filt_dim, has_bias=True,
                              #   in_dims_hint=[['c']],
                              in_dims_hint=[
                                  None, ['out_c', 'in_c'], ['out_c']],
                              out_dims_hint=[['c']])

        G.add_edge(NNEdge(from_node=weights_params, to_node=params, to_idx=1))
        G.add_edge(NNEdge(from_node=biases_params, to_node=params, to_idx=2))

        out_dims = params.get_output_size([Dim.unnamed(real_x_shape)])
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        if isinstance(x[2], ProvisionalDim):
            out_dim = x[2].infer_mapping(out_dims[0].shape)
        else:
            out_dim = out_dims[0]
        all_nodes[node.output[0]] = (params, 0, out_dim, None)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
