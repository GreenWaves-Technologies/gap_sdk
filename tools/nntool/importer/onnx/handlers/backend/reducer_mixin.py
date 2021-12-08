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

from graph.dim import Dim
from graph.types import (ConstantInputParameters, GlobalPoolingParameters,
                         NNEdge, NoOPParameters)
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger


class ReducerMixin(ConstantMixin):
    @classmethod
    def _common(cls, node, copy_qtype=False, constant_operation=None, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        reduce_type = kwargs['reduce_type']
        inputs = [all_nodes[inp] for inp in node.input]

        x = inputs[0]
        x_shape = x[2].shape
        x_rank = len(x_shape)

        axes = node.attrs['axes']

        # convert all negative axis to their true value
        axes = set([elem if elem >= 0 else x_rank + elem
                    for elem in axes])
        assert all(axis >= 0 and axis <
                   x_rank for axis in axes), "axis out of bounds"
        keep_dims = node.attrs.get('keepdims', 1)

        stripped_axes = [axis for axis in axes if x_shape[axis] is not None]

        if not stripped_axes:
            params = NoOPParameters(valid_name)
            pout_shape = x_shape.copy()
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        else:
            if keep_dims:
                pout_shape = [dim if idx not in axes else 1 for idx,
                              dim in enumerate(x_shape)]
            else:
                pout_shape = [dim for idx, dim in enumerate(
                    x_shape) if idx not in axes]
                # if all(dim is None for dim in pout_shape):
                #     pout_shape.append(1)

            # subtract 1 from axis for all None's preceeding it and remove
            # axes that are not defined
            axes = [ax - sum([1 if dim is None else 0 for dim in x_shape[:ax:]])
                    for ax in stripped_axes]
            if cls.is_constant(x) and constant_operation:
                val = constant_operation(cls.get_constant(x), axis=tuple(axes), keepdims=keep_dims)
                if val.size < 10:
                    logger.info("reducing %s to a constant %s", valid_name, val)
                else:
                    logger.info("reducing %s to a constant", valid_name)
                params = ConstantInputParameters(valid_name, value=val, dims=Dim.unnamed(val.shape))
            else:
                params = GlobalPoolingParameters(valid_name, pool_type=reduce_type, axis=tuple(axes),
                                                 keep_dims=keep_dims)

                G.add_edge(
                    NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(
            pout_shape), x[3] if copy_qtype else None)
        return params
