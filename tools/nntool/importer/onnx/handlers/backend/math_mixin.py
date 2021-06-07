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

import numpy as np
from graph.dim import Dim
from graph.types.base import NNEdge
from graph.types.input_output import ConstantInputParameters
from graph.types.tensor_arithmetic import Broadcastable
from importer.common.broadcast_mixin import BroadcastMixin
from importer.common.constant_mixin import ConstantMixin
from importer.onnx.common import logger


class BasicMathMixin(ConstantMixin, BroadcastMixin):
    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        valid_name = kwargs['valid_name']
        G = kwargs['G']
        constant_operation = kwargs.get('constant_operation')
        inputs = [all_nodes[inp] for inp in node.input]
        # may have more than one input i.e. clip
        x = inputs[0]
        if cls.is_constant(x) and constant_operation:
            res = constant_operation(cls.get_constant(x))
            if res.size < 10:
                logger.info("reducing %s to a constant %s", valid_name, res)
            else:
                logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(valid_name, value=res,
                                             constant_store=G.constant_store)
        else:
            params_args = kwargs.get('params_args', {})
            params = kwargs['params_class'](valid_name, **params_args)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, copy.deepcopy(x[2]))
        return params


class ArithmeticMixin(ConstantMixin, BroadcastMixin):
    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        valid_name = kwargs['valid_name']
        G = kwargs['G']
        constant_operation = kwargs.get('constant_operation')
        constant_int_operation = kwargs.get('constant_int_operation')
        inputs = [all_nodes[inp] for inp in node.input]
        assert len(inputs) == 2
        if all(cls.is_constant(inp) for inp in inputs) and constant_operation:
            values = [cls.get_constant(inp) for inp in inputs]
            outputs = cls.implied_broadcast(inputs)
            if constant_int_operation and all(np.issubdtype(val.dtype, np.integer) for val in values):
                res = constant_int_operation(*values)
            else:
                res = constant_operation(*values)
            if res.size < 10:
                logger.info("reducing %s to a constant %s", valid_name, res)
            else:
                logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(valid_name, value=res,
                                             dims=Dim.unnamed(outputs[0].known_shape),
                                             constant_store=G.constant_store)
        else:
            params_args = kwargs.get('params_args', {})
            params = kwargs['params_class'](valid_name, **params_args)
            outputs = cls.implied_broadcast(inputs)
            shapes = []
            for idx, inp in enumerate(inputs):
                G.add_edge(NNEdge(from_node=inp[0], to_node=params, from_idx=inp[1], to_idx=idx))
                shapes.append(inp[2].known_shape)
            if isinstance(params, Broadcastable):
                params.set_broadcast(shapes)
        all_nodes[node.output[0]] = (params, 0, outputs[0])
        return params
