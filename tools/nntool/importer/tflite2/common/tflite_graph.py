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

from typing import Iterator, List

from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.common.tflite_tensor import TFLiteTensorWrapper

from ..tflite_schema_head.Model import Model
from ..tflite_schema_head.SubGraph import SubGraph
from ..tflite_schema_head.Tensor import Tensor


class TFliteIterator():
    def __init__(self, obj, len_prop, elem_prop) -> None:
        self._obj = obj
        self._len_prop = len_prop
        self._elem_prop = elem_prop
        self._idx = 0

    def __len__(self):
        return getattr(self._obj, self._len_prop)(self._obj)

    def __next__(self):
        if self._idx >= len(self):
            raise StopIteration()
        elem = getattr(self._obj, self._elem_prop)(self._obj, self._idx)
        self._idx += 1
        return elem

    def __iter__(self):
        self._idx = 0
        return self

    def __getitem__(self, idx):
        return getattr(self._obj, self._elem_prop)(self._obj, idx)


class TFLiteGraph():
    def __init__(self, model: Model, subgraph: SubGraph, subgraph_idx: int, name_cache=None, anonymise=False) -> None:
        self._model = model
        self._subgraph = subgraph
        self._subgraph_idx = subgraph_idx
        self._tensors = [TFLiteTensorWrapper(self._subgraph.Tensors(
            idx), self._model) for idx in range(self._subgraph.TensorsLength())]
        self._nodes = [TFLiteNode(self._subgraph.Operators(idx), idx, self._model,
                                  self, name_cache=name_cache, anonymise=anonymise)
                       for idx in range(self._subgraph.OperatorsLength())]

    @classmethod
    def from_model(cls, model: Model, subgraph_idx: int, name_cache=None, anonymise=False):
        return cls(model, model.Subgraphs(subgraph_idx), subgraph_idx,
                   name_cache=name_cache, anonymise=anonymise)

    @property
    def model_version(self) -> int:
        return self._model.Version()

    @property
    def tensors(self) -> Iterator[Tensor]:
        return self._tensors

    @property
    def input(self) -> List[TFLiteTensorWrapper]:
        return [self.tensors[idx] for idx in self._subgraph.InputsAsNumpy()]

    @property
    def output(self) -> List[TFLiteTensorWrapper]:
        return [self.tensors[idx] for idx in self._subgraph.OutputsAsNumpy()]

    @property
    def nodes(self) -> List[TFLiteNode]:
        return self._nodes

    @property
    def idx(self):
        return self._subgraph_idx
