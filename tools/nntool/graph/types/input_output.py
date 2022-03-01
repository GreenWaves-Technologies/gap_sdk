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

import logging
import sys

import numpy as np
from graph.dim import Dim

from .base import (InsensitiveToQuantization, NNNodeRef, Parameters,
                   cls_op_name, not_generated)

LOG = logging.getLogger("nntool." + __name__)


@not_generated
class InputOutputParameters(Parameters):

    def __init__(self, *args, imported_dtype=None, dims=None, fixed_order=False, short_name=None, **kwargs):
        super().__init__(*args, **kwargs)
        self._output_value = None
        self._index = None
        self._short_name = short_name
        self.imported_dtype = imported_dtype
        if isinstance(dims, (tuple, list)):
            dims = Dim.unnamed(dims)
        self.dims = dims
        self.at_options.valid_options['ALLOCATE'] = int
        self.at_options.valid_options['FIXED_ORDER'] = int
        self.fixed_order = fixed_order

    @property
    def graph_anon_label(self):
        return self.graph_label

    @property
    def graph_label(self):
        shape = (self.out_dims[0] if self.out_dims and
                 self.out_dims[0] is not None else "No Shape!")
        if self.fixed_order:
            return [self.name, f'{shape}', "Frozen Order"]
        return [self.name, f'{shape}']

    @property
    def short_name(self):
        return self._short_name

    @short_name.setter
    def short_name(self, val):
        self._short_name = val

    @property
    def fixed_order(self):
        if hasattr(self.at_options, 'fixed_order'):
            return self.at_options.fixed_order == 1
        return False

    @fixed_order.setter
    def fixed_order(self, val):
        self.at_options.fixed_order = 1 if val else 0

    @property
    def output_value(self):
        return self._output_value

    @output_value.setter
    def output_value(self, value):
        self._output_value = value

    @property
    def index(self):
        return self._index

    @index.setter
    def index(self, value):
        self._index = value

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0


class InputBaseParameters(InputOutputParameters):

    @property
    def in_dims(self):
        dim = self.dims.clone()
        if self.in_dims_hint:
            dim.apply_naming_hints(self.in_dims_hint[0])
        return [dim]

    @in_dims.setter
    def in_dims(self, val):
        pass

    def __str__(self):
        return "I {} {}".format(
            self.dims,
            self.at_options
        )

    def get_output_size(self, _):
        out_dim = self.dims.clone()
        if self.out_dims_hint:
            out_dim.apply_naming_hints(self.out_dims_hint[0])
        return [out_dim]


@cls_op_name('input')
class InputParameters(InputBaseParameters, InsensitiveToQuantization):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.at_options.valid_options['EXTERN_INPUT_POINTER'] = int

    def __call__(self, graph):
        if graph.__class__.__name__ != 'NNGraph':
            raise ValueError('expecting NNGraph as parameter')
        return NNNodeRef(graph, self, 0)

    def verify(self, G):
        problems = []
        for edge in G.in_edges(self.name):
            problems.append(
                f'input node {self.name} has input edge from {edge.from_node.name}:{edge.from_idx}')
        for edge in G.out_edges(self.name):
            if edge.from_idx > 0:
                problems.append(f'input node {self.name} has output edge on idx {edge.from_idx} '
                                f'to {edge.to_node.name}:{edge.to_idx}')
        return problems

    def set_input(self, value):
        try:
            value = value.reshape(self.dims.shape)
        except ValueError as ex:
            trace_back = sys.exc_info()[2]
            raise ValueError(
                "Input data dimensions are not compatible with graph input: {!s}".format(
                    ex)
            ).with_traceback(trace_back)
        self.output_value = value


@cls_op_name('output')
class OutputParameters(InputOutputParameters, InsensitiveToQuantization):

    def __init__(self, *args, **kwargs):
        super(OutputParameters, self).__init__(*args, **kwargs)
        self.at_options.valid_options['EXTERN_OUTPUT_POINTER'] = int

    def verify(self, G):
        problems = []
        for edge in G.out_edges(self.name):
            problems.append(
                f'output node {self.name} has output edge to {edge.to_node.name}:{edge.to_idx}')
        for edge in G.in_edges(self.name):
            if edge.to_idx > 0:
                problems.append(f'output node {self.name} has input edge on idx {edge.to_idx} '
                                f'from {edge.from_node.name}:{edge.from_idx}')
        return problems

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        return [out_dim]

    @property
    def out_dims(self):
        return [self.dims]

    @out_dims.setter
    def out_dims(self, val):
        self.dims = val[0]

    def __str__(self):
        return "O {} {}".format(
            self.dims,
            self.at_options
        )
