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

from graph.types import GlobalPoolingParameters, NNEdge, NoOPParameters
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.tflite_schema_head.ReducerOptions import ReducerOptions
from utils.node_id import NodeId

from ..backend_handler import BackendHandler


class ReducerMixin(object):
    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(ReducerOptions)
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        reduce_type = kwargs['reduce_type']
        opts = kwargs['opts']

        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]

        x_shape = x[2].shape

        axes = cls._verify_constant(inputs[1])
        node.input[1].used = True

        if len(axes.shape) == 0:
            axes = list([int(axes)])
        else:
            axes = sorted(list(axes))

        # convert all negative axis to their true value
        axes = sorted([elem if elem >= 0 else len(x_shape) + elem
                       for elem in axes])

        if not BackendHandler.remove_unspecified_dim(axes):
            params = NoOPParameters(node.name)
            pout_shape = x_shape.copy()
        else:
            pout_shape = [1 if idx in axes and dim is not None else dim for idx, dim in enumerate(x_shape)]
            # subtract 1 from axis for all None's preceeding it and remove
            # axes that are not defined
            axes = [ax - sum([1 if dim is None else 0 for dim in x_shape[:ax:]])
                    for ax in axes if x_shape[ax] is not None]
            params = GlobalPoolingParameters(node.name, pool_type=reduce_type, axis=tuple(axes),
                                          keep_dims=node_opts.KeepDims())
            # the reduced axes are set to 1 in the output shape

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = BackendHandler.load_tf_quantization(
                [node.input[0]], node.output)

        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pout_shape))
        return params
