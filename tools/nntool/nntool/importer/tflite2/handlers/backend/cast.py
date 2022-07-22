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

from nntool.graph.types import NNEdge
from nntool.graph.types.constant_input import ConstantInputNode
from nntool.graph.types.misc import NoOPNode
from nntool.importer.tflite2.common import LOG
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.common.tflite_tensor import TFLiteTensorWrapper
from nntool.importer.tflite2.handlers.backend.quantize_mixin import \
    QuantizeMixin
from nntool.importer.tflite2.tflite_schema_head.CastOptions import CastOptions
from nntool.quantization.qtype import QType

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("CAST")
class Cast(BackendHandler, QuantizeMixin):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(CastOptions)
        G = kwargs['G']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]

        if node_opts:
            in_qtype = QType(dtype=TFLiteTensorWrapper.TF_TO_NUMPY_TYPE[node_opts.InDataType()])
            out_qtype = QType(dtype=TFLiteTensorWrapper.TF_TO_NUMPY_TYPE[node_opts.OutDataType()])
            return cls.common_quantize(in_qtype, out_qtype, node, **kwargs)

        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            value = cls.get_constant(x)
            params = ConstantInputNode(node.name, value=value)
        else:
            params = NoOPNode(node.name, desc='cast with no type information')
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, deepcopy(x[2]))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
