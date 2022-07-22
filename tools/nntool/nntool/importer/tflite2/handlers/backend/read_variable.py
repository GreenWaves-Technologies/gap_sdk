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

from nntool.graph.types.constant_input import ConstantInputNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import TFLiteImportException
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.common.constant_mixin import ConstantMixin

from ..backend_handler import BackendHandler
from ..handler import tflite_op, partial_support, ps_description


@tflite_op("READ_VARIABLE")
@partial_support(True)
@ps_description('Supported at import. Variables are folded into the graph. Values must be constants.')
class ReadVariable(BackendHandler, ConstantMixin):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        all_nodes = kwargs['all_nodes']
        variables = kwargs['variables']

        inputs = [all_nodes[t] for t in node.input]
        if len(inputs) != 1:
            raise TFLiteImportException(f'{node.name} expecting 1 inputs')
        var_ref = inputs[0][0]
        if not isinstance(var_ref, tuple):
            raise TFLiteImportException(f'{node.name} expecting first input to be variable reference')

        if var_ref not in variables:
            raise TFLiteImportException(f'{node.name} VarRef {var_ref} has not been assigned a value before use')
        value = variables[var_ref]
        params = ConstantInputNode(node.name, value=value)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(value.shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
