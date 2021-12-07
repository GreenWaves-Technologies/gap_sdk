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
from functools import reduce
from graph.types.others import GatherParameters

import numpy as np
from graph.types import ConstantInputParameters
from importer.common.provisional_dim import ProvisionalDim

from graph.dim import Dim
from graph.types import (NNEdge, NoOPParameters, ReshapeParameters)
from importer.tflite2.common import LOG
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.GatherOptions import \
    GatherOptions
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op, partial_support, ps_description


def sequence_to_slice(val):
    if len(val) == 1:
        return (val[0], val[0] + 1, 1)
    if val[0] == val[1]:
        return None
    step = val[1] - val[0]
    for idx, v in enumerate(val[:-1:]):
        if v + step != val[idx + 1]:
            return None
    if val[0] < val[1]:
        return (val[0], val[-1] + 1, step)
    elif val[0] > val[1]:
        return (val[-1], val[0] - 1, step)
    else:
        return None


@tflite_op("GATHER")
@partial_support(True)
@ps_description('Gather is supported on constants and is folded to a constant value. It is not supported on '
                'GAP at present.')
class Gather(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(GatherOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        gather_tensor = list(cls._verify_constant(inputs[1]))
        axis = node_opts.Axis()

        if cls._is_constant(x):
            inp = cls._get_constant(x)
            inp = np.take(inp, gather_tensor, axis=axis)
            pout_shape = inp.shape
            LOG.info("reducing %s to a constant", node.name)
            params = ConstantInputParameters(
                node.name, value=inp, dims=pout_shape)
        else:
            if x_shape[axis] is None:
                raise ValueError(
                    f'GATHER {node.name} on batch axis not supported')
            slices = [sequence_to_slice(elem) for elem in gather_tensor]
            strides = set([slices[idx + 1][0] - slice[0]
                           for idx, slice in enumerate(slices[:-1:])])
            lengths = set([abs(slice[1] - slice[0]) for slice in slices])
            if len(strides) != 1 or len(lengths) != 1:
                raise ValueError(f'Irregular GATHER {node.name} not supported')
            out_len = sum(len(slice) for slice in slices)
            pout_shape = x_shape.copy()
            pout_shape[axis] = out_len
            axis -= sum(1 if dim is None else 0 for dim in x_shape[:axis:])
            LOG.info("reducing %s to an overlapped copy", node.name)
            params = GatherParameters(
                node.name,
                indices=gather_tensor,
                axis=axis)

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization(
                node.input, node.output)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pout_shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
