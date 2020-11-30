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

from graph.types import CastParameters, ConstantInputParameters, NNEdge
from importer.common.constant_mixin import ConstantMixin
from importer.tflite2.common import LOG
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.common.tflite_tensor import TFLiteTensorWrapper
from importer.tflite2.tflite_schema_head.CastOptions import CastOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("CAST")
class Cast(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(CastOptions)
        G = kwargs['G']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]

        if node_opts:
            in_dtype = TFLiteTensorWrapper.TF_TO_NUMPY_TYPE[node_opts.InDataType()]
            out_dtype = TFLiteTensorWrapper.TF_TO_NUMPY_TYPE[node_opts.OutDataType()]
        else:
            in_dtype = out_dtype = None
        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            val = cls.get_constant(x)
            if out_dtype:
                val = val.astype(out_dtype)
            params = ConstantInputParameters(node.name, value=val)
        else:
            params = CastParameters(node.name, in_dtype=in_dtype, out_dtype=out_dtype)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, deepcopy(x[2]))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
