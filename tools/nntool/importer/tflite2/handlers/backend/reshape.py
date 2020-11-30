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
from importer.common.provisional_dim import ProvisionalDim

from graph.dim import Dim
from graph.types import (NNEdge, NoOPParameters, ReshapeParameters,
                         StridedSliceParameters)
from importer.tflite2.common import LOG
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.ReshapeOptions import \
    ReshapeOptions
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("RESHAPE")
class Reshape(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(ReshapeOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        set_shape_tensor = list(cls._verify_constant(inputs[1]))
        node.input[1].used = True

        # TF2 seems to use the second input whereas TF1 uses the opts
        new_shape = list(node_opts.NewShapeAsNumpy()
                         if node_opts else set_shape_tensor)
        if -1 in new_shape:
            new_shape_size = reduce(lambda x, y: x * 1 if y == -1 else x * y, new_shape, 1)
            inp_size = reduce(lambda x, y: x * y if y is not None else x, x_shape, 1)
            new_shape[new_shape.index(-1)] = inp_size // new_shape_size

        if None in x_shape:
            if 1 in new_shape:
                old_batch_dim = x_shape.index(None)
                new_batch_dim = new_shape.index(1)
                if old_batch_dim != new_batch_dim:
                    LOG.info("node %s moved batch dimension for axis %s to axis %s",
                             node.name, old_batch_dim, new_batch_dim)
                new_shape[new_batch_dim] = None
            else:
                raise ValueError("unable to determine movement of unspcified axis in node %s"%node.name)

        pnew_shape = ProvisionalDim(new_shape)
        old_shape = Dim.unnamed(cls.remove_unspecified_dim(x_shape), is_ordered=True)
        new_shape = Dim.unnamed(cls.remove_unspecified_dim(new_shape), is_ordered=True)

        params = ReshapeParameters(node.name, old_shape=old_shape, shape=new_shape)

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization([node.input[0]], node.output)
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pnew_shape)
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
