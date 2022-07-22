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

from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.common import TFLiteImportException
from nntool.importer.common.constant_mixin import ConstantMixin

from ..backend_handler import BackendHandler
from ..handler import tflite_op, partial_support, ps_description

@tflite_op("ASSIGN_VARIABLE")
@partial_support(True)
@ps_description('Supported at import. Variables are folded into the graph. Values must be constants.')
class AssignVariable(BackendHandler, ConstantMixin):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        variables = kwargs['variables']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        if len(inputs) != 2:
            raise TFLiteImportException(f'{node.name} expecting 2 inputs')
        var_ref = inputs[0][0]
        if not isinstance(var_ref, tuple):
            raise TFLiteImportException(f'{node.name} expecting first input to be variable reference')
        if node.output:
            raise TFLiteImportException(f'{node.name} not expecting variable assignment to have an output')

        value = cls._verify_constant(inputs[1])
        variables[var_ref] = value
        return None

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
