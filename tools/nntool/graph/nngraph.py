# Copyright (C) 2020, 2022  GreenWaves Technologies, SAS

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
from typing import Any, Callable, Generator, Mapping, Sequence, Tuple, Union

import numpy as np
from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from interpreter.commands.qtune import SCHEME_NAME_MAPPINGS
from quantization.quantization_set import QuantizationSet
from quantization.quantizer.new_quantizer import NewQuantizer
from reports.graph_reporter import GraphReporter
from stats.activation_ranges_collector import ActivationRangesCollector
from utils.graph import Graph, Node
from utils.node_id import NodeId
from utils.stats_funcs import cos_similarity, qsnr
from utils.tabular import TextTableRenderer

from graph.dim import Dim
from graph.graph_identity import GraphIdentity
from graph.manipulations.adjust_order import adjust_order
from graph.manipulations.balance_filter import (balance_all_filters,
                                                balance_filter_with_constants)
from graph.manipulations.dimensions import add_dimensions
from graph.manipulations.liveness import calculate_liveness
from graph.matches.fusions import fusions
from graph.matches.matches import get_fusions
from graph.types import (ConstantInputParameters, InputBaseParameters,
                         InputParameters, MultiplicativeBiasParameters,
                         OutputParameters, ResizerParameters,
                         RNNBaseParameters, SSDDetectorParameters)
from graph.types.base import NNEdge, NNNodeRef, Parameters
from graph.types.dsp_preprocessing import DSPParameters
from graph.types.expression_fusion import ExpressionFusionParameters
from graph.types.fusions import (ActivationFusionBase, FilterFusionBase,
                                 FusionBase, MatMulOpFusionParameters,
                                 PaddedAddFusionParameters)

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


class NNGraph(Graph):
    def __init__(self,
                 model=None,
                 name=None,
                 filename=None):

        attrs = {
            'model': model,
            'node_options': {},

            'quantization': None,
            'has_quantized_parameters': False,
            'graphname': name,
            'graph_state': NNGraphState(),
            'graph_identity': GraphIdentity(filename)
        }
        super().__init__(**attrs)

    INVALID_CHARS = re.compile(r'[^A-Za-z0-9_]')

    @staticmethod
    def valid_c_identifier(val: str) -> str:
        return NNGraph.INVALID_CHARS.sub('_', val)

    @property
    def _edge_class(self):
        return NNEdge

    @property
    def name(self) -> str:
        """Returns the name of the graph potentially modified to be a valid C identifier

        Returns:
            str: The graph name
        """
        if self._attr['graphname'] is None:
            base, _ = os.path.splitext(
                os.path.basename(self._attr['graph_identity'].filename))
            return self.valid_c_identifier(base)
        return self.valid_c_identifier(self._attr['graphname'])

    @name.setter
    def name(self, val: str):
        """Sets the name of the graph

        Args:
            val (str): The name of the graph
        """
        self._attr['graphname'] = val

    @property
    def model(self):
        """The original model that generated the NNTool graph

        Returns:
            Any: The model file (TFLite or ONNX graph descriptor)
        """
        return self._attr['model']

    @property
    def num_inputs(self) -> int:
        """Current number of inputs

        Returns:
            int: Number of inputs
        """
        return len(self.nodes(node_classes=InputParameters))

    @property
    def num_outputs(self) -> int:
        """Current number of outputs

        Returns:
            int: Number of outputs
        """
        return len(self.nodes(node_classes=OutputParameters))

    @property
    def num_constants(self) -> int:
        """Current number of constant inputs

        Returns:
            int: Number of constant inputs
        """
        return len(self.nodes(node_classes=ConstantInputParameters))

    @property
    def node_options(self) -> dict:
        return self._attr['node_options']

    @property
    def quantization(self) -> Union[QuantizationSet, None]:
        """Current graph Quantization

        Returns:
            Union[QuantizationSet, None]: quantization set
        """
        return self._attr['quantization']

    @quantization.setter
    def quantization(self, val: Union[QuantizationSet, None]):
        """Sets or clears the quantization

        Args:
            val (Union[QuantizationSet, None]): quantization set
        """
        self._attr['quantization'] = val

    @property
    def has_quantized_parameters(self) -> bool:
        """Graph was imported with quantized parameters

        Returns:
            bool: quantized parameters or not
        """
        return self._attr['has_quantized_parameters']

    @has_quantized_parameters.setter
    def has_quantized_parameters(self, val: bool):
        """Graph was imported with quantized parameters

        Args:
            val (bool): quantized parameters or not
        """
        self._attr['has_quantized_parameters'] = val

    @property
    def graph_state(self) -> NNGraphState:
        return self._attr['graph_state']

    @property
    def graph_identity(self) -> GraphIdentity:
        return self._attr['graph_identity']

    @property
    def inputs_dim(self) -> list:
        return [in_node.in_dims[0].shape for in_node in self.input_nodes()]

    @property
    def outputs_dim(self) -> list:
        return [out_node.out_dims[0].shape for out_node in self.output_nodes()]

    @property
    def has_ssd_postprocess(self) -> bool:
        """Graph has SSD detector nodes

        Returns:
            bool: True if present
        """
        return self.has_node_type(SSDDetectorParameters)

    @property
    def has_resizer(self) -> bool:
        """Graph has resizer nodes

        Returns:
            bool: True if present
        """
        return self.has_node_type(ResizerParameters)

    @property
    def has_expressions(self) -> bool:
        """Graph has compiled expressions

        Returns:
            bool: True if present
        """
        return self.has_node_type(ExpressionFusionParameters)

    @property
    def has_dsp(self) -> bool:
        """Graph has DSP nodes

        Returns:
            bool: True if present
        """
        return self.has_node_type(DSPParameters)

    @property
    def all_expressions(self) -> Sequence[ExpressionFusionParameters]:
        """All the expression nodes in the graph

        Returns:
            Sequence[ExpressionFusionParameters]: List of nodes
        """
        return self.nodes(node_classes=ExpressionFusionParameters)

    @property
    def nodes_by_step_idx(self) -> Sequence[Parameters]:
        """All the nodes in the graph ordered by execution order

        Returns:
            Sequence[Parameters]: List of nodes
        """
        return [step['node'] for step in self.graph_state.steps]

    @property
    def nodes_by_step_idx_with_fusions(self) -> Sequence[Parameters]:
        """Nodes ordered by execution order but also including internal nodes
        for fusions

        Returns:
            Sequence[Parameters]: List of nodes
        """
        nodes = []
        for step in self.graph_state.steps:
            node = step['node']
            if isinstance(node, (FilterFusionBase, ActivationFusionBase,
                                 PaddedAddFusionParameters, MatMulOpFusionParameters)):
                nodes.extend(node.contained_nodes())
            nodes.append(node)
        return nodes

    @property
    def total_ops(self) -> int:
        """Estimated total operations in the graph

        Returns:
            int: Number of operations
        """
        tot_ops = 0
        for node in self.nodes():
            ops = node.compute_load()
            tot_ops += ops if ops else 0
        return tot_ops

    def has_rnn(self, ktype: str = None, ne16: bool = False) -> bool:
        """Graph has RNN nodes

        Args:
            ktype (str, optional): kernel type to match or all. Defaults to None.
            ne16 (bool, optional): match nodes that will map to ne16 kernels. Defaults to False.

        Returns:
            bool: True if present
        """
        nodes = self.nodes(node_classes=RNNBaseParameters)
        if not nodes:
            return False
        if ktype is not None and not any(self.quantization[NodeId(node)].ktype == ktype for node in nodes):
            return False
        if ne16 and not any(self.quantization[NodeId(node)].cache.get('ne16') for node in nodes):
            return False
        return True

    def has_node_type(self, node_type: Parameters) -> bool:
        """Returns True if graph contains node type

        Args:
            node_type (Parameters): Node class

        Returns:
            bool: True if present
        """
        return any(isinstance(node, node_type) for node in self.nodes())

    def inputs_and_constants(self) -> Generator[Node, None, None]:
        """Iterate over all inputs and constants

        Returns:
            Generator[Node]: a generator for all nodes
        """
        return (node for node in self.nodes() if isinstance(node, InputBaseParameters))

    def input_nodes(self) -> Generator[Node, None, None]:
        """Iterate over all inputs

        Returns:
            Generator[Node]: a generator for all nodes
        """
        return (node for node in self.nodes() if isinstance(node, InputParameters))

    def output_nodes(self) -> Generator[Node, None, None]:
        """Iterate over all outputs

        Returns:
            Generator[Node]: a generator for all nodes
        """
        return (node for node in self.nodes() if isinstance(node, OutputParameters))

    def add_input(self, dim: Union[Dim, Tuple[int]], name: str = None, **kwargs) -> NNNodeRef:
        """Create an input node. If a name is not supplied then one will be automatically chosen.

        Args:
            dim (Union[Dim, Tuple[int]]): Input dimension
            name (str, optional): Node name. Defaults to None.

        Returns:
            NNNodeRef: Reference to created node in graph
        """
        node_name = self.unique_name(
            f"input_{self.num_inputs + 1}") if not name else name
        node = InputParameters(node_name, dims=dim, **kwargs)
        self.add_node(node)
        return NNNodeRef(self, node, 0)

    def add_constant(self, dim: Union[Dim, Tuple[int]] = None,
                     name: str = None,
                     value: np.ndarray = None,
                     adjust_transpose: Sequence[int] = None,
                     is_mutated=False,
                     is_intermediate=False,
                     short_name: str = None) -> NNNodeRef:
        """Creates a constant node

        Args:
            dim (Union[Dim, Tuple[int]], optional): Dimension of constant if not supplied then a value must be. Defaults to None.
            name (str, optional): Optional name. A unique one will be created if None. Defaults to None.
            value (np.ndarray, optional): Numpy array with value. Defaults to None.
            adjust_transpose (Sequence[int], optional): Adjust will transpose the value using this transpose. Defaults to None.
            is_mutated (bool, optional): Constant is both an input and an output. Defaults to False.
            is_intermediate (bool, optional): Constant is marked as intermediate at import. Defaults to False.
            short_name (str, optional): Preferred short name for model generation. Defaults to None.

        Returns:
            NNNodeRef: A reference to the Node in the Graph
        """
        node_name = name if name else self.unique_name(
            f"constant_{self.num_constants}")
        node = ConstantInputParameters(node_name, dims=dim,
                                       value=value,
                                       adjust_transpose=adjust_transpose,
                                       is_intermediate=is_intermediate,
                                       is_mutated=is_mutated,
                                       short_name=short_name)
        self.add_node(node)
        return NNNodeRef(self, node, 0)

    def add_output(self, name=None) -> OutputParameters:
        """Create an output node. If a name is not supplied then one will be automatically chosen.

        Args:
            name (str, optional): Node name. Defaults to None.

        Returns:
            OutputParameters: Created node
        """
        node_name = self.unique_name(
            f"output_{self.num_outputs + 1}") if name is None else name
        node = OutputParameters(node_name)
        self.add_node(node)
        return node

    def nodes_iterator(self, yield_fusions=True):
        """Yields a tuple of length 4 with the step idx and parameters of each node. Optionally
        when in a fusion yields tuples containing the fusion internal step id and node for each internal
        node.

        Args:
            yield_fusions (bool, optional): Whether to yield fusion nodes. Defaults to True.

        Yields:
            [Tuple[int, Parameters, Optional[int], Optional[Parameters]]]: Tuple containing node_idx, node, fusion_idx, fusion_node
        """
        for step_idx, step in enumerate(self.graph_state.steps):
            node = step['node']
            if yield_fusions:
                if isinstance(node, (FusionBase)) and not isinstance(node, ExpressionFusionParameters):
                    for fusion_idx, fnode in enumerate(node.contained_nodes()):
                        yield (step_idx, node, fusion_idx, fnode)
            yield (step_idx, node, None, None)

    def adjust_order(
        self,
        reshape_weights=True,
        no_postprocess=False,
        debug_function: Callable = None,
        steps: int = None,
        single_step=False
    ):
        """Adjusts tensor order to match selected kernels

        Args:
            reshape_weights (bool, optional): Whether weights should be modified to remove transposes. Defaults to True.
            no_postprocess (bool, optional): Whether post processing such as transpose elimination is run. Defaults to False.
            debug_function (Callable, optional): Function to be called after each transpose elimination step. Defaults to None.
            steps (int, optional): Number of elimination steps to run. Defaults to None.
            single_step (bool, optional): Execute only one transpose elimination step in each cycle. Defaults to False.
        """
        adjust_order(self, reshape_weights=reshape_weights,
                     postprocess=not no_postprocess, debug_function=debug_function,
                     steps=steps, single_step=single_step)
        LOG.info("adjusted order")
        self.graph_identity.is_adjusted = True

    @staticmethod
    def get_fusions():
        """Returns a dictionary of all the fusion/graph optimization pass names and descriptions

        Returns:
            Dict[str, str]: Names and descriptions of graph optimisation passes
        """
        return get_fusions()

    def fusions(self, *match_names, no_postprocess: bool = False):
        """Run matchers on the graph

        Args:
            match_names (str): Names of matches to apply
            no_postprocess (bool, optional): Do not execute postprocessing such as transpose elimination. Defaults to False.
        """
        fusions(self, *match_names, no_postprocess=no_postprocess)

    def add_dimensions(
        self,
        quiet=False
    ):
        """Add dimensions to the graph and calculate execution order and liveness

        Args:
            quiet (bool, optional): Do not log progress. Defaults to False.
        """
        if not quiet:
            LOG.info("update graph dimensions")
        self.graph_state.steps = add_dimensions(self)
        if not quiet:
            LOG.info("calculate liveness")
        self.graph_state.liveness = calculate_liveness(
            self,
            self.graph_state.steps)

    def collect_statistics(
        self,
        input_tensors_iterator: Union[Sequence[Sequence[np.ndarray]], Sequence[np.ndarray]]
    ) -> Mapping[Union[str, Tuple[str, str]], Mapping]:
        """Collect tensor statistics for quantization

        Args:
            input_tensors_iterator (Union[Sequence[Sequence[np.ndarray]], Sequence[np.ndarray]]):
                If the graph has a single input this can just be an iterator over numpy arrays. If the graph has
                multiple inputs then it should be an iterator over sequences of numpy arrays.

        Returns:
            Mapping[Union[str, Tuple[str, str]], Mapping]: Mapping of statistics for each node's inputs and outputs
        """
        stats_collector = ActivationRangesCollector()
        for input_tensors in input_tensors_iterator:
            if isinstance(input_tensors, np.ndarray):
                input_tensors = [input_tensors]
            stats_collector.collect_stats(self, input_tensors)
        return {k.key: v for k, v in stats_collector.stats.items()}

    @staticmethod
    def qsnrs(tensors1, tensors2, idx=0):
        return tuple([qsnr(t1[idx], t2[idx]) if len(t1) > idx and len(t2) > idx else None for t1, t2 in zip(tensors1, tensors2)])

    @staticmethod
    def cos_sim(tensors1, tensors2, idx=0):
        return tuple([cos_similarity(t1[idx], t2[idx]) if len(t1) > idx and len(t2) > idx else None for t1, t2 in zip(tensors1, tensors2)])

    def quantize(
        self,
        statistics: Mapping[Union[str, Tuple[str, str]], Mapping] = None,
        schemes: Sequence[str] = None,
        graph_options: Mapping[str, Any] = None,
        node_options: Mapping[Union[str, Tuple[str, str]],
                              Mapping[str, Any]] = None,
        read_existing_options = True
    ) -> None:
        """Quantize the graph

        Args:
            statistics (Mapping[Union[str, Tuple[str, str]], Mapping], optional): Statistics collected by the NNGraph.collect_statistics
                method.
            schemes (Sequence[], optional): Sequence of schemes "scaled", "pow2", or "float" to use in priority order. If None use scaled. Defaults to None.
            graph_options (Mapping[str, Any], optional): Quantization options to set for the whole graph. Defaults to None.
            node_options (Mapping[Union[str, Tuple[str, str]], Mapping[str, Any]], optional):
                Quantization options to set for specific nodes. The map key should be the node name or if the node is inside a fusion
                then a tuple of the fusion name and the node name. Defaults to None.
            read_existing_options (bool, optional): Incorporate existing quantization options and schemes in the graph. Leaving this as
                True and just supplying graph_option, node_options and/or schemes is the equivalent of the nntool qtune command
        """
        quantizer = NewQuantizer(self)
        if schemes:
            for scheme in schemes:
                scheme = scheme.lower()
                if scheme not in SCHEME_NAME_MAPPINGS:
                    raise ValueError(f'invalid scheme name {scheme}')
                quantizer.schemes.append(SCHEME_NAME_MAPPINGS[scheme])
        elif 'SQ8' not in quantizer.schemes:
            quantizer.schemes.append('SQ8')
        options = {}
        if graph_options:
            options.update(graph_options)
        if node_options:
            options.update({NodeId(name) if isinstance(name, str) else NodeId(*name): v
                            for name, v in node_options.items()})
        quantizer.set_stats(statistics)
        quantizer.update_options(options)
        quantizer.quantize()

    def execute(
            self,
            input_tensors: Union[np.ndarray, Sequence[np.ndarray]],
            quantize=False,
            dequantize=False,
            output_fusion_tensors=False
    ) -> Sequence[Sequence[np.ndarray]]:
        """Runs inference on the graph

        Args:
            input_tensors (Union[np.ndarray, Sequence[np.ndarray]]):
                Numpy arrays containing inputs (which should be normalized and in float)
                If there is only one input it can be specified without a sequence.
            quantize (bool, optional): Run the graph using quantization parameters. Defaults to False.
            dequantize (bool, optional): Dequantize outputs. Implies quantize. Defaults to False.
            output_fusion_tensors (bool, optional): Output outputs from nodes that have been fused. Defaults to False.

        Raises:
            ValueError: Incorrect parameters

        Returns:
            Sequence[Sequence[np.ndarray]]: List of lists of outputs of each node in the graph. If output_fusion_tensors
            is True this will also include the output of nodes contained inside fusions (except fused expressions)
        """
        if dequantize:
            quantize = True
        if quantize:
            if self.quantization is None or not self.quantization.verify_quantization(self):
                raise ValueError('graph is not quantized')
            if dequantize:
                qmode = QuantizationMode.all_dequantize()
            else:
                qmode = QuantizationMode.all()
        else:
            qmode = QuantizationMode.none()
        if isinstance(input_tensors, np.ndarray):
            input_tensors = [input_tensors]
        executer = GraphExecuter(self, self.quantization)
        return executer.execute(input_tensors, qmode=qmode, append_fusion_output=output_fusion_tensors)

    def balance_filters(
        self,
        step_idx: int = None,
        precision_threshold=0.20
    ):
        """Experimental filter balancing routines

        Args:
            step_idx (int, optional): Step to balance. Defaults to None.
            precision_threshold (float, optional): Precision threshold. Defaults to 0.20.

        Raises:
            ValueError: Bad parameters
            NotImplementedError: Bad graph structure
        """
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

    def __getitem__(self, key) -> Parameters:
        if isinstance(key, int):
            return self.nodes_by_step_idx[key]
        return super().__getitem__(key)

    def __repr__(self) -> str:
        tab = GraphReporter().report(self)
        renderer = TextTableRenderer(150)
        tab.render(renderer)
        return renderer.get_output()
