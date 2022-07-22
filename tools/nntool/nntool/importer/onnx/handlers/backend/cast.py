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


from nntool.graph.dim import Dim
from nntool.graph.types import ConstantInputNode, NNEdge
from nntool.graph.types.misc import QuantizeNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger
from nntool.quantization.qtype import QType

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("Cast")
class Cast(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        to_dtype = node.attrs['to']
        if cls.is_constant(x):
            x_val = cls.get_constant(x)
            x_val = x_val.astype(to_dtype)
            if x_val.size < 10:
                logger.info("reducing %s to a constant %s", valid_name, x_val)
            else:
                logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputNode(valid_name, dims=Dim.unnamed(x_val.shape), value=x_val)
        else:
            params = QuantizeNode(valid_name, to_qtype=QType(dtype=to_dtype))
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(x_shape), None)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
