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
from graph.types import FcParameters, MatMulOpParameters, NNEdge
from graph.types.input_output import ConstantInputParameters

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description
from .promote_linear_mixin import PromoteLinearMixin


@onnx_op("MatMul")
@partial_support(True)
@ps_description("Supports only 2D or groups of 2D matrix multiplications.")
class MatMul(PromoteLinearMixin, BackendHandler):
    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = cls._get_real_dim(x[2].shape)
        y = inputs[1]
        y_shape = cls._get_real_dim(y[2].shape)
        if cls.is_linear(y, x_shape, y_shape):
            filt_dim = FcFilterDim(y_shape[1], x_shape[0])
            weights = np.transpose(cls.get_constant(y), [1, 0])
            weights_params = ConstantInputParameters(f'{valid_name}_weights',
                                                     dims=Dim.unnamed(
                                                         [y_shape[1], x_shape[0]]),
                                                     value=weights)
            params = FcParameters(valid_name, filt=filt_dim, has_bias=True,
                                #   in_dims_hint=[
                                #       ['c'], ['out_c', 'in_c'], ['out_c']],
                                  in_dims_hint=[
                                      None, ['out_c', 'in_c'], ['out_c']],
                                  out_dims_hint=[['c']],
                                  constant_store=G.constant_store)
            out_dims = params.get_output_size([Dim.unnamed(x_shape)])
            biases_params = ConstantInputParameters(f'{valid_name}_biases', dims=Dim.unnamed([y_shape[1]]),
                                                    value=np.zeros((y_shape[1]), dtype=np.float32))
            G.add_edge(NNEdge(from_node=weights_params,
                              to_node=params, to_idx=1))
            G.add_edge(NNEdge(from_node=biases_params,
                              to_node=params, to_idx=2))
        else:
            params = MatMulOpParameters(valid_name)
            out_dims = params.get_output_size(
                [Dim.unnamed(x_shape), Dim.unnamed(y_shape)])
            G.add_edge(
                NNEdge(from_node=y[0], to_node=params, from_idx=y[1], to_idx=1))
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        pout_dims = x[2].infer_mapping(out_dims[0].shape)
        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
