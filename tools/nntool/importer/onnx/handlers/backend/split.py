
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
from graph.types.base import NNEdge
from graph.types.others import SplitParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("Split")
class Split(ConstantMixin, BackendHandler):
    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]

        x = inputs[0]
        x_shape = x[2].shape
        axis = node.attrs.get('axis', 0)

        if axis < 0:
            axis += len(x_shape)

        assert axis < len(x_shape) and axis >= 0,\
            "axis %s is out of bounds - input dims %s in node %s" % (axis, x_shape, valid_name)

        split_dim = x_shape[axis]
        assert split_dim is not None, "split dimension must be defined"

        split = None
        if cls.SINCE_VERSION >= 13:
            if len(inputs) > 1:
                split = cls.get_constant(inputs[1])
        else:
            split = node.attrs.get('split')
            if split:
                split = np.array(split)
                assert sum(split) == split_dim, "split sizes should add up to total size %s" % valid_name
                assert np.all(split > 0), "split sizes should be greater than zero %s" % valid_name
            else:
                num_outputs = len(node.outputs)
                assert split_dim % num_outputs == 0,\
                    "no split attribute or value and dimension is not divisible by number of outputs %s" % valid_name
                split = np.array([split_dim // num_outputs] * num_outputs)

        split = split.tolist()
        act_slices = []
        out_shapes = []
        out_pshapes = []
        cur = 0
        for idx, split_dim in enumerate(split):
            act_slices.append(
                tuple((cur, cur + split_dim, 1) if didx == axis else (0, dim, 1)
                      for didx, dim in enumerate(x_shape) if dim is not None)
            )
            out_pshape = tuple(split_dim if didx == axis else dim for didx, dim in enumerate(x_shape))
            out_shapes.append(
                tuple(dim for dim in out_pshape if dim is not None)
            )
            out_pshapes.append(
                ProvisionalDim(out_pshape)
            )
            cur += split_dim
        axis -= sum(1 if dim is None else 0 for dim in x_shape[:axis:])
        params = SplitParameters(valid_name, act_slices=act_slices,
                                 out_shapes=out_shapes, axis=axis)
        for idx, out_pshape in enumerate(out_pshapes):
            all_nodes[node.output[idx]] = (params, idx, out_pshape)
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_2(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
