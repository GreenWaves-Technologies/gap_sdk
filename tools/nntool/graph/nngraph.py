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
import re
from typing import Generator, Sequence, Union

import numpy as np
from quantization.quantization_set import QuantizationSet
from reports.graph_reporter import GraphReporter
from reports.quantization_reporter import QuantizationReporter
from utils.graph import Graph, Node
from utils.tabular import TextTableRenderer

from graph.dim import Dim
from graph.dump_tensor import PrintDumper, dump_tensor
from graph.graph_identity import GraphIdentity
from graph.manipulations import (add_dimensions, adjust_order,
                                 balance_all_filters, calculate_liveness)
from graph.manipulations.balance_filter import balance_filter_with_constants
from graph.types import (ConstantInputParameters, InputBaseParameters,
                         InputParameters, MultiplicativeBiasParameters,
                         OutputParameters, ResizerParameters,
                         RNNBaseParameters, SSDDetectorParameters)
from graph.types.base import NNNodeRef
from graph.types.dsp_preprocessing import DSPParameters
from graph.types.expression_fusion import ExpressionFusionParameters
from graph.types.fusions import FilterFusionBase, FusionBase

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
                 filename=None):
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
        self.graph_identity = GraphIdentity(filename)
        self._info = {
            'quantization': None,
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

    INVALID_CHARS = re.compile(r'[^A-Za-z0-9_]')

    @staticmethod
    def valid_c_identifier(val: str) -> str:
        return NNGraph.INVALID_CHARS.sub('_', val)


    @property
    def name(self) -> str:
        if self.graphname is None:
            base, _ = os.path.splitext(
                os.path.basename(self.graph_identity.filename))
            return self.valid_c_identifier(base)
        return self.valid_c_identifier(self.graphname)

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
        return self.has_node_type(SSDDetectorParameters)

    @property
    def has_resizer(self):
        return self.has_node_type(ResizerParameters)

    @property
    def has_expressions(self):
        return self.has_node_type(ExpressionFusionParameters)

    @property
    def has_rnn(self):
        return self.has_node_type(RNNBaseParameters)

    @property
    def has_dsp(self):
        return self.has_node_type(DSPParameters)

    @property
    def all_expressions(self):
        return self.all_node_types(ExpressionFusionParameters)

    def has_node_type(self, node_type):
        return any(isinstance(node, node_type) for node in self.nodes())

    def all_node_types(self, node_type):
        return [node for node in self.nodes() if isinstance(node, node_type)]

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
        out_edges = self.indexed_out_edges(name)
        return [edge_list[0].params for edge_list in out_edges]

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

    def add_input(self, dim: Dim, name=None, **kwargs) -> InputParameters:
        self.num_inputs += 1
        node_name = "input_"+str(self.num_inputs) if not name else name
        node = InputParameters(node_name, dims=dim, **kwargs)
        self.add_node(node)
        return NNNodeRef(node, 0, self)

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
        return NNNodeRef(node, 0, self)

    def variable_in_edges(self, node_name):
        return list([edge for edge in self.in_edges(node_name)
                     if not isinstance(edge.from_node, ConstantInputParameters)])

    def add_output(self, name=None) -> OutputParameters:
        self.num_outputs += 1
        node_name = "output_"+str(self.num_outputs) if name is None else name
        node = OutputParameters(node_name)
        self.add_node(node)
        return node

    def nodes_iterator(self, yield_fusions=True):
        for step_idx, step in enumerate(self.graph_state.steps):
            node = step['node']
            if isinstance(node, (FusionBase)) and not isinstance(node, ExpressionFusionParameters):
                if yield_fusions:
                    for fusion_idx, fnode in enumerate(node.contained_nodes()):
                        yield (step_idx, node, fusion_idx, fnode)
                yield (step_idx, node, None, None)
            else:
                yield (step_idx, node, None, None)

    def adjust_order(self, reshape_weights=True, postprocess=True, debug_function=None, steps=None, single_step=False):
        adjust_order(self, reshape_weights=reshape_weights,
                     postprocess=postprocess, debug_function=debug_function,
                     steps=steps, single_step=single_step)
        LOG.info("adjusted order")
        self.graph_identity.is_adjusted = True

    def add_dimensions(self, quiet=False):
        if not quiet:
            LOG.info("update graph dimensions")
        self.graph_state.steps = add_dimensions(self)
        if not quiet:
            LOG.info("calculate liveness")
        self.graph_state.liveness = calculate_liveness(
            self,
            self.graph_state.steps)

    def balance_filters(self, step_idx=None, precision_threshold=0.20):
        if step_idx is not None:
            if step_idx > len(self.graph_state.steps) or step_idx < 0:
                raise ValueError("step idx out of range")
            pnode = self.graph_state.steps[step_idx]['node']
            if isinstance(pnode, FilterFusionBase):
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
            balance_filter_with_constants(
                self, pnode, fnode=fnode, precision_threshold=precision_threshold)
        else:
            balance_all_filters(self, precision_threshold=precision_threshold)

    def print_intermediates(self, outputs, limit=None, width=8,
                            precision=4, channel=None, order=None,
                            checksum=False, print_constants=False):
        def print_step(step, outs, index):
            node = step['node']
            if checksum:
                for out_idx, out in enumerate(outs):
                    if isinstance(node, ConstantInputParameters):
                        continue
                    print(f"S{index} - {node.name}\n\tChecksum = {np.sum(out) if out.dtype != np.uint8 else np.sum(out.astype(np.int8))}")
            else:
                print(node.name)
                for out_idx, out in enumerate(outs):
                    dims = node.out_dims[out_idx]
                    if order is not None and dims.is_named and order != dims.order and all(k in dims.order
                                                                                           for k in order):
                        transpose = dims.transpose_to_order(order)
                        out = out.transpose(transpose)
                    if channel is not None:
                        out = out[channel:channel+1:1, ...]
                    dump_tensor(out, PrintDumper(
                        out, width=width, precision=precision))

        if limit is not None:
            print_step(self.graph_state.steps[limit], outputs[limit], limit)
        else:
            for idx, out in enumerate(outputs):
                print_step(self.graph_state.steps[idx], out, idx)
        print()

    def qshow(self):
        tab = QuantizationReporter().report(self, self.quantization)
        renderer = TextTableRenderer(150)
        tab.render(renderer)
        return renderer.get_output()

    def merge(self, other: 'NNGraph'):
        if self != other:
            for edge in other.edges:
                self.add_edge(edge)
        return self

    def __repr__(self):
        tab = GraphReporter().report(self)
        renderer = TextTableRenderer(150)
        tab.render(renderer)
        return renderer.get_output()

    def total_ops(self):
        tot_ops = 0
        for node in self.nodes():
            ops = node.compute_load()
            tot_ops += ops if ops else 0
        return tot_ops
