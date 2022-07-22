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

import fcntl
from functools import reduce
import importlib
import json
import logging
import os
import re
import shutil
import subprocess
import tempfile
import zipimport
from io import StringIO
from select import select
from typing import (Any, Callable, Generator, Literal, Mapping, Sequence,
                    Tuple, Union)
from itertools import groupby

import numpy as np
import texttable
from nntool.execution.graph_executer import GraphExecuter
from nntool.execution.quantization_mode import QuantizationMode
from nntool.generation.autotiler_options import DEFAULT_GEN_OPTS
from nntool.generation.code_generator import CodeGenerator
from nntool.generation.default_template import (basic_kernel_header_template,
                                                basic_kernel_source_template,
                                                default_template,
                                                dynamic_template)
from nntool.generation.gen_project.utils import gen_project, make_script
from nntool.generation.naming_convension import DefaultNamingConvension
from nntool.graph.dim import Dim
from nntool.graph.manipulations.adjust_order import adjust_order, needs_adjust
from nntool.graph.manipulations.balance_filter import (
    balance_all_filters, balance_filter_with_constants)
from nntool.graph.manipulations.dimensions import add_dimensions
from nntool.graph.manipulations.dump import dump_graph
from nntool.graph.manipulations.liveness import calculate_liveness
from nntool.graph.matches.fusions import fusions
from nntool.graph.matches.matches import get_fusions
from nntool.graph.types import (ConstantInputNode, InputNNNodeBase, InputNode,
                                MultiplicativeBiasNodeBase, OutputNode,
                                ResizerNodeBase, RNNNodeBase, SSDDetectorNode)
from nntool.graph.types.base import NNEdge, NNNodeBase, NNNodeRef
from nntool.graph.types.dsp_preprocessing import (DSPNodeBase,
                                                  MFCCPreprocessingNode,
                                                  RFFT2DPreprocessingNode)
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import (ActivationFusionNodeBase,
                                        FilterFusionNodeBase, FusionNodeBase,
                                        MatMulOpFusionNode,
                                        PaddedAddFusionNode)
from nntool.graph.types.tensor_shape import ReshapeNode
from nntool.importer.importer import create_graph
from nntool.interpreter.commands.qtune import SCHEME_NAME_MAPPINGS
from nntool.interpreter.settings import DEFAULT_NNTOOL_SETTINGS
from nntool.quantization.new_qrec import QRec
from nntool.quantization.quantization_set import QuantizationSet
from nntool.quantization.quantizer.new_quantizer import NewQuantizer
from nntool.reports.draw_graph_reporter import DrawGraphReporter
from nntool.reports.graph_reporter import GraphReporter
from nntool.reports.quantization_reporter import QuantizationReporter
from nntool.stats.activation_ranges_collector import ActivationRangesCollector
from nntool.utils.at_tensor_loader import at_map_tensors, at_tensor_loader_int
from nntool.utils.graph import Graph, Node
from nntool.utils.json_serializable import JsonSerializableStateDecoder
from nntool.utils.stats_funcs import cos_similarity, qsnr
from nntool.utils.tabular import TextTableRenderer

LOG = logging.getLogger(__name__)


def non_block_read(output):
    file_descr = output.fileno()
    retval = fcntl.fcntl(file_descr, fcntl.F_GETFL)
    fcntl.fcntl(file_descr, fcntl.F_SETFL, retval | os.O_NONBLOCK)
    try:
        return output.read()
    except:
        return ""


class TexttableEx(texttable.Texttable):
    @classmethod
    def _fmt_int(cls, x, **kw):
        return f'{x:,}'


class CompletedProcess(object):
    def __init__(self, args, returncode, stdout=None, stderr=None, output_tensors=None, performance=None, at_log=None):
        self.args = args
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr
        self.output_tensors = output_tensors
        self.performance = performance
        self.at_log = at_log

    def __repr__(self):
        args = ['args={!r}'.format(self.args),
                'returncode={!r}'.format(self.returncode)]
        if self.stdout is not None:
            args.append('stdout={!r}'.format(self.stdout))
        if self.stderr is not None:
            args.append('stderr={!r}'.format(self.stderr))
        return "{}({})".format(type(self).__name__, ', '.join(args))


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
                 filename=None,
                 build_settings=None):

        attrs = {
            'model': model,
            'node_options': {},

            'quantization': None,
            'has_quantized_parameters': False,
            'graphname': name,
            'graph_state': NNGraphState(),
            'filename': filename,
            'build_settings': build_settings
        }
        super().__init__(**attrs)

    @staticmethod
    def load_graph(filepath_or_model: Union[str, Any],
                   load_quantization=False,
                   remove_quantize_ops=True,
                   use_hard_sigmoid=False,
                   use_hard_tanh=False,
                   fold_batchnorm=True,
                   rescale_perchannel=True,
                   **opts) -> 'NNGraph':
        """ Load a graph into NNTool returning an NNGraph instance.

        Args:
            filepath_or_model (Union[str, Any]):
                Absolute path to file or ONNX ModelProto
            load_quantization (bool, optional):
                Load quantization information in graph. In TFLite graphs this
                loads the tensor quantization information. In ONNX graphs exported from the NNCF compression
                and quantization framework this enables interpretation of the NNCF LinearQuantize operators.
                Defaults to False.
            remove_quantize_ops (bool, optional):
                Remove quantize operators from graph. Defaults to True.
            use_hard_sigmoid (bool, optional):
                Force use of hard sigmoids for sigmoid operations. Defaults to False.
            use_hard_tanh (bool, optional):
                Force use of hard tanh for tanh operations. Defaults to False.
            fold_batchnorm (bool, optional):
                Fold batchnorm during import. Defaults to True.
            rescale_perchannel (bool, optional):
                Rescale all filter tensors to per channel quantized
                weights when importing TFLITE quantized graphs. Defaults to True.

        Returns:
            NNGraph: Loaded graph
        """
        if isinstance(filepath_or_model, str) and filepath_or_model.endswith('.json'):
            with open(filepath_or_model) as fp:
                history = json.load(fp, cls=JsonSerializableStateDecoder)
            nntool = importlib.import_module(
                'nntool.interpreter.nntool_shell').NNToolShell()
            nntool.graph_history = history
            nntool.replay_history()
            return nntool.G

        opts.update({
            'load_quantization': load_quantization,
            'remove_quantize_ops': remove_quantize_ops,
            'use_hard_sigmoid': use_hard_sigmoid,
            'use_hard_tanh': use_hard_tanh,
            'fold_batchnorm': fold_batchnorm,
            'rescale_perchannel': rescale_perchannel
        })
        return create_graph(filepath_or_model, opts=opts)

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
                os.path.basename(self._attr['filename']))
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
    def settings(self) -> dict:
        """NNTool shell settings relating to autotiler model creation

        Returns:
            dict: Map of settings
        """
        settings = self._attr.get('build_settings', {})
        if not settings:
            return settings

        def settings_filter(item):
            k, v = item
            k = k.lower()
            if k in DEFAULT_NNTOOL_SETTINGS:
                return v != DEFAULT_NNTOOL_SETTINGS[k]
            if k in DEFAULT_GEN_OPTS:
                return v != DEFAULT_GEN_OPTS[k]
            return False
        return dict(filter(settings_filter, settings.items()))

    @settings.setter
    def settings(self, val: str):
        """NNTool shell settings relating to autotiler model creation

        Args:
            val (str): Tsettings to point to
        """
        self._attr['build_settings'] = val

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
        return len(self.nodes(node_classes=InputNode))

    @property
    def num_outputs(self) -> int:
        """Current number of outputs

        Returns:
            int: Number of outputs
        """
        return len(self.nodes(node_classes=OutputNode))

    @property
    def num_constants(self) -> int:
        """Current number of constant inputs

        Returns:
            int: Number of constant inputs
        """
        return len(self.nodes(node_classes=ConstantInputNode))

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
    def filename(self) -> str:
        """Filename that graph was loaded from (if any)

        Returns:
            str: Filename
        """
        return self._attr['filename']

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
        return self.has_node_type(SSDDetectorNode)

    @property
    def has_resizer(self) -> bool:
        """Graph has resizer nodes

        Returns:
            bool: True if present
        """
        return self.has_node_type(ResizerNodeBase)

    @property
    def has_expressions(self) -> bool:
        """Graph has compiled expressions

        Returns:
            bool: True if present
        """
        if self.has_node_type(ExpressionFusionNode):
            return True
        if any(node.subgraph.nodes(node_classes=ExpressionFusionNode)
               for node in self.nodes(node_classes=FusionNodeBase) if node.quantize_internals):
            return True
        return False

    @property
    def has_dsp(self) -> bool:
        """Graph has DSP nodes

        Returns:
            bool: True if present
        """
        return self.has_node_type(DSPNodeBase)

    @property
    def all_expressions(self) -> Sequence[ExpressionFusionNode]:
        """All the expression nodes in the graph

        Returns:
            Sequence[ExpressionFusionParameters]: List of nodes
        """
        return self.nodes(node_classes=ExpressionFusionNode)

    @property
    def all_constants(self) -> Sequence[ConstantInputNode]:
        """All the constant nodes in the graph

        Returns:
            Sequence[ConstantInputParameters]: List of nodes
        """
        return self.nodes(node_classes=ConstantInputNode)

    @property
    def nodes_by_step_idx(self) -> Sequence[NNNodeBase]:
        """All the nodes in the graph ordered by execution order

        Returns:
            Sequence[Parameters]: List of nodes
        """
        return [step['node'] for step in self.graph_state.steps]

    @property
    def nodes_by_step_idx_with_fusions(self) -> Sequence[NNNodeBase]:
        """Nodes ordered by execution order but also including internal nodes
        for fusions

        Returns:
            Sequence[Parameters]: List of nodes
        """
        nodes = []
        for step in self.graph_state.steps:
            node = step['node']
            if isinstance(node, (FilterFusionNodeBase, ActivationFusionNodeBase,
                                 PaddedAddFusionNode, MatMulOpFusionNode)):
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
        nodes = self.nodes(node_classes=RNNNodeBase)
        if not nodes:
            return False
        if ktype is not None and not any(self.quantization[node.name].ktype == ktype for node in nodes):
            return False
        if ne16 and not any(self.quantization[node.name].cache.get('ne16') for node in nodes):
            return False
        return True

    def has_node_type(self, node_type: NNNodeBase) -> bool:
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
        return sorted(self.nodes(node_classes=InputNNNodeBase), key=lambda x: x.index, reverse=False)

    def input_nodes(self) -> Generator[Node, None, None]:
        """Iterate over all inputs

        Returns:
            Generator[Node]: a generator for all nodes
        """
        return sorted(self.nodes(node_classes=InputNode), key=lambda x: x.index, reverse=False)

    def output_nodes(self) -> Generator[Node, None, None]:
        """Iterate over all outputs

        Returns:
            Generator[Node]: a generator for all nodes
        """
        return sorted(self.nodes(node_classes=OutputNode), key=lambda x: x.index, reverse=False)

    def nodes(self, node_classes: Node=None, sort=False) -> Sequence[Node]:
        """Select nodes in the graph filtering by node class.

        Args:
            node_classes (Node, optional): Node classes to filter. Defaults to None.
            sort (bool, optional): Sort nodes by step idx. Defaults to False.

        Returns:
            Sequence[Node]: List of nodes.
        """
        return super().nodes(node_classes=node_classes, sort=sort)

    def add_input(self, dim: Union[Dim, Tuple[int]], name: str = None, **kwargs) -> NNNodeRef:
        """Create an input node. If a name is not supplied then one will be automatically chosen.

        Args:
            dim (Union[Dim, Tuple[int]]): Input dimension
            name (str, optional): Node name. Defaults to None.

        Returns:
            NNNodeRef: Reference to created node in graph
        """
        idx = self.num_inputs + 1
        node_name = self.unique_name(
            f"input_{idx}") if not name else name
        node = InputNode(node_name, dims=dim, index=idx-1, **kwargs)
        self.add_node(node)
        return NNNodeRef(self, node, 0)

    def add_constant(self, dim: Union[Dim, Tuple[int]] = None,
                     name: str = None,
                     value: np.ndarray = None,
                     is_mutated=False,
                     is_intermediate=False,
                     short_name: str = None) -> NNNodeRef:
        """Creates a constant node

        Args:
            dim (Union[Dim, Tuple[int]], optional):
                Dimension of constant if not supplied then a value must be. Defaults to None.
            name (str, optional):
                Optional name. A unique one will be created if None. Defaults to None.
            value (np.ndarray, optional):
                Numpy array with value. Defaults to None.
            is_mutated (bool, optional):
                Constant is both an input and an output. Defaults to False.
            is_intermediate (bool, optional):
                Constant is marked as intermediate at import. Defaults to False.
            short_name (str, optional):
                Preferred short name for model generation. Defaults to None.

        Returns:
            NNNodeRef: A reference to the Node in the Graph
        """
        node_name = name if name else self.unique_name(
            f"constant_{self.num_constants}")
        node = ConstantInputNode(node_name, dims=dim,
                                 value=value,
                                 is_intermediate=is_intermediate,
                                 is_mutated=is_mutated,
                                 short_name=short_name)
        self.add_node(node)
        return NNNodeRef(self, node, 0)

    def add_output(self, name=None, not_used=False) -> OutputNode:
        """Create an output node. If a name is not supplied then one will be automatically chosen.

        Args:
            name (str, optional): Node name. Defaults to None.

        Returns:
            OutputParameters: Created node
        """
        idx = self.num_outputs + 1
        node_name = self.unique_name(
            f"output_{idx}") if name is None else name
        node = OutputNode(node_name, index=idx-1, not_used=not_used)
        self.add_node(node)
        return node

    def nodes_iterator(self, yield_fusions=True):
        """Yields a tuple of length 4 with the step idx and parameters of each node. Optionally
        when in a fusion yields tuples containing the fusion internal step id and node for each internal
        node.

        Args:
            yield_fusions (bool, optional):
                Whether to yield fusion nodes. Defaults to True.

        Yields:
            [Tuple[int, Parameters, Optional[int], Optional[Parameters]]]: Tuple containing node_idx,
                node, fusion_idx, fusion_node
        """
        for step_idx, step in enumerate(self.graph_state.steps):
            node = step['node']
            if yield_fusions:
                if isinstance(node, (FusionNodeBase)) and not isinstance(node, ExpressionFusionNode):
                    for fusion_idx, fnode in enumerate(node.contained_nodes()):
                        yield (step_idx, node, fusion_idx, fnode)
            yield (step_idx, node, None, None)

    def get_node_option(self, name: str, option_name: str) -> Any:
        """ Shortcut to get an option on a node

        Args:
            name (str): Name of the node to get
            option_name (str): Node option name

        Raises:
            KeyError: Node not found
            ValueError: Option not valid for node

        Returns:
            Any: Value of option
        """
        if name not in self:
            raise KeyError(f'node {name} not found in graph')
        node = self[name]

        node_options = node.at_options
        if option_name not in node_options.valid_options:
            raise ValueError(
                f"{option_name} is not a valid option for node {name}")
        return getattr(node_options, option_name)

    def set_node_option(self, name: str, option_name: str, value: Any = None):
        """ Shortcut to set nodeoptions. Equivalent to the node option command.

        Args:
            name (str): Name of the node to set
            option_name (str): Name of the node option
            value (Any, optional): The value of the option. If None the option will be cleared. Defaults to None.

        Raises:
            KeyError: Node name is not found
            ValueError: Option is not valid for the node
            ValueError: Value is not of correct type
        """
        if name not in self:
            raise KeyError(f'node {name} not found in graph')
        node = self[name]

        node_options = node.at_options
        if option_name not in node_options.valid_options:
            raise ValueError(
                f"{option_name} is not a valid option for node {name}")
        option_type = node_options.valid_options[option_name]
        if value is not None and not isinstance(value, option_type):
            raise ValueError(
                f"{option_name} expects an argument of type {option_type.__name__}")
        if option_name in ["RNN_STATES_AS_INPUTS", "LSTM_OUTPUT_C_STATE"] and value:
            if option_name == "RNN_STATES_AS_INPUTS":
                node.set_states_as_inputs(self)
            elif option_name in ["LSTM_OUTPUT_C_STATE", "OUTPUT_C_STATE"]:
                node.set_c_state_as_output(self)
        else:
            setattr(node_options, option_name, value)
        self.node_options[node.name] = node_options

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
            reshape_weights (bool, optional):
                Whether weights should be modified to remove transposes. Defaults to True.
            no_postprocess (bool, optional):
                Whether post processing such as transpose elimination is run. Defaults to False.
            debug_function (Callable, optional):
                Function to be called after each transpose elimination step. Defaults to None.
            steps (int, optional):
                Number of elimination steps to run. Defaults to None.
            single_step (bool, optional):
                Execute only one transpose elimination step in each cycle. Defaults to False.
        """
        adjust_order(self, reshape_weights=reshape_weights,
                     postprocess=not no_postprocess, debug_function=debug_function,
                     steps=steps, single_step=single_step)
        LOG.info("adjusted order")

    def needs_adjust(self):
        """ Checks if all nodes' tensor order matches an available kernel

        Returns:
            bool: Graph requires adjust order to be run
        """
        return needs_adjust(self)

    @staticmethod
    def get_fusions():
        """Returns a dictionary of all the fusion/graph optimization pass names and descriptions

        Returns:
            Dict[str, str]: Names and descriptions of graph optimisation passes
        """
        return get_fusions()

    def fusions(
        self,
        *match_names,
        no_postprocess: bool = False,
        exclude_match_names: Sequence[str] = None):
        """Run matchers on the graph

        The scale8 match group has the name scaled_match_group
        The pow2 match group has the name pow2_match_group
        get_fusions() can be used to retrieve a dictionary of available fusions

        Args:
            match_names (str):
                Names of matches to apply
            no_postprocess (bool, optional):
                Do not execute postprocessing such as transpose elimination. Defaults to False.
            exclude_match_names (Sequence[str], optional):
                Do no run these match names if in a match group or added by other matches
        """
        fusions(self, *match_names, no_postprocess=no_postprocess, exclude_match_names=exclude_match_names)

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
            LOG.debug("calculate liveness")
        self.graph_state.liveness = calculate_liveness(
            self,
            self.graph_state.steps)

    def insert_dsp_preprocessing(
        self,
        input_node: Node,
        dsp_node_type: str,
        dsp_node_name: str = "dsp_node",
        config_dict: Mapping[str, Any] = None,
    ):
        """Inserts a DSP preprocessing node before one of the input_node provided

        Args:
            input_node (NNNodeBase):
                Graph Node before which nntool will insert the DSP preprocessing
            dsp_node_type (str):
                Type of DSP operation ("mfcc" or "fft")
            dsp_node_name (str):
                Name of the node insrted (default: "dsp_node")
            config_dict (Mapping[str, Any]):
                Dictionary with configuration for LUT generation, i.e.::

                    config = {
                        "window": "hanning",        # Window numpy function
                        "pad_type": "center",       # How to apply padding if frame_size < n_fft
                        "n_fft": 512,               # Number of FFT bins
                        "n_frames": 401,            # Number of frames (it must match the graph input size)
                        "frame_size": 512,          # Number of samples in each frame
                        "window_size": 400,         # Number of samples for the window function (if < frame_size the window will be center padded)
                        "frame_step": 160,          # Number of samples between two consecutive frames
                        "sample_rate": 16000,       # Samplerate
                        "magsquared": True,         # If True, the spectrogram is calculated as np.abs(rfft) ** 2, else np.abs(rfft)
                        "n_fbanks": 128,            # Number of Mel Filterbanks (ignored when dsp_node_type=="fft")
                        "fbank_type": "librosa",    # Algorithm used to generate the Mel Filters
                        "mel_filterbanks_file": None, # Path to melfilters .npy array provided by user, in this case fbank_type will be ignored
                        "n_dct": 0,                 # Number of DCT bins in case of Mfcc preprocessing
                        "dct_type": 2,              # DCT algorithm type
                        "fmin": 0.0,                # Fmin used to calculate the Mel Filters
                        "fmax": None,               # Fmax used to calculate the Mel Filters
                        "log_type": "natural",       # Logarithm type ("None", "db" or "natural"), None will output melspectrogram
                        "log_offset": 1e-6          # Offset applied before log function
                    }

        """
        assert "frame_step" in config_dict, "Provide frame_step in the config dictionary"
        assert "frame_size" in config_dict, "Provide frame_size in the config dictionary"
        new_input_size = config_dict["frame_step"] * \
            (config_dict["n_frames"] - 1) + config_dict["frame_size"]
        if dsp_node_type == "mfcc":
            dsp_node = MFCCPreprocessingNode(dsp_node_name, **config_dict)
            win_lut, fft_twiddles, swaptable, rfft_twiddles = dsp_node.gen_fft_twiddles()
            melfilt_coeff_sparse_node, melfilt_sparsity_node = dsp_node.gen_melfilter()
            dsp_node = dsp_node(None, win_lut(self) if win_lut else win_lut, fft_twiddles(self), swaptable(
                self), rfft_twiddles(self), melfilt_sparsity_node(self), melfilt_coeff_sparse_node(self))
        elif dsp_node_type == "fft":
            dsp_node = RFFT2DPreprocessingNode(dsp_node_name, **config_dict)
            win_lut, fft_twiddles, swaptable, rfft_twiddles = dsp_node.gen_fft_twiddles()
            dsp_node = dsp_node(None, win_lut(self) if win_lut else win_lut, fft_twiddles(
                self), swaptable(self), rfft_twiddles(self))
        else:
            raise ValueError(
                f"{dsp_node_type} dsp_type not supported: only 'mfcc' or 'fft'")
        dsp_out_dims = dsp_node.get_output_size(
            input_node.get_output_size(None))[0]

        input_node_edge = self.out_edges(input_node)[0]
        nn_in_dims = input_node_edge.to_node.in_dims[0]
        #new_input_node = InputNode(input_node.name, dims=(new_input_size, ))
        self.replace_node(input_node, dsp_node)
        new_input = self.add_input((new_input_size, ), input_node.name)
        self.add_edge(NNEdge(new_input.node, dsp_node, to_idx=0))

        if len(dsp_out_dims) < len(nn_in_dims):
            if np.prod(dsp_out_dims.shape) != np.prod(nn_in_dims.shape):
                raise ValueError(
                    f"cannot attach DSP preprocessing with {dsp_out_dims.shape} output shape to NN with input shape of {nn_in_dims.shape}")
            reshape_node = ReshapeNode(
                dsp_node_name + "_out_reshape", old_shape=dsp_out_dims.shape, shape=nn_in_dims.shape)
            self.insert_node_after(dsp_node, reshape_node, edge_class=NNEdge)
        # self.replace_node(input_node, new_input_node)
        self.add_dimensions()

    def collect_statistics(
        self,
        input_tensors_iterator: Union[Sequence[Sequence[np.ndarray]],
                                      Sequence[np.ndarray]]
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
        return stats_collector.stats

    @staticmethod
    def qsnrs(
            tensors1: Sequence[Sequence[np.ndarray]],
            tensors2: Sequence[Sequence[np.ndarray]],
            idx: int = 0) -> Sequence[float]:
        """Convenience method to calculate the QSNR between two sets of output tensors from the execute method

        Args:
            tensors1 (Sequence[Sequence[np.ndarray]]): Output from execute
            tensors2 (Sequence[Sequence[np.ndarray]]): Output from execute
            idx (int, optional): Output index. Defaults to 0.

        Returns:
            Sequence[float]: QSNRs
        """
        if idx is None:
            return tuple([tuple([qsnr(t1__, t2__)
                         if (t1__ is not None and t2__ is not None) else None
                         for t1__, t2__ in zip(t1_, t2_)])
                         if (t1_ is not None and t2_ is not None) else (None)
                         for t1_, t2_ in zip(tensors1, tensors2)])
        return tuple([qsnr(t1[idx], t2[idx])
                      if (len(t1) > idx and len(t2) > idx and
                          t1[idx] is not None and t2[idx] is not None) else None
                      for t1, t2 in zip(tensors1, tensors2)])

    @staticmethod
    def cos_sim(
            tensors1: Sequence[Sequence[np.ndarray]],
            tensors2: Sequence[Sequence[np.ndarray]],
            idx: int = 0) -> Sequence[float]:
        """Convenience method to calculate the Cosine similarity between two sets of output tensors from
            the execute method

        Args:
            tensors1 (Sequence[Sequence[np.ndarray]]): Output from execute
            tensors2 (Sequence[Sequence[np.ndarray]]): Output from execute
            idx (int, optional): Output index. Defaults to 0.

        Returns:
            Sequence[float]: Cosine similarity
        """
        if idx is None:
            return tuple([tuple([cos_similarity(t1__, t2__)
                         if (t1__ is not None and t2__ is not None) else None
                         for t1__, t2__ in zip(t1_, t2_)])
                         if (t1_ is not None and t2_ is not None) else (None)
                         for t1_, t2_ in zip(tensors1, tensors2)])
        return tuple([cos_similarity(t1[idx], t2[idx])
                      if (len(t1) > idx and len(t2) > idx and
                          t1[idx] is not None and t2[idx] is not None) else None
                      for t1, t2 in zip(tensors1, tensors2)])

    def quantize(
        self,
        statistics: Mapping[Union[str, Tuple[str, str]], Mapping] = None,
        schemes: Sequence[str] = None,
        graph_options: Mapping[str, Any] = None,
        node_options: Mapping[Union[str, Tuple[str, str]],
                              Mapping[str, Any]] = None,
        read_existing_options=True,
        no_postprocess=False
    ) -> None:
        """Quantize the graph

        Args:
            statistics (Mapping[Union[str, Tuple[str, str]], Mapping], optional):
                Statistics collected by the NNGraph.collect_statistics method.
            schemes (Sequence[], optional):
                Sequence of schemes "scaled", "pow2", or "float" to use in priority order.
                If None use scaled. Defaults to None.
            graph_options (Mapping[str, Any], optional):
                Quantization options to set for the whole graph.
                Defaults to None.
            node_options (Mapping[Union[str, Tuple[str, str]], Mapping[str, Any]], optional):
                Quantization options to set for specific nodes. The map key should be the node name or if the
                node is inside a fusion then a tuple of the fusion name and the node name. Defaults to None.
            read_existing_options (bool, optional):
                Incorporate existing quantization options and schemes in the graph (if quantization is present).
                Leaving this as True and just supplying graph_option, node_options and/or schemes is the
                equivalent of the nntool qtune command
            no_postprocess (bool, optional):
                If True skip adjust after quantize
        """
        if self.quantization and read_existing_options:
            quantizer = NewQuantizer.from_quantized_graph(self)
        else:
            quantizer = NewQuantizer(self)
        if schemes:
            if graph_options is not None and graph_options.get("scheme"):
                schemes.append(graph_options["scheme"])
            for scheme in schemes:
                scheme = scheme.lower()
                if scheme not in SCHEME_NAME_MAPPINGS:
                    raise ValueError(f'invalid scheme name {scheme}')
                quantizer.schemes.append(SCHEME_NAME_MAPPINGS[scheme])
        elif graph_options is not None and graph_options.get("scheme"):
            quantizer.schemes.append(graph_options["scheme"])
        elif 'SQ8' not in quantizer.schemes:
            quantizer.schemes.append('SQ8')

        if graph_options is None:
            graph_options = {}
        if node_options is None:
            node_options = {}
        quantizer.update_options(graph_options, node_options)
        if statistics is not None:
            quantizer.set_stats(statistics)
        quantizer.quantize(no_postprocess=no_postprocess)

    def execute(
            self,
            input_tensors: Union[np.ndarray, Sequence[np.ndarray]],
            quantize=False,
            dequantize=False,
            output_fusion_tensors=False,
            check_quantization=True
    ) -> Sequence[Sequence[np.ndarray]]:
        """Runs inference on the graph

        Args:
            input_tensors (Union[np.ndarray, Sequence[np.ndarray]]):
                Numpy arrays containing inputs (which should be normalized and in float)
                If there is only one input it can be specified without a sequence.
            quantize (bool, optional):
                Run the graph using quantization parameters. Defaults to False.
            dequantize (bool, optional):
                Dequantize outputs. Implies quantize. Defaults to False.
            output_fusion_tensors (bool, optional):
                Output outputs from nodes that have been fused. Defaults to False.
            check_quantization (bool, optional):
                Run a check that quantization is consistent before executing. Defaults to True.

        Raises:
            ValueError: Incorrect parameters

        Returns:
            Sequence[Sequence[np.ndarray]]:
                List of lists of outputs of each node in the graph. If output_fusion_tensors
                is True this will also include the output of nodes contained inside fusions
                (except fused expressions)
        """
        if dequantize:
            quantize = True
        if quantize:
            if check_quantization and (self.quantization is None or not self.quantization.verify_quantization(self)):
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
            step_idx (int, optional):
                Step to balance. Defaults to None.
            precision_threshold (float, optional):
                Precision threshold. Defaults to 0.20.

        Raises:
            ValueError: Bad parameters
            NotImplementedError: Bad graph structure
        """
        if step_idx is not None:
            if step_idx > len(self.graph_state.steps) or step_idx < 0:
                raise ValueError("step idx out of range")
            pnode = self.graph_state.steps[step_idx]['node']
            if isinstance(pnode, FilterFusionNodeBase):
                fnode = pnode.contained_filters()
                if len(fnode) > 1:
                    raise NotImplementedError(
                        "fusions with more than one contained filter is not supported")
                fnode = fnode[0]
                node = fnode
            else:
                node = pnode
                fnode = None
            if not isinstance(node, MultiplicativeBiasNodeBase):
                raise ValueError(
                    "weights can only be balanced on nodes that support multiplicative bias")
            balance_filter_with_constants(
                self, pnode, fnode=fnode, precision_threshold=precision_threshold)
        else:
            balance_all_filters(self, precision_threshold=precision_threshold)

    def generate(self,
                 anonymise=False,
                 gen_model=True,
                 gen_expressions=True,
                 template_file="",
                 write_constants=False,
                 settings={}):
        res = {}
        code_gen = CodeGenerator(
            self, DefaultNamingConvension(anonymise=anonymise), settings)
        if template_file:
            code_template = dynamic_template(template_file)
        else:
            code_template = default_template
        if gen_model:
            res['model'] = code_template(self, code_generator=code_gen)
        if gen_expressions and self.has_expressions:
            res['basic_kernels'] = basic_kernel_source_template(
                self, code_generator=code_gen)
            res['basic_kernel_headers'] = basic_kernel_header_template(
                self, code_generator=code_gen)
        if write_constants:
            model_dir = settings.get("model_directory", ".")
            tensor_dir = settings.get("tensor_directory", ".")
            os.makedirs(os.path.abspath(model_dir), mode=0o750, exist_ok=True)
            os.makedirs(os.path.abspath(tensor_dir), mode=0o750, exist_ok=True)
            code_gen.write_constants()

        return res

    def gen_project(self,
                    input_tensors: Sequence[np.ndarray] = None,
                    directory: str = None,
                    settings: Mapping[str, Any] = None,
                    script: Sequence[str] = None,
                    output_tensors=False):
        """Builds a GAP template project in a directory.

        Args:
            input_tensors (Sequence[np.ndarray], optional):
                Optional sequence of tensors to pass to function. Defaults to None.
            directory (str, optional):
                Directory to generate project in. Defaults to None.
            settings (Mapping[str, Any], optional):
                Generator settings. Defaults to None. If none is inherited from nntool.graph.settings.
            script (Sequence[str], optional):
                Optional nntool script. If not provided AutoTiler model will be directly generated.
                Defaults to None.
            output_tensors (bool, optional):
                Return output tensors in CompletedProcess instance. Defaults to False.

        Raises:
            ValueError: SDK not sourced

        """
        if "GAP_SDK_HOME" not in os.environ or "NNTOOL_PATH" not in os.environ:
            raise ValueError(
                'you must run "source sourceme.sh" in the GAP SDK in your environment before using this API')

        if settings is None:
            settings = {}

        if script is None:
            script = []

        with tempfile.TemporaryDirectory() as tempdir:
            if directory is None:
                directory = tempdir
            gen_project(self,
                        settings,
                        directory,
                        script,
                        performance=True,
                        gen_atproject=not script,
                        dump_tensors=output_tensors,
                        input_tensors=input_tensors,
                        overwrite=True)

    def execute_on_target(self,
                          input_tensors: Sequence[np.ndarray] = None,
                          directory: str = None,
                          settings: Mapping[str, Any] = None,
                          script: Sequence[str] = None,
                          output_tensors=False,
                          performance=True,
                          pretty=False,
                          dont_run=False,
                          at_log=False,
                          jobs: int = None,
                          profile=False,
                          at_loglevel: int = 0,
                          print_output=False,
                          pmsis_os='freertos',
                          source='gap9_v2',
                          platform='gvsoc',
                          do_clean=True) -> CompletedProcess:
        """Execute the model using SDK. Builds a project in a directory and executes it.

        Args:
            input_tensors (Sequence[np.ndarray], optional):
                Optional sequence of tensors to pass to function. Defaults to None.
            directory (str, optional):
                Directory to generate project in. Defaults to None.
            settings (Mapping[str, Any], optional):
                Generator settings. Defaults to None. If None is inheritted from nntool.graph.settings.
            script (Sequence[str], optional):
                Optional nntool script. If not provided AutoTiler model will be directly generated.
                Defaults to None.
            output_tensors (bool, optional):
                Return output tensors in CompletedProcess instance. Defaults to False.
            performance (bool, optional):
                Return layer performance information table as list in CompletedProcess instance.
                Defaults to True.
            at_log (bool, optional):
                Return extracted autotiler log in CompletedProcess instance. Defaults to False.
            pretty (bool, optional):
                Return perormance table in CompletedProcess instance formatted nicely as string
                rather than a list. Defaults to False.
            dont_run (bool, optional):
                Only build the project. Defaults to False.
            jobs (int, optional):
                Number of CPUs to use for build. Defaults to None. If None the amount will
                be determined using a system call.
            at_loglevel (int, optional):
                AutoTiler log level. Defaults to 0.
            pmsis_os (str, optional):
                OS to use for PMSIS. Usually pulpos or freertos. Defaults to 'freertos'.
            platform (str, optional):
                platform to run on gvsoc, board, etc. Defaults to 'gvsoc'.
            source (str, optional):
                Target to source i.e. one of the scripts from GAP_SDK/configs. For the
                GAP9 evk use gap9_evk_audio. Defaults to 'gap9_v2'.
            do_clean (bool, optional):
                Do not clean the project when run a second time. Default: False

        Raises:
            ValueError: SDK not sourced

        Returns:
            CompletedProcess: Information on process that ran
        """
        if "GAP_SDK_HOME" not in os.environ or "NNTOOL_PATH" not in os.environ:
            raise ValueError(
                'you must run "source sourceme.sh" in the GAP SDK in your environment before using this API')

        if settings is None:
            settings = self.settings or {}

        if script is None:
            script = []

        if pretty:
            performance = True

        with tempfile.TemporaryDirectory() as tempdir:
            if directory is None:
                directory = tempdir
            gen_project(self,
                        settings,
                        directory,
                        script,
                        performance=True,
                        gen_atproject=not script,
                        dump_tensors=output_tensors,
                        input_tensors=input_tensors,
                        overwrite=True)
            if jobs is None:
                jobs = int(subprocess.getoutput('nproc --all'))

            with open(os.path.join(directory, 'run_make.sh'), 'w') as fp:
                fp.write(make_script(directory, jobs,
                                     at_loglevel=at_loglevel,
                                     platform=platform,
                                     pmsis_os=pmsis_os,
                                     dont_run=dont_run,
                                     profile=profile,
                                     target=source,
                                     do_clean=do_clean))
            bash = shutil.which('bash')
            if not bash:
                raise ValueError(
                    "Unable to find bash shell. Bash must be in your path.")
            script = shutil.which('script')
            if script:
                logfile = 'log.txt'
                proc_args = [script, '-e', '-c',
                             f"{bash} run_make.sh", os.path.join(tempdir, logfile)]
            else:
                logfile = None
                proc_args = [bash, os.path.join(directory, 'run_make.sh')]
            process = subprocess.Popen(proc_args,
                                       text=True,
                                       stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE,
                                       cwd=directory)
            output = []
            error = []
            while True:
                retcode = process.poll()
                if process.stdout and process.stderr:
                    readable, _, _ = select(
                        [process.stdout, process.stderr], [], [], 500)
                    for stream, buffer in [(process.stdout, output), (process.stderr, error)]:
                        if stream in readable:
                            lines = non_block_read(stream)
                            if lines:
                                if print_output:
                                    print(lines)
                                buffer.append(lines)
                if retcode is not None:
                    break
            output = "".join(output)
            error = "".join(error)
            if logfile:
                with open(os.path.join(tempdir, logfile)) as filep:
                    log_output = "".join(filep.readlines())
            else:
                log_output = output

            parsed_output_tensors = None
            if output_tensors:
                with StringIO(log_output) as fp:
                    parsed_output_tensors = at_map_tensors(
                        self, at_tensor_loader_int(fp))
            if at_log:
                at_log_start = None
                try:
                    at_log_start = log_output.index(
                        "RUNNING AUTOTILER MODEL - START") + len("RUNNING AUTOTILER MODEL - START")
                    at_log_end = log_output.index(
                        "RUNNING AUTOTILER MODEL - FINISH")
                    at_log = log_output[at_log_start:at_log_end].split('\n')
                except ValueError:
                    if at_log_start is None:
                        at_log = None
                    else:
                        at_log = log_output[at_log_start:].split('\n')
            if performance:
                match_perf = r" +((?:S\d+)[^:]+): *Cycles: +(\d+)[^:]+: +(\d+.\d+)[^:]+: +(\d+)[^:]+: +(\d+.\d+)[^:]+: +([\d<.]+)"
                matcher = re.compile(match_perf)
                performance = matcher.findall(log_output)
                if not performance:
                    performance = None
                else:
                    performance = [[n, int(c), int(o), float(p)]
                                   for n, c, _, o, _, p in performance]
                    total_cycles = sum(perf[1] for perf in performance)
                    total_ops = sum(perf[2] for perf in performance)
                    performance = [perf + [perf[1] * 100/total_cycles]
                                   for perf in performance]
                    performance.append(
                        ['Total', total_cycles, total_ops, total_ops/total_cycles, 100])
                    if pretty:
                        table = TexttableEx()
                        table.header(['Layer', 'Cycles', 'Ops',
                                      'Ops/Cycle', '% cycles'])
                        table.set_header_align(['l', 'c', 'c', 'c', 'c'])
                        table.set_cols_dtype(['t', 'i', 'i', 'f', 'f'])
                        table.set_cols_align(['l', 'r', 'r', 'r', 'r'])
                        table.set_max_width(0)
                        for row in performance:
                            table.add_row(row)
                        performance = table.draw()

            return CompletedProcess(proc_args, retcode,
                                    stdout=output,
                                    stderr=error,
                                    output_tensors=parsed_output_tensors,
                                    performance=performance,
                                    at_log=at_log)

    def remove_nodes(self, node_from, node_to=None, up=True, leave=True, no_check=False):
        """ Removes all the edges and nodes between node_from and node_to. Will only work if nodes do not affect shape of tensor.
            If node_to is None, this will remove all nodes following/preceding (up argument) node_from and the node specified (leave argument).
            New output nodes will be created on all the inputs to the node specified. Specifying up does the remove upwards and creates inputs.

        Args:
            node_from (Node, optional):
                Node that remove operation starts from.
            node_to (str, optional):
                Node that remove operation ends to. Default to None: i.e. remove everything above/under node_from.
            up (bool, optional):
                When node_to=None, remove it and everything above it. Defaults to True.
            leave (bool, optional):
                When node_to=None, only remove what is above or below and not the node itself. Defaults to True.
            no_check (bool, optional):
                Don't check that the graph structure is damaged. Default to False
        """

        if node_to is None:
            if up:
                nodes_above = set(self.nodes_above(node_from))
                if leave:
                    remove_nodes = nodes_above
                    # remove constant inputs on the node left as targets for removal
                    for in_edge in self.indexed_in_edges(node_from):
                        if isinstance(in_edge.from_node, ConstantInputNode):
                            nodes_above.remove(in_edge.from_node)
                else:
                    remove_nodes = nodes_above | {node_from}
                # check for deleted nodes that have edges to left nodes. These need to be the new inputs.
                # group them by source so common edges have one input
                inputs_on = [
                    list(edges) for _, edges in
                    groupby(
                        [edge for node in remove_nodes for edge in self.out_edges(node)
                         if edge.to_node not in remove_nodes],
                        key=lambda x: (x.from_node, x.from_idx))]
                dims = [edges[0].to_node.in_dims[edges[0].to_idx]
                        for edges in inputs_on]
                input_names = sorted(
                    [node.name for node in remove_nodes if isinstance(node, InputNode)])
                LOG.info(f"Removing nodes: {[node.name for node in remove_nodes]}")
                self.remove_all(remove_nodes)

                for idx, edge_group in enumerate(inputs_on):
                    name = input_names.pop(0) if input_names else None
                    in_node = self.add_input(dims[idx], name=name)
                    LOG.info(f'adding input {in_node.name}')
                    for edge in edge_group:
                        self.add_edge(NNEdge(from_node=in_node,
                                               to_idx=edge.to_idx,
                                               to_node=edge.to_node))
                    if self.quantization and edge_group:
                        edge = edge_group[0]
                        fnid = edge.to_node.name
                        if fnid in self.quantization:
                            qrec = self.quantization[fnid]
                            self.quantization[in_node.name] = QRec.copy_ktype(
                                qrec, out_qs=[qrec.in_qs[edge.to_idx]])
            else:
                nodes_below = set(self.nodes_below(node_from))
                if not no_check and self.is_vertex_cut(nodes_below):
                    raise ValueError(
                        f'removing everything below {node_from.name} would split the graph which is not permitted')
                if leave:
                    remove_nodes = nodes_below
                    outputs_on = [edge_bundle[0]
                                  for edge_bundle in self.indexed_out_edges(node_from)]
                else:
                    input_nodes = {edge.from_node for edge in self.in_edges(node_from)
                                   if isinstance(edge.from_node, (InputNode, ConstantInputNode))}
                    remove_nodes = nodes_below | {node_from} | input_nodes
                    outputs_on = self.indexed_in_edges(node_from)
                output_names = sorted(
                    [node.name for node in remove_nodes if isinstance(node, OutputNode)])

                self.remove_all(remove_nodes)
                for edge in outputs_on:
                    name = output_names.pop(0) if output_names else None
                    out_node = self.add_output(name=name)
                    LOG.info(f'adding output {out_node.name}')
                    self.add_edge(NNEdge(from_node=edge.from_node,
                                           from_idx=edge.from_idx, to_node=out_node))
                    if self.quantization:
                        fnid = edge.from_node.name
                        if fnid in self.quantization:
                            qrec = self.quantization[fnid]
                            self.quantization[out_node.name] = QRec.copy_ktype(
                                qrec, in_qs=[qrec.out_qs[edge.from_idx]])
        else:
            nodes_between = self.nodes_between(node_from, node_to)
            if not nodes_between:
                raise ValueError(
                    f'there are no nodes between {node_from.name} and {node_to.name}')
            if not self.nodes_between_in(node_from, node_to, nodes_between):
                raise ValueError(
                    f'all paths from {node_from.name} must lead to {node_to.name}')

            edges_from = set(self.out_edges(node_from))
            edges_to = set(self.in_edges(node_to.name))
            between_edges = reduce(lambda s, x: s | set(
                self.edges(x)), nodes_between, set())
            edges_from = edges_from.intersection(between_edges)
            edges_to = edges_to.intersection(between_edges)
            if len(edges_from) != len(edges_to):
                self.perror(
                    f"{node_from.name} has a different number of outputs than {node_to.name}'s inputs")
                return
            for idx, _ in enumerate(edges_from):
                if node_from.out_dims[idx].size() != node_to.in_dims[idx].size():
                    self.perror(
                        f"{node_from.name} output {idx} has a different size to {node_to.name}'s input")
                    return
            self.remove_all(nodes_between)
            for idx, _ in enumerate(edges_from):
                if node_from.out_dims[idx].shape != node_to.in_dims[idx].shape:
                    reshape = ReshapeNode(
                        self.unique_name(f'{node_from.name}_reshape{idx}'),
                        old_shape=node_from.out_dims[idx],
                        shape=node_to.in_dims[idx])
                    self.add_edge(NNEdge(from_node=node_from,
                                           from_idx=idx, to_node=reshape))
                    self.add_edge(
                        NNEdge(from_node=reshape, to_node=node_to, to_idx=idx))
                else:
                    self.add_edge(NNEdge(from_node=node_from,
                                           from_idx=idx, to_node=node_to, to_idx=idx))
        self.add_dimensions()

    def show(self,
             show_constants=False,
             width=150) -> str:
        """ Show the structure of the graph

        Args:
            show_constants (bool, optional): Include constants in report. Defaults to False.
            width (int, optional): Width in characters of the report. Defaults to 150 characters.

        Returns:
            str: A table of the nodes in the graph
        """
        tab = GraphReporter(show_constants=show_constants).report(self)
        renderer = TextTableRenderer(width)
        tab.render(renderer)
        return renderer.get_output()

    def qshow(self,
              nodes=None,
              width=150) -> str:
        """ Show the quantization of the graph

        Args:
            nodes (list, optional): List of graph nodes of which you want a quantization report, i.e. G.qshow([G[10]])
            width (int, optional): Width in characters of the report. Defaults to 150 characters.

        Returns:
            str: A table of the node quantification
        """
        tab = QuantizationReporter().report(
            self, self.quantization, nodes)
        renderer = TextTableRenderer(width)
        tab.render(renderer)
        return renderer.get_output()

    #pylint: disable=redefined-outer-name
    def draw(self,
             all_dims=False,
             filepath: str = None,
             view=True,
             expressions: Literal["quantized", "unquantized"] = None,
             quant_labels=False,
             fusions=False,
             nodes: Sequence[NNNodeBase] = None):
        """ Draw the graph in a PDF file

        Args:
            all_dims (bool, optional):
                Show dimensions of constant inputs. Defaults to False.
            filepath (str, optional):
                Filename to save PDF to. If None a temporary file will be used. Defaults to None.
            view (bool, optional):
                Launch the default PDF viewer to view file. Defaults to True.
            expressions (Literal["quantized", "unquantized"], optional):
                Show fused piecewise expressions. If the graph is quantized and "quantized" is selected then
                the quantized version of the compiled piecewise expression will be shown. Defaults to False.
            quant_labels (bool, optional):
                Instead of showing tensor dimensions on edges show the quantization information for the edge.
                Defaults to False.
            fusions (bool, optional):
                Show all fusion internals (except expressions). Defaults to False.
            nodes (Sequence[NNNodeBase], optional):
                List of nodes to draw. If None all nodes except constants to reduce the complexity of the displayed
                graph. Constants can be included by calling with nodes=graph.nodes().
        """
        if (expressions == "quantized" or quant_labels) and not self.quantization:
            raise ValueError(
                "you cannot view quantization information since the graph is not quantized")
        DrawGraphReporter().report(self, nodes=nodes, all_dims=all_dims, filename=filepath, view=view,
                                   expressions=expressions, quant_labels=quant_labels, fusions=fusions)

    def use_compressed(self, on_off=True):
        """Switch on and off compression on constant nodes that have compressed values"""
        for node in self.all_constants:
            if node.compressed_value:
                node.use_compressed = on_off

    @staticmethod
    def read_graph_state(graphpath: str) -> 'NNGraph':
        """Read a graph zip module created by write_graph_state.

        Args:
            graphpath (str): path to archive file

        Returns:
            NNGraph: Created graph
        """
#pylint: disable=no-member
        test_graph_module = zipimport.zipimporter(graphpath)
        graph_base = os.path.basename(graphpath)
        if graph_base.endswith('.zip'):
            graph_base = graph_base[:-len('.zip')]
        graph_mod = test_graph_module.load_module(graph_base)
        return getattr(graph_mod, graph_base)

    def write_graph_state(self, graphpath: str = None, build_settings=None) -> Tuple[Any]:
        """Save a graph along with all changes made and quantization.

        The zip file created by this method can be loaded with NNGraph.read_graph_state(path_to_file).

        The file is actually a zip module with a single module in it with the
        name of the basename of the supplied path without a .zip extension if any.
        The module contains a single element with the same name containing the
        imported graph.

        It can be imported like::

            import sys
            sys.path.insert(0, "/path_to_zip/mygraph.zip")
            from mygraph import mygraph


        mygraph will now contain the imported NNGraph

        Args:
            graphpath (str, optional):
                If provided a zip module of the graph along with its tensors is saved.
                Defaults to None.
            build_settings (dict, optional):
                Override build settings in graph.settings.

        Returns:
            Tuple[Any]: The python function used to create the graph and dictionary of the tensors.
        """

        if build_settings is None:
            build_settings = self.settings
        return dump_graph(self, graphpath=graphpath, build_settings=build_settings)

    def __getitem__(self, key: Union[str, int]) -> NNNodeBase:
        """ Get node parameters

        Args:
            key (Union[str, int]): Either the node name or a node step number. Can be a slice.

        Returns:
            Parameters: The node selected or nodes if a slice.
        """
        try:
            return self.nodes_by_step_idx[key]
        except:
            return super().__getitem__(key)

    def __repr__(self) -> str:
        return f'NNGraph(name={self.name!r}, filename={self.filename!r})'


class NNSubgraphGraph(Graph):
    def __init__(self, **attr):
        super().__init__(**attr)
        self.steps = None
        self.liveness = None

    def add_dimensions(
        self,
    ):
        """Add dimensions to the graph and calculate execution order and liveness

        """
        self.steps = add_dimensions(self)
        self.liveness = calculate_liveness(
            self,
            self.steps)
