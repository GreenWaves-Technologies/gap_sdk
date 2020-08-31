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

from graph.dim import Dim
from graph.dump_tensor import PrintDumper, dump_tensor
from graph.graph_identity import GraphIdentity
from graph.manipulations import (add_dimensions, adjust_order,
                                 balance_all_filters, balance_filter,
                                 calculate_liveness)
from graph.types import (ConstantInputParameters, ConvFusionParameters,
                         InputBaseParameters, InputParameters,
                         MultiplicativeBiasParameters, OutputParameters,
                         RecurrentOutputParameters, SSDDetectorParameters)
from quantization.quantization_set import QuantizationSet
from utils.graph import Graph, Node
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId
from interpreter.commands.imageformat import insert_formatter

LOG = logging.getLogger("nntool." + __name__)


class NNGraphError(Exception):
    pass


class GraphStepsNotCalculatedError(NNGraphError):
    pass


class NNGraphChanges(JsonSerializable):
    def __init__(self, init=None):
        if init is not None:
            self._changes = init['changes']
            self._image_format = init.get('image_format') or {}
            return
        self._changes = []
        self._image_format = {}

    def _encapsulate(self):
        return {'changes': self._changes, 'image_format': self._image_format}

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init=val)

    def image_format(self, input_node_name, formatter, normalizer):
        if formatter is None and normalizer is None:
            if input_node_name in self._image_format:
                del self._image_format[input_node_name]
            return
        self._image_format[input_node_name] = {"formatter": formatter, "normalizer": normalizer}

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
        graph_changed = False
        for input_node_name, params in self._image_format.items():
            graph_changed = True
            out_edge = G.out_edges(input_node_name)[0]
            insert_formatter(G, out_edge, params["formatter"], params["normalizer"])
        if graph_changed:
            G.add_dimensions()


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
    def __init__(self,
                 model=None,
                 name=None,
                 filename=None,
                 constant_store=None):
        super().__init__()

        self.model = model

        self.num_inputs = 0
        self.num_outputs = 0
        self.num_constants = 0
        self.node_options = {}
        self.num_rinputs = 0
        self.num_routputs = 0

        self.graph_state = NNGraphState()

        self.load_function = None
        self.graphname = name
        self.constant_store = constant_store
        self.graph_identity = GraphIdentity(filename)
        self._info = {
            'quantization': None,
            'changes': NNGraphChanges()
        }

    @property
    def info(self):
        return self._info

    @info.setter
    def info(self, val):
        self._info = val

    @property
    def quantization(self) -> QuantizationSet:
        return self._info.get('quantization')

    @quantization.setter
    def quantization(self, val: QuantizationSet):
        self._info['quantization'] = val

    @property
    def has_quantized_parameters(self) -> bool:
        return self._info.get('has_quantized_parameters')

    @has_quantized_parameters.setter
    def has_quantized_parameters(self, val: bool):
        self._info['has_quantized_parameters'] = val

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

    @property
    def has_ssd_postprocess(self):
        for node in self.nodes():
            if isinstance(node, SSDDetectorParameters):
                return True
        return False

    def set_load_function(self, func):
        self.load_function = func

    def load_tensors(self, file=None):
        assert self.load_function
        self.load_function(self, file)

    def get_in_params(self, name: str) -> set:
        in_edges = self.in_edges(name)
        if not in_edges:
            return in_edges
        in_edges.sort(key=lambda edge: edge.to_idx)
        res = []
        in_idx = 0
        for real_idx in range(max(edge.to_idx for edge in in_edges) + 1):
            if real_idx == in_edges[in_idx].to_idx:
                res.append(in_edges[in_idx].params)
                in_idx += 1
            else:
                res.append(None)

        return res

    def get_out_params(self, name: str) -> set:
        out_edges = self.out_edges(name)
        out_edges.sort(key=lambda edge: edge.from_idx)
        return [edge.params for edge in out_edges]

    def all_inputs(self) -> Generator[Node, None, None]:
        return (node for node in self.nodes() if isinstance(node, (InputBaseParameters)))

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
        self.num_rinputs = 0
        self.num_routputs = 0

    def add_routput(self, name: str, rinput: ConstantInputParameters) -> RecurrentOutputParameters:
        self.num_routputs += 1
        node = RecurrentOutputParameters(name, input_node=rinput)
        self.add_node(node)
        return node

    def add_input(self, dim: Dim) -> InputParameters:
        self.num_inputs += 1
        node_name = "input_"+str(self.num_inputs)
        node = InputParameters(node_name, dims=dim)
        self.add_node(node)
        return node

    def add_constant(self, dim: Dim, name: str = None,
                     adjust_transpose=None, is_mutated=False,
                     is_intermediate=False, short_name=None) -> ConstantInputParameters:
        self.num_constants += 1
        node_name = name if name else "constant_"+str(self.num_constants)
        node = ConstantInputParameters(node_name, dims=dim,
                                       adjust_transpose=adjust_transpose,
                                       is_intermediate=is_intermediate,
                                       is_mutated=is_mutated,
                                       short_name=short_name)
        self.add_node(node)
        return node

    def variable_in_edges(self, node_name):
        return list([edge for edge in self.in_edges(node_name)
                     if not isinstance(edge.from_node, ConstantInputParameters)])

    def add_output(self) -> OutputParameters:
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

    def adjust_order(self, reshape_weights=True, postprocess=True):
        adjust_order(self, reshape_weights=reshape_weights, postprocess=postprocess)
        LOG.info("adjusted order")
        self.graph_identity.is_adjusted = True

    def add_dimensions(self):
        LOG.info("update graph dimensions")
        self.graph_state.steps = add_dimensions(self)
        LOG.info("calculate liveness")
        self.graph_state.liveness = calculate_liveness(self, self.graph_state.steps)

    def balance_filters(self, step_idx=None, precision_threshold=0.20):
        if step_idx is not None:
            if step_idx > len(self.graph_state.steps) or step_idx < 0:
                raise ValueError("step idx out of range")
            pnode = self.graph_state.steps[step_idx]['node']
            if isinstance(pnode, ConvFusionParameters):
                fnode = pnode.contained_filters()
                if len(fnode) > 1:
                    raise NotImplementedError(
                        "fusions with more than one contained filter is not supported")
                fnode = fnode[0]
                node = fnode
            else:
                node = pnode
                fnode = None
            if not isinstance(node, MultiplicativeBiasParameters):
                raise ValueError(
                    "weights can only be balanced on nodes that support multiplicative bias")
            balance_filter(pnode, fnode=fnode, G=self)
        else:
            balance_all_filters(self, precision_threshold=precision_threshold)

    def print_intermediates(self, outputs, limit=None, width=8,
                            precision=4, channel=None, order=None):
        def print_step(step, outs):
            node = step['node']
            print(node.name)
            for out_idx, out in enumerate(outs):
                dims = node.out_dims[out_idx]
                if order is not None and dims.is_named and order != dims.order and all(k in dims.order
                                                                                       for k in order):
                    transpose = dims.transpose_to_order(order)
                    out = out.transpose(transpose)
                if channel is not None:
                    out = out[channel:channel+1:1, ...]
                dump_tensor(out, PrintDumper(out, width=width, precision=precision))

        if limit is not None:
            print_step(self.graph_state.steps[limit], outputs[limit])
        else:
            for idx, out in enumerate(outputs):
                print_step(self.graph_state.steps[idx], out)
        print()
