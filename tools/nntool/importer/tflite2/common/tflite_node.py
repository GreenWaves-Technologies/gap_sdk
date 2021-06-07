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
#

import array

from flatbuffers.flexbuffers import GetRoot
from importer.common.get_reasonable_name import get_reasonable_name

from ..tflite_schema_head.BuiltinOperator import BuiltinOperator
from ..tflite_schema_head.Model import Model
from ..tflite_schema_head.Operator import Operator

TFLITE_BUILTIN_OPERATOR_NAMES = {
    getattr(BuiltinOperator, attr): attr
    for attr in dir(BuiltinOperator) if not attr.startswith("__")
}


class TFLiteNode():
    def __init__(self, operator: Operator, op_idx, model: Model, graph, name_cache=None, anonymise=False) -> None:
        self._op = operator
        self._op_idx = op_idx
        self._model = model
        self._graph = graph
        self._overriden_outputs = None
        self._name = get_reasonable_name(f"{self.op_name}_{self._graph.idx}_{self._op_idx}",
                                         name_cache=name_cache, anonymise=anonymise, length=40)

    def override_outputs(self, val):
        self._overriden_outputs = val

    @property
    def name(self):
        return self._name

    @property
    def idx(self):
        return self.idx

    @property
    def is_custom(self):
        return self.op_name == 'CUSTOM'

    @property
    def op_code(self):
        return self._model.OperatorCodes(self._op.OpcodeIndex())

    @property
    def op_version(self):
        return self.op_code.Version()

    @property
    def op_number(self):
        op_code = self.op_code
        if hasattr(op_code, "DeprecatedBuiltinCode"):
            if op_code.BuiltinCode() > BuiltinOperator.PLACEHOLDER_FOR_GREATER_OP_CODES:
                op_number = op_code.BuiltinCode()
            else:
                op_number = op_code.DeprecatedBuiltinCode()
        else:
            op_number = op_code.DeprecatedBuiltinCode()
        return op_number

    @property
    def op_name(self):
        return TFLITE_BUILTIN_OPERATOR_NAMES[self.op_number]

    @property
    def custom_op_name(self):
        return str(self.op_code.CustomCode(), 'utf-8') if self.is_custom else ""

    @property
    def op_type(self):
        op_name = self.op_name
        if op_name == 'CUSTOM':
            return str(self.op_code.CustomCode(), 'utf-8')
        return op_name

    @property
    def output(self):
        return self._overriden_outputs or [self._graph.tensors[idx] if idx >= 0 else None
                                           for idx in self._op.OutputsAsNumpy()]

    @property
    def input(self):
        return [self._graph.tensors[idx] if idx >= 0 else None for idx in self._op.InputsAsNumpy()]

    def is_mutated(self, input_idx):
        return (self._op.MutatingVariableInputsLength() > 0 and
                bool(self._op.MutatingVariableInputsAsNumpy()[input_idx]))

    def is_intermediate(self, input_idx):
        return (self._op.IntermediatesLength() > 0 and
                bool(self._op.IntermediatesAsNumpy()[input_idx]))

    def get_options(self, options_class):
        if self._op.BuiltinOptions():
            opts = options_class()
            opts.Init(self._op.BuiltinOptions().Bytes,
                      self._op.BuiltinOptions().Pos)
            return opts
        return None

    def get_custom_options(self):
        flex_buffer = GetRoot(array.array(
            'B', self._op.CustomOptionsAsNumpy()).tobytes()).AsMap
        return flex_buffer.Value
