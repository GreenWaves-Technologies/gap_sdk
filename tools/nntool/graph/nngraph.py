# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
import os
from typing import Generator, Iterable, Sequence, Union

from utils.graph import Graph, Node

from .dim import Dim
from .dump_tensor import PrintDumper, dump_tensor
from .graph_identity import GraphIdentity
from .manipulations import add_dimensions, adjust_order, calculate_liveness
from .types import (FilterParameters, FusionParameters, InputParameters,
                    OutputParameters)

LOG = logging.getLogger("nntool." + __name__)

class NNGraphError(Exception):
    pass

class GraphStepsNotCalculatedError(NNGraphError):
    pass

class NNGraphState():
    def __init__(self):
        self._state = {
            'liveness': None,
            'steps': None,
            'quantization': None
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
                 filename=None, value_cache=None):
        super().__init__()

        self.model = model

        self.num_inputs = 0
        self.num_outputs = 0

        self.graph_state = NNGraphState()

        self.load_function = None
        self.graphname = name
        # disable value cache for now
#        self.value_cache = value_cache
        self.value_cache = None
        self.graph_identity = GraphIdentity(filename)
        self._info = {
            'quantization': None
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
    def name(self) -> str:
        if self.graphname is None:
            base, _ = os.path.splitext(os.path.basename(self.graph_identity.filename))
            return base
        return self.graphname

    @name.setter
    def name(self, val):
        self.graphname = val

    def set_load_function(self, func):
        self.load_function = func

    def load_tensors(self, file=None):
        assert self.load_function
        self.load_function(self, file)

    def get_in_params(self, name: str) -> set:
        return [edge.params for edge in self.in_edges(name)]

    def get_out_params(self, name: str) -> set:
        return [edge.params for edge in self.out_edges(name)]

    def inputs(self) -> Generator[Node, None, None]:
        return (node for node in self.nodes() if isinstance(node, InputParameters))

    def outputs(self) -> Generator[Node, None, None]:
        return (node for node in self.nodes() if isinstance(node, OutputParameters))

    def is_input(self, node_name: Union[str, Node]) -> bool:
        if isinstance(node_name, str):
            return isinstance(self[node_name], InputParameters)
        return isinstance(node_name, InputParameters)

    def is_output(self, node_name: Union[str, Node]) -> bool:
        if isinstance(node_name, str):
            return isinstance(self[node_name], OutputParameters)
        return isinstance(node_name, OutputParameters)

    def add_input(self, c: int, w: int, h: int, order: Iterable) -> str:
        self.num_inputs += 1
        node_name = "input_"+str(self.num_inputs)
        self.add_node(InputParameters(node_name, Dim.named(c=c, h=h, w=w, order=order)))
        return node_name

    def add_output(self) -> str:
        self.num_outputs += 1
        node_name = "output_"+str(self.num_outputs)
        self.add_node(OutputParameters(node_name))
        return node_name

    def nodes_iterator(self, yield_fusions=True):
        for step_idx, step in enumerate(self.graph_state.steps):
            node = step['node']
            if isinstance(node, FusionParameters):
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

    def get_weights_by_step(self):
        weights = []
        for step in self.graph_state.steps:
            node = step['node']
            if isinstance(node, (FilterParameters, FusionParameters)):
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
