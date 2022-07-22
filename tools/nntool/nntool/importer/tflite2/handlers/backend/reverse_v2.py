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

from nntool.graph.types import NNEdge, NoOPNode, ReverseNode
from nntool.importer.tflite2.common import TFLiteImportException
from nntool.importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import partial_support, ps_description, tflite_op


@tflite_op("REVERSE_V2")
@partial_support(True)
@ps_description("reverse is limited to one dimension")
class Reverse_V2(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        axes = list(cls._verify_constant(inputs[1]))
        node.input[1].used = True
        if len(axes) > 1:
            raise TFLiteImportException("reverses of more than one dimension are not supported")
        axis = axes[0]
        if x_shape[axis] is None:
            params = NoOPNode(node.name, desc="reversed removed dimension")
        else:
            axis -= sum([1 if dim is None else 0 for dim in x_shape[:axis:]])
            params = ReverseNode(node.name, axis=axis)
        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization([node.input[0]], node.output)
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, deepcopy(x[2]))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
