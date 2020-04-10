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
import os
from typing import Generator, Sequence, Union

from utils.graph import Graph, Node
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId

from .dim import Dim
from .dump_tensor import PrintDumper, dump_tensor
from .graph_identity import GraphIdentity
from .manipulations import add_dimensions, adjust_order, calculate_liveness, balance_filter
from .types import (ConstantInputParameters, FilterParameters,
                    ConvFusionParameters, InputBaseParameters, InputParameters,
                    OutputParameters, MultiplicativeBiasParameters)

LOG = logging.getLogger("nntool." + __name__)

class NNGraphError(Exception):
    pass

class GraphStepsNotCalculatedError(NNGraphError):
    pass

class NNGraphAttributeChanges(JsonSerializable):
    def __init__(self, init=None):
        if init is not None:
            self._changes = init['changes']
            return
        self._changes = []

    def _encapsulate(self):
        return {'changes': self._changes}

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init=val)

    def modify(self, node, attr, val, fnode=None):
        nid = NodeId(node, fnode)
        self._changes.append({
            'nid': nid,
            'attr': attr,
            'val': val
        })
        if fnode is not None:
            node = fnode
        setattr(node, attr, val)

    def replay(self, G):
        for change in self._changes:
            node = change['nid'].get_node(G)
            setattr(node, change['attr'], change['val'])

class NNGraphState():
    def __init__(self):
        self._state = {
            'liveness': None,
            'steps': None,
            'quantization': None,
        }

    @property
    def steps(self) -> Sequence[Node]:
        if not self._state['steps']:
            raise GraphStepsNotCalculatedError()
        return self._state['steps']

    @steps.setter
    def steps(self, val: Sequence[Node]):
        self._state['steps'] = val

    @property
    def liveness(self):
        if not self._state['liveness']:
            raise GraphStepsNotCalculatedError()
        return self._state['liveness']

    @liveness.setter
    def liveness(self, val):
        self._state['liveness'] = val

    @property
    def has_quantization_info(self):
        return self._state['quantization']

    @has_quantization_info.setter
    def has_quantization_info(self, val):
        self._state['quantization'] = val

class NNGraph(Graph):
    def __init__(self, model=None, name=None,
                 filename=None, value_cache=None,
                 constant_store=None):
        # TODO - Value caching disabled
        del value_cache
        super().__init__()

        self.model = model

        self.num_inputs = 0
        self.num_outputs = 0
        self.num_constants = 0
        self.node_options = {}

        self.graph_state = NNGraphState()

        self.load_function = None
        self.graphname = name
        # disable value cache for now
#        self.value_cache = value_cache
        self.value_cache = None
        self.constant_store = constant_store
        self.graph_identity = GraphIdentity(filename)
        self._info = {
            'quantization': None,
            'changes': NNGraphAttributeChanges()
        }

    @property
    def info(self):
        return self._info

    @info.setter
    def info(self, val):
        self._info = val

    @property
    def quantization(self):
        return self._info['quantization']

    @quantization.setter
    def quantization(self, val):
        self._info['quantization'] = val

    @property
    def changes(self):
        return self._info['changes']

    @property
    def name(self) -> str:
        if self.graphname is None:
            base, _ = os.path.splitext(os.path.basename(self.graph_identity.filename))
            return base
        return self.graphname

    @property
    def inputs_dim(self) -> list:
        return [in_node.in_dims[0].shape for in_node in self.input_nodes()]

    @property
    def outputs_dim(self) -> list:
        return [out_node.out_dims[0].shape for out_node in self.output_nodes()]

    @name.setter
    def name(self, val):
        self.graphname = val

    def set_load_function(self, func):
        self.load_function = func

    def load_tensors(self, file=None):
        assert self.load_function
        self.load_function(self, file)

    def get_in_params(self, name: str) -> set:
        in_edges = self.in_edges(name)
        in_edges.sort(key=lambda edge: edge.to_idx)
        return [edge.params for edge in in_edges]

    def get_out_params(self, name: str) -> set:
        out_edges = self.out_edges(name)
        out_edges.sort(key=lambda edge: edge.from_idx)
        return [edge.params for edge in out_edges]

    def inputs_and_constants(self) -> Generator[Node, None, None]:
        return (node for node in self.nodes() if isinstance(node, InputBaseParameters))

    def input_nodes(self) -> Generator[Node, None, None]:
        return (node for node in self.nodes() if isinstance(node, InputParameters))

    def output_nodes(self) -> Generator[Node, None, None]:
        return (node for node in self.nodes() if isinstance(node, OutputParameters))

    def is_input(self, node_name: Union[str, Node]) -> bool:
        if isinstance(node_name, str):
            return isinstance(self[node_name], InputParameters)
        return isinstance(node_name, InputParameters)

    def is_output(self, node_name: Union[str, Node]) -> bool:
        if isinstance(node_name, str):
            return isinstance(self[node_name], OutputParameters)
        return isinstance(node_name, OutputParameters)

    def reset_inout_counts(self):
        self.num_inputs = 0
        self.num_outputs = 0
        self.num_constants = 0

    def add_input(self, dim: Dim) -> str:
        self.num_inputs += 1
        node_name = "input_"+str(self.num_inputs)
        node = InputParameters(node_name, dims=dim)
        self.add_node(node)
        return node

    def add_constant(self, dim: Dim) -> str:
        self.num_constants += 1
        node_name = "constant_"+str(self.num_constants)
        node = ConstantInputParameters(node_name, dims=dim)
        self.add_node(node)
        return node

    def add_output(self) -> str:
        self.num_outputs += 1
        node_name = "output_"+str(self.num_outputs)
        node = OutputParameters(node_name)
        self.add_node(node)
        return node

    def nodes_iterator(self, yield_fusions=True):
        for step_idx, step in enumerate(self.graph_state.steps):
            node = step['node']
            if isinstance(node, ConvFusionParameters):
                if yield_fusions:
                    for fusion_idx, fnode in enumerate(node.contained_nodes()):
                        yield (step_idx, node, fusion_idx, fnode)
                yield (step_idx, node, None, None)
            else:
                yield (step_idx, node, None, None)

    def adjust_order(self, reshape_weights=True):
        adjust_order(self, reshape_weights)
        LOG.info("adjusted order")
        self.graph_identity.is_adjusted = True

    def add_dimensions(self):
        LOG.info("update graph dimensions")
        self.graph_state.steps = add_dimensions(self)
        LOG.info("calculate liveness")
        self.graph_state.liveness = calculate_liveness(self, self.graph_state.steps)

    def balance_filter(self, step_idx):
        if step_idx > len(self.graph_state.steps) or step_idx < 0:
            raise ValueError("step idx out of range")
        node = self.graph_state.steps[step_idx]['node']
        if not isinstance(node, MultiplicativeBiasParameters):
            raise ValueError("weights can only be balanced on nodes that support multiplicative bias")
        balance_filter(node)

    def get_weights_by_step(self):
        weights = []
        for step in self.graph_state.steps:
            node = step['node']
            if isinstance(node, (FilterParameters, ConvFusionParameters)):
                weights.append({'weights': node.quantized_weights, 'biases': node.quantized_biases})
            else:
                weights.append(None)
        return weights

    def print_intermediates(self, outputs, limit=None, width=8,
                            precision=4, channel=None, order=None):
        def print_step(step, outs):
            node = step['node']
            print(node.name)
            for out_idx, out in enumerate(outs):
                dims = node.out_dims[out_idx]
                if order is not None and order != dims.order:
                    transpose = dims.transpose_to_order(order)
                    out = out.transpose(transpose)
                if channel is not None:
                    out = out[channel].reshape((1, dims.h, dims.w))
                dump_tensor(out, PrintDumper(out, width=width, precision=precision))

        if limit is not None:
            print_step(self.graph_state.steps[limit], outputs[limit])
        else:
            for idx, out in enumerate(outputs):
                print_step(self.graph_state.steps[idx], out)
