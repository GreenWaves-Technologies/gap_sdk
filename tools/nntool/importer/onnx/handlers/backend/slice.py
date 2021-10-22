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

import sys
import numpy as np
from graph.dim import Dim
from graph.types import ConstantInputParameters, NNEdge, StridedSliceParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description


@onnx_op("Slice")
@partial_support(True)
@ps_description("Supports only constant inputs for starts, ends, axes and steps")
class Slice(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, starts, ends, axes, steps, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        x = all_nodes[node.input[0]]
        x_shape = np.array(x[2].shape)
        x_rank = len(x_shape)
        axes = cls._resolve_negative_ranks(axes, len(x_shape)) if axes else tuple(range(x_rank))
        axes_rank = len(axes)
        steps = steps if steps else [1] * axes_rank
        slices = np.stack([starts, ends, steps]).transpose((1, 0))
        p_slices = []
        p_shape = []
        for idx, dim in enumerate(x_shape):
            try:
                if dim is None:
                    p_slices.append(None)
                    p_shape.append(None)
                else:
                    slice_idx = axes.index(idx)  # @IgnoreException
                    begin, end, step = slices[slice_idx]
                    begin = max(min(begin if begin >= 0 else dim + begin, dim), 0)
                    end = max(min(end if end >= 0 else dim + end, dim), -1)
                    # -sys.maxsize is used to indicate 0 in the reverse slice direction
                    # this makes it compatible with the numpy slice
                    p_slices.append((begin, -sys.maxsize if end == -1 else end, step))
                    if step < 0:
                        p_shape.append((begin - end)//-step)
                    else:
                        p_shape.append((end - begin)//step)

            except ValueError:
                p_slices.append((0, dim, 1))
                p_shape.append(dim)
        slices = cls._get_real_dim(p_slices)
        shape = cls._get_real_dim(p_shape)

        params = StridedSliceParameters(
            valid_name, act_slice=slices, out_shape=shape)
        if cls.is_constant(x):
            x_val = cls.get_constant(x)
            x_val = params.numpy_slice(x_val)
            if x_val.size < 10:
                logger.info("reducing %s to a constant %s", valid_name, x_val)
            else:
                logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(valid_name, dims=Dim.unnamed(x_val.shape), value=x_val, constant_store=G.constant_store)
        else:
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(p_shape))
        return params

    @classmethod
    def _pre10(cls, node, **kwargs):
        starts = node.attrs['starts']
        ends = node.attrs['ends']
        axes = node.attrs['axes'] if 'axes' in node.attrs else None
        return cls._common(node, starts, ends, axes, None, **kwargs)

    @classmethod
    def _post10(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[inp] for inp in node.input]

        starts = tuple(cls.get_constant(inputs[1]))
        ends = tuple(cls.get_constant(inputs[2]))
        axes = tuple(cls.get_constant(inputs[3])) if len(inputs) >= 4 and inputs[3] else None
        steps = tuple(cls.get_constant(inputs[4])) if len(inputs) >= 5 and inputs[4] else None

        return cls._common(node, starts, ends, axes, steps, **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._pre10(node, **kwargs)

    @classmethod
    def version_10(cls, node, **kwargs):
        return cls._post10(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._post10(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._post10(node, **kwargs)
