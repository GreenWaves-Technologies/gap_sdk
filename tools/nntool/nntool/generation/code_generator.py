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
from itertools import chain

import numpy as np
from bfloat16 import bfloat16
from nntool.expressions.symbolic.iteration_space import IterationSpace
from nntool.graph.manipulations.dimensions import add_dimensions
from nntool.graph.types import (ConcatNode, ConstantInputNode,
                         CopyNode, ExpressionFusionNode,
                         FusionNodeBase, InputNode, NNEdge, OutputNode,
                         QuantizeNode, RNNNodeBase,
                         SplitNode, TransposeNode)
from nntool.graph.types.rnn import GRUNode
from nntool.utils.slugify import variablize

from nntool.generation.gen_utils import ModelGenerationInternalError
# pylint: disable=wildcard-import,unused-wildcard-import
from nntool.generation.generators import *
from nntool.generation.generators.helpers.global_names import *
from nntool.generation.name_cache import NameCache
from nntool.generation.generators.generator_helpers import NewGenerator

from .at_types.gen_ctrl import gen_graph_ctrl, gen_kernel_ctrl
from .at_types.tc_arg_info import InputArgInfo, LocalArgInfo, OutputArgInfo
from .at_types.tensor_stack import TensorStack
from .autotiler_options import AUTO_TILER_OPTIONS, DEFAULT_GEN_OPTS
from .bindings import FunctionBindingList, GArgEdge, Imm
from .checksums import calc_value_checksum, checksum_func
from .code_block import CodeBlock
from .write_constants import write_constants

LOG = logging.getLogger(__name__)

CTYPE = {"int8": "signed char",
         "uint8": "unsigned char",
         "int16": "short int",
         "uint16": "unsigned short int",
         "int32": "int",
         "uint32": "unsigned int",
         "bfloat16": "F16",
         "float16": "F16",
         "float32": "float"}


def dtype2ctype(val):
    if val.dtype == np.int8:
        return 'signed char'
    if val.dtype == np.uint8:
        return 'unsigned char'
    if val.dtype == np.int16:
        return 'signed short'
    if val.dtype == np.uint16:
        return 'unsigned short'
    if val.dtype == np.float32 or val.dtype == np.float64:
        return 'float'
    if val.dtype == np.float16:
        return 'F16'
    if val.dtype == bfloat16:
        return 'F16'
    assert False


def dtype2typesuffix(val):
    if val.dtype == np.int8:
        return ''
    if val.dtype == np.uint8:
        return 'U'
    if val.dtype == np.int16:
        return ''
    if val.dtype == np.uint16:
        return 'U'
    if val.dtype == np.float64:
        return 'F'
    if val.dtype == np.float32:
        return 'F'
    if val.dtype == np.float16:
        return 'F'
    if val.dtype == bfloat16:
        return 'F'
    assert False


def dtype2fmtcode(val):
    if val.dtype == np.int8:
        return 'hhd'
    if val.dtype == np.uint8:
        return 'hhu'
    if val.dtype == np.int16:
        return 'hd'
    if val.dtype == np.uint16:
        return 'hu'
    if val.dtype == np.float64:
        return 'f'
    if val.dtype == np.float32:
        return 'f'
    if val.dtype == np.float16:
        return 'f'
    if val.dtype == bfloat16:
        return 'f'
    assert False


class CodeGenerator(NewGenerator):
    def __init__(self, G, naming_convension, opts=None):
        super().__init__()
        self.G = G
        # this generates a view of the graph with all nodes that are not needed during generation removed
        self.hidden_graph = G.with_hidden_nodes(
            lambda node: node.exclude_from_generation,
            edge_class=NNEdge
        )
        self.sorted_nodes = sorted(
            self.hidden_graph.nodes(), key=lambda node: node.step_idx)
        naming_convension.G = self.hidden_graph
        # the edge parameters are generated from the graph with the hidden nodes but the dimensions
        # are not updated. They are read from the nodes
        add_dimensions(self.hidden_graph, update_graph=False)
        self.naming_convension = naming_convension
        self.name_cache = NameCache()
        self.bindings = []
        self.kernels = []
        self.globals = []
        self.stacked_tensors = []
        self.locals = []
        self.inputs_by_name = {}
        self.expressions_kernel_cache = {}
        self.func_bindings = []
        self._globals_run = False
        self._has_constants = False
        self.include_files = ["CNN_Basic_Kernels.h"]
        self.opts = DEFAULT_GEN_OPTS.copy()
        if opts is not None:
            self.opts.update(opts)
        if self.opts['include_project_header']:
            self.include_files.append(self.project_name + '.h')
        if any(step and step['node'].at_options.vcd_trace_on is not None
               for step in G.graph_state.steps):
            self.include_files.append('hal/gvsoc/gvsoc.h')

    @property
    def output_nodes(self):
        for node in sorted(self.hidden_graph.outputs(), key=lambda node: node.name):
            if isinstance(node, OutputNode) and not node.not_used:
                yield node

    @property
    def input_nodes(self):
        for node in sorted(self.hidden_graph.inputs(), key=lambda node: node.name):
            if isinstance(node, InputNode):
                yield node

    @property
    def project_name(self):
        return self.naming_convension.get_project_name()

    @property
    def force_relu(self):
        return self.opts['AT_force_relu']

    @property
    def flash_pointer(self):
        return ("AT_HYPERFLASH_FS_EXT_ADDR_TYPE" if self.opts["l3_flash_device"] == "AT_MEM_L3_HFLASH"
                else "AT_QSPIFLASH_FS_EXT_ADDR_TYPE")

    @property
    def has_constants(self):
        self.initialize_globals()
        return self._has_constants

    @property
    def globals_decl(self):
        self.initialize_globals()
        return self.globals

    def get_edge_name(self, eparams):
        return self.name_cache[eparams]['edge']

    def get_node_name(self, params, target):
        try:
            return self.name_cache[params][target]
        except KeyError as ex:
            raise ModelGenerationInternalError(
                f"Name Cache: {params.name} {target} not found") from ex

    def memory_device_generator(self, indent=0):
        self.opts['memory_devices'].set_l2_ram_ext_managed(
            self.opts['l2_ram_ext_managed'])
        self.opts['memory_devices'].set_l3_ram_ext_managed(self.opts['l3_ram_ext_managed'],
                                                           self.opts['l3_ram_device'])
        self.opts['memory_devices'].set_l3_flash_ext_managed(self.opts['l3_flash_ext_managed'],
                                                             self.opts['l3_flash_device'])

        code_block = CodeBlock(starting_indent=indent)
        self.opts['memory_devices'].gen(self.G, code_block)
        return str(code_block)

    def options_type_generator(self, option_type, code_block):
        if option_type == 'kernel':
            gen_ctrl = gen_kernel_ctrl
        else:
            gen_ctrl = gen_graph_ctrl

        for option in filter(lambda x: x['type'] == option_type, AUTO_TILER_OPTIONS):
            opt_name = option['name']
            opt_l_name = opt_name.lower()
            if self.opts[opt_l_name] != option['default']:
                gen_ctrl('AT_' + opt_name, self.opts[opt_l_name], code_block)

    def options_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        self.options_type_generator('kernel', code_block)
        self.options_type_generator('graph', code_block)
        return str(code_block)

    def binding_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for binding in self.bindings + self.func_bindings:
            binding.gen_bindings(self, code_block)
        return str(code_block)

    @staticmethod
    def get_output(G, eparams):
        oedges = G.indexed_out_edges(eparams.creating_node.name)[
            eparams.creating_node_idx]
        if len(oedges) != 1 or not isinstance(oedges[0].to_node, OutputNode):
            return None
        return oedges[0]

    def local_generator(self, indent=0):
        edges = set(edge.params for edge in self.hidden_graph.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        for eparams in sorted_edges:
            if eparams.edge_type == "out":
                # The edge was marked as an output so find the real output edge
                oedges = self.hidden_graph.indexed_out_edges(eparams.creating_node.name)[
                    eparams.creating_node_idx]
                oedges = list(filter(lambda edge: isinstance(
                    edge.to_node, OutputNode), oedges))
                if not oedges:
                    raise ModelGenerationInternalError(
                        f'output edge created by {eparams.creating_node.name}:{eparams.creating_node_idx} '
                        f'is not connected to an output - {" ".join(edge.to_node.name for edge in oedges)}')
                if len(oedges) > 1:
                    raise ModelGenerationInternalError(
                        f'output edge created by {eparams.creating_node.name}:{eparams.creating_node_idx} '
                        f'is connected to more than one output - {" ".join(edge.to_node.name for edge in oedges)}')

                rin_eparams = oedges[0].params
                self.name_cache.set(eparams, 'edge', rin_eparams.name)
                continue

            cname = self.naming_convension.get_edge_name(eparams.creating_node,
                                                         eparams.creating_step,
                                                         eparams.edge_type,
                                                         eparams.edge_order)

            qrec = self.G.quantization[eparams.creating_node.name]
            out_q = qrec.out_qs[eparams.creating_node_idx]
            self.name_cache.set(eparams, 'edge', cname)
            if isinstance(eparams.creating_node, ConstantInputNode):
                cnode = eparams.creating_node
                if cnode.is_constant or cnode.is_global or out_q.attr.dont_generate_value:
                    continue
            elif eparams.edge_type != "in_out" or eparams.is_alias:
                continue

            home_location = eparams.creating_node.at_options.out_home_mem_loc \
                if eparams.creating_node.at_options.out_home_mem_loc \
                is not None else self.opts['default_local_location']
            exec_location = eparams.creating_node.at_options.out_exec_mem_loc \
                if eparams.creating_node.at_options.out_exec_mem_loc \
                is not None else self.opts['default_local_location']

            self.locals.append(LocalArgInfo(
                out_q.ctype, eparams.name, home_location, exec_location))

        code_block = CodeBlock(starting_indent=indent)
        if len(self.locals) > 0:
            code_block.write("CArgs({},", len(self.locals))
        else:
            code_block.write("CArgs(0")
        code_block.indent()
        for idx, local_def in enumerate(self.locals):
            code_block.write(
                f'{local_def}{"," if idx < (len(self.locals) - 1) else ""}')
        code_block.deindent()
        code_block.write(")")
        return str(code_block)

    def stack_generator(self, indent=0):
        edges = set(edge.params for edge in self.hidden_graph.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        concat_edges = list([eparams for eparams in sorted_edges if isinstance(
            eparams.creating_node, ConcatNode) and eparams.creating_node.no_model_code])
        for eparams in concat_edges:
            node = eparams.creating_node
            cname_out = self.name_cache[eparams]['edge']
            in_edge_names = [self.name_cache[edge.params]['edge']
                             for edge in self.hidden_graph.indexed_in_edges(node.name)]
            self.stacked_tensors.append(TensorStack(cname_out, in_edge_names))

        split_nodes = [node for node in self.hidden_graph.nodes(
        ) if isinstance(node, SplitNode) and node.no_model_code]
        for split_node in split_nodes:
            eparams_in = self.hidden_graph.in_edges(split_node.name)[0].params
            out_edges = self.hidden_graph.indexed_out_edges(split_node.name)
            eparams_out = [
                edge_bundle[0].params for edge_bundle in out_edges]
            unused_sizes = [np.prod(split_node.out_dims[idx].shape)
                            if len(edges) == 1 and isinstance(edges[0].to_node, OutputNode) and
                            edges[0].to_node.not_used
                            else None
                            for idx, edges in enumerate(out_edges)]
            cname_in = self.name_cache[eparams_in]['edge']
            cnames_out = [self.name_cache[eparams]['edge']
                          for eparams in eparams_out]
            self.stacked_tensors.append(TensorStack(
                cname_in, cnames_out, unused_sizes=unused_sizes))

        code_block = CodeBlock(starting_indent=indent)
        if len(self.stacked_tensors) == 0:
            code_block.comment(
                "no concats in graph so not stacked tensors created")
        else:
            for stacked_tensor in self.stacked_tensors:
                code_block.write(str(stacked_tensor))
        return str(code_block)

    def initialize_globals(self):
        if not self._globals_run:
            self._globals_run = True
            self.generate_inputs()
            inputs_len = len(self.globals)
            self.generate_constants()
            self._has_constants = len(self.globals) != inputs_len
            self.generate_outputs()

    def global_generator(self, indent=0):
        self.initialize_globals()
        code_block = CodeBlock(starting_indent=indent + 1)
        if self.globals:
            code_block.write(f"CArgs({len(self.globals)},")
        else:
            code_block.write(f"CArgs(0")
        code_block.indent()
        for idx, global_def in enumerate(self.globals):
            if global_def.comment is not None:
                code_block.comment(global_def.comment)
            code_block.write(
                f'{global_def}{"," if idx < (len(self.globals) - 1) else ""}')

        code_block.deindent()
        code_block.write(")")
        return str(code_block)

    def generate_outputs(self):
        outputs = set()
        for node in self.output_nodes:
            qrec = self.G.quantization[node.name]
            for edge in self.hidden_graph.in_edges(node.name):
                if isinstance(edge.from_node, (RNNNodeBase, )) and edge.from_idx > 0:
                    continue
                eparams = edge.params
                if eparams in outputs:
                    continue
                eparams.edge_type = "out"
                outputs.add(eparams)
                if node.not_used:
                    continue
                eparams = edge.params
                self.globals.append(
                    OutputArgInfo(qrec.in_qs[edge.to_idx].ctype, eparams.name,
                                  self.opts['default_output_home_location'],
                                  self.opts['default_output_exec_location'],
                                  allocate=node.at_options.allocate,
                                  extern_output_pointer=node.at_options.extern_output_pointer))

    def sorted_nodes_and_fusions(self):
        for node in self.sorted_nodes:
            if isinstance(node, FusionNodeBase) and node.quantize_internals:
                for fnode in node.contained_nodes():
                    yield node, fnode
            yield node, None

    def generate_constants(self):
        for pnode, fnode in self.sorted_nodes_and_fusions():
            if fnode:
                anode = fnode
                nid = (pnode.name, fnode.name)
            else:
                anode = pnode
                nid = pnode.name
            qrec = self.G.quantization.get(nid)
            self.new_execute_phase(
                "globals", anode, qrec, pnode, fnode, break_on_true=True)

    def generate_inputs(self):
        inputs = set()
        for node in self.input_nodes:
            qrec = self.G.quantization[node.name]
            for edge in self.hidden_graph.out_edges(node.name):
                eparams = edge.params
                if eparams in inputs:
                    continue
                inputs.add(eparams)
                self.globals.append(
                    InputArgInfo(
                        qrec.out_qs[edge.from_idx].ctype, edge.params.name,
                        self.opts['default_input_home_location'],
                        self.opts['default_input_exec_location'],
                        allocate=node.at_options.allocate,
                        extern_input_pointer=node.at_options.extern_input_pointer))

    def cnn_includes_generator(self):
        includes = []
        if any(qrec.ktype == "scaled" for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators_SQ8.h"')
            if self.G.has_rnn(ktype='scaled'):
                includes.append('"RNN_Generators_SQ8.h"')
        if any(qrec.ktype == "symmetric" for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators.h"')
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators_fp16.h"')
            if self.G.has_rnn(ktype='float'):
                includes.append('"RNN_Generators_fp16.h"')
        if any(qrec.cache.get('ne16') for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators_NE16.h"')
            if self.G.has_rnn(ne16=True):
                includes.append('"RNN_Generators_NE16.h"')
        if self.G.has_resizer:
            includes.append('"ResizeGenerator.h"')
        return "".join(["#include %s\n" % include for include in includes])

    def cnn_kernels(self):
        kernels = []
        if any(qrec.ktype == "scaled" for qrec in self.G.quantization.values()):
            kernels.append("\"CNN_BasicKernels_SQ8.h\"")
        if any(qrec.ktype == "symmetric" for qrec in self.G.quantization.values()):
            kernels.append("\"CNN_BasicKernels.h\"")
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            kernels.append("\"CNN_BasicKernels_fp16.h\"")
        if any(qrec.cache.get('ne16') for qrec in self.G.quantization.values()):
            kernels.append('\"CNN_BasicKernels_NE16.h\"')
            if '\"CNN_BasicKernels_SQ8.h\"' not in kernels:
                kernels.append('\"CNN_BasicKernels_SQ8.h\"')
            if self.G.has_rnn(ne16=True):
                kernels.append('"RNN_BasicKernels_NE16.h"')
        return kernels

    def extra_includes_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        if self.G.has_ssd_postprocess:
            if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
                code_block.write("#include \"SSD_Generators_fp16.h\"")
            code_block.write("#include \"SSD_Generators.h\"")
        if self.G.has_dsp:
            code_block.write("#include \"DSP_Generators.h\"")
        code_block.write("#include \"CNN_Copy_Generators.h\"")
        return str(code_block)

    def used_filenames(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        # put project settings first so that float type is picked up
        kernels = ['"Gap.h"', f'"{self.project_name}.h"']
        kernels.extend(self.cnn_kernels())
        if self.G.has_ssd_postprocess:
            if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
                kernels.append("\"SSD_BasicKernels_fp16.h\"")
            kernels.append("\"SSD_BasicKernels.h\"")
        if self.G.has_resizer:
            kernels.append("\"ResizeBasicKernels.h\"")
        if self.G.has_expressions:
            kernels.append('"CNN_BasicKernels_SQ8.h"')
            kernels.append("\"%s\"" % self.opts['basic_kernel_header_file'])
        if self.G.nodes(node_classes=(CopyNode,)):
            kernels.append("\"CNN_Copy.h\"")
        if self.G.has_dsp:
            kernels.append("\"DSP_Lib.h\"")
        code_block.write("SetUsedFilesNames(0, {}, {});",
                         len(kernels), str(', '.join(kernels)))
        return str(code_block)

    def extra_includes_kernels(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("\"CNN_Copy_Generators.h\"")
        return str(code_block)

    def get_node_cname(self, node):
        cname = self.name_cache.get(node, 'node')
        if not cname:
            cname = self.naming_convension.get_node_name(
                node.name, node.step_idx, node) if not node.node_cname else node.node_cname
            self.name_cache.set(node, 'node', cname)
        return cname

    def kernel_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for node in self.sorted_nodes:
            name = node.name
            cname = self.get_node_cname(node)
            if node.at_options.vcd_trace_on is not None:
                self.add_vcd_trace_binding(cname, node.at_options.vcd_trace_on)
            self.name_cache.set(node, 'node', cname)
            in_eparams = [edge.params if edge else None
                          for edge in self.hidden_graph.indexed_in_edges(name)]
            out_eparams = [edge_bundle[0].params if edge_bundle else None
                           for edge_bundle in self.hidden_graph.indexed_out_edges(name)]
            try:
                qrec = self.G.quantization[node.name]
            except KeyError as err:
                LOG.error("Quantization record not found for node %s", node.name)
                raise err
            if node.exclude_from_generation:
                continue
            elif isinstance(node, (InputNode, OutputNode)):
                continue
            elif isinstance(node, ConstantInputNode) and node.no_model_code:
                # constants that are initializers need to do a binding
                self.new_execute_phase("bindings", node, qrec,
                                       in_eparams, out_eparams, cname, break_on_true=True)
            elif not isinstance(node, (ConcatNode, SplitNode)) or not node.no_model_code:
                self.new_execute_phase("bindings", node, qrec,
                                       in_eparams, out_eparams, cname, break_on_true=True)
                if not self.new_execute_phase("kernels", node, qrec,
                                              in_eparams, out_eparams,
                                              cname, break_on_true=True):
                    raise NotImplementedError("Don't know how to generate kernel for parameter type "
                                              f"{node.name} {node.CLS_OP_NAME}. "
                                              "Perhaps you need to run fusions -a expression_matcher.")

            # if self.opts['generate_checksums']:
            #     if last_node_was_input:
            #         self.add_checksum_binding(cname, name, step_idx, in_eparams, True)
            #     self.add_checksum_binding(cname, name, step_idx, out_eparams, False)
        for kernel in self.kernels:
            kernel.code(code_block)
        return str(code_block)

    def add_vcd_trace_binding(self, cname, enable):
        self.func_bindings.append(
            FunctionBindingList(cname,
                                "gv_vcd_configure",
                                Imm(1 if enable else 0),
                                Imm(0),
                                before=True))

    def add_checksum_binding(self, cname, name, step_idx, eparams, before):
        node = self.hidden_graph[name]
        if before:
            size = node.in_dims[0].size()
        else:
            size = node.out_dims[0].size()
        self.bindings.append(
            FunctionBindingList(cname,
                                checksum_func(self.hidden_graph, name),
                                Imm(step_idx),
                                Imm(calc_value_checksum(
                                    self.hidden_graph, name)),
                                GArgEdge(eparams[0]),
                                Imm(size),
                                before=before)
        )

    def write_constants(self, tensor_directory=None):
        write_constants(
            self.globals, tensor_directory=tensor_directory)

    def load_basic_kernel_library(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        if any(qrec.ktype == "scaled" for qrec in self.G.quantization.values()):
            code_block.write("LoadCNN_SQ8_Library();")
            if self.G.has_rnn(ktype='scaled'):
                code_block.write("Load_RNN_SQ8_Library();")
            if self.G.has_ssd_postprocess:
                code_block.write("LoadSSDLibrary();")
        if any(qrec.ktype == "symmetric" for qrec in self.G.quantization.values()):
            code_block.write("LoadCNNLibrary();")
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            code_block.write("LoadCNNLibrary_fp16();")
            if self.G.has_rnn(ktype='float'):
                code_block.write("LoadRNN_fp16_Library();")
            if self.G.has_ssd_postprocess:
                code_block.write("LoadSSDLibrary_fp16();")
        if any(qrec.cache.get('ne16') for qrec in self.G.quantization.values()):
            # We need to ensure that also LoadCNN_SQ8_Library is called
            code_block.write("LoadCNN_NE16_SQ8_Library();")
            if self.G.has_rnn(ne16=True):
                code_block.write("Load_RNN_NE16_Library();")
        if self.G.has_resizer:
            code_block.write("LoadResizeLibrary();")
        if self.G.nodes(node_classes=(CopyNode, QuantizeNode, TransposeNode)):
            code_block.write("LoadCNN_Copy_Library();")
        if self.G.has_dsp:
            code_block.write("LoadMFCCLibrary();")

        return str(code_block)

    def header_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for node in self.sorted_nodes:
            cname = self.name_cache[node]['node']
            qrec = self.G.quantization[node.name]
            code_block.comment(cname)
            if any(qrec.ktype == "scaled" for qrec in self.G.quantization.values()):
                code_block.write("#define {}_{}_OUT_SCALE\t{}".format(
                    variablize(self.G.name), cname, qrec.out_qs[0].scale[0]))
                qscales, qnorms, zero_point = qrec.out_qs[0].get_quantized_scale(
                )
                code_block.write(
                    "#define {}_{}_OUT_QSCALE\t{}".format(variablize(self.G.name), cname, qscales[0]))
                code_block.write(
                    "#define {}_{}_OUT_QNORM\t{}".format(variablize(self.G.name), cname, qnorms[0]))
                code_block.write(
                    "#define {}_{}_OUT_ZERO_POINT\t{}".format(variablize(self.G.name), cname, zero_point[0]))

            if any(qrec.ktype == "symmetric" for qrec in self.G.quantization.values()):
                for out_q in qrec.out_qs:
                    code_block.write("#define {}_{}_Q\t{}".format(
                        variablize(self.G.name), cname, out_q.q))
        return str(code_block)

    def expressions_kernel_header_includes_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("#include \"{0}\"",
                         self.opts['basic_kernel_header_file'])
        return str(code_block)

    def expressions_foreach_basic_kernel(self):
        for node in self.G.all_expressions:
            basic_kernel = self.expressions_kernel_cache.get(node)
            if not basic_kernel:
                qrec = self.G.quantization[node.name]
                basic_kernel = IterationSpace(qrec.cache['qfunc_col'],
                                              constants=[inp.name for inp in node.constant_inputs])
                self.expressions_kernel_cache[node] = basic_kernel
            yield node, basic_kernel

    def expressions_foreach_custom_act_kernel(self):
        seen_keys = set()
        for node in self.G.nodes(node_classes=FusionNodeBase):
            if not node.quantize_internals:
                continue
            expressions = node.subgraph.nodes(
                node_classes=ExpressionFusionNode)
            if not expressions:
                continue
            expression = expressions[0]
            qrec = self.G.quantization[(node.name, expression.name)]
            qfunc = qrec.cache['qfunc_col']
            kernel_key = qfunc.infos[0] if qfunc.infos else expression
            if kernel_key in seen_keys:
                continue
            seen_keys.add(kernel_key)
            basic_kernel = self.expressions_kernel_cache.get(kernel_key)
            if not basic_kernel:
                basic_kernel = IterationSpace(qfunc)
                self.expressions_kernel_cache[kernel_key] = basic_kernel
            yield kernel_key, basic_kernel

    def expressions_kernel_source_generator(self):
        code_block = CodeBlock(starting_indent=0)
        for node, basic_kernel in self.expressions_foreach_basic_kernel():

            func_name, arg_name = self.expressions_get_names(node)
            basic_kernel.gen_function(func_name,
                                      arg_name,
                                      code=code_block)
            code_block.write('')
        for kernel_key, basic_kernel in self.expressions_foreach_custom_act_kernel():
            func_name = kernel_key.name
            arg_name = f'{kernel_key.name}_args_t'
            basic_kernel.gen_custom_act_function(func_name,
                                                 arg_name,
                                                 code=code_block)
            code_block.write('')
        return str(code_block)

    def expressions_kernel_types_generator(self):
        code_block = CodeBlock(starting_indent=0)
        for node, basic_kernel in self.expressions_foreach_basic_kernel():
            _, arg_name = self.expressions_get_names(node)
            basic_kernel.gen_kernel_arg_typedecl(arg_name, code=code_block)
            code_block.write('')
        for kernel_key, basic_kernel in self.expressions_foreach_custom_act_kernel():
            arg_name = f'{kernel_key.name}_args_t'
            basic_kernel.gen_custom_act_kernel_arg_typedecl(
                arg_name, code=code_block)
            code_block.write('')
        return str(code_block)

    def expressions_kernel_includes_generator(self):
        code_block = CodeBlock(starting_indent=0)
        includes = set.union(*[basic_kernel.assignments.c_header_set for node,
                               basic_kernel in chain(
                                   self.expressions_foreach_basic_kernel(),
                                   self.expressions_foreach_custom_act_kernel())
                               ])
        for include in includes:
            code_block.write('#include {}', include)
        return str(code_block)

    def expressions_kernel_prototypes_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for node in self.G.all_expressions:
            func_name, arg_name = self.expressions_get_names(node)
            code_block.write("void {}({} *Args);", func_name, arg_name)
            code_block.write('')
        for kernel_key, _ in self.expressions_foreach_custom_act_kernel():
            func_name = kernel_key.name
            arg_name = f'{kernel_key.name}_args_t'
            code_block.write("void {}({} *Args);", func_name, arg_name)
            code_block.write('')
        return str(code_block)

    @ staticmethod
    def expressions_get_names(node):
        return "s%s_kernel" % node.step_idx, "s%s_kernel_args_t" % node.step_idx

    def expressions_kernel_load_generator(self, indent=0):
        if not self.G.has_expressions:
            return ""
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("{0}", "void load_expressions_kernels() {")
        code_block.indent()
        for node, basic_kernel in self.expressions_foreach_basic_kernel():
            func_name, arg_name = self.expressions_get_names(node)
            basic_kernel.gen_kernel_model(func_name, arg_name, code=code_block)
        for kernel_key, basic_kernel in self.expressions_foreach_custom_act_kernel():
            func_name = kernel_key.name
            arg_name = f'{kernel_key.name}_args_t'
            basic_kernel.gen_custom_act_kernel_model(
                func_name, arg_name, code=code_block)
        code_block.deindent()
        code_block.write("{0}", "}\n\n")
        return str(code_block)

    def load_expressions_library(self, indent=0):
        if not self.G.has_expressions:
            return ""
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("load_expressions_kernels();")
        return str(code_block)

    def expressions_user_kernel_source_generator(self, indent=0):
        if not self.G.has_expressions:
            return ""
        code_block = CodeBlock(starting_indent=indent)
        for node, basic_kernel in self.expressions_foreach_basic_kernel():
            func_name, _ = self.expressions_get_names(node)
            basic_kernel.gen_user_kernel(
                func_name+"_gen", func_name, code=code_block)
        return str(code_block)

    def generate_main_appl_in_def(self, test_inputs=None, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("/* Inputs */")
        for i, node in enumerate(self.input_nodes):
            if node.at_options.extern_input_pointer:
                # if node.at_options.allocate or node.at_options.extern_input_pointer:
                continue
            nodeq = self.G.quantization[node.name].out_qs[0]
            if test_inputs and not node.at_options.allocate:
                inp = test_inputs[i]
                vals = ",".join(f'{elem}{dtype2typesuffix(inp)}' if not np.isposinf(elem) else "INFINITY"
                                for elem in inp.flatten())
                name = f"{node.name.capitalize()}" #{'_init' if node.at_options.allocate else ''}"
                code_block.write(
                    f'L2_MEM {dtype2ctype(inp)} {name}[] = {{{vals}}};'
                )
            elif not node.at_options.allocate:
                code_block.write(
                    f"L2_MEM {CTYPE[nodeq.ctype]} {node.name.capitalize()}[{node.out_dims[0].size()}];")
        return str(code_block)

    def generate_main_appl_out_def(self, test_outputs=None, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("/* Outputs */")
        for i, node in enumerate(self.G.output_nodes()):
            in_edge = self.hidden_graph.in_edges(node)[0]
            allocate = node.at_options.allocate
            if isinstance(in_edge.from_node, RNNNodeBase) and in_edge.from_idx > 0:
                from_node = in_edge.from_node
                names = from_node.get_name_indexes()
                indexed_in_edges = self.hidden_graph.indexed_in_edges(from_node)
                if in_edge.from_idx == 1:
                    state_in_edge = indexed_in_edges[names['h_state'] if isinstance(from_node, GRUNode) else names['i_state']]
                elif in_edge.from_idx == 2:
                    state_in_edge = indexed_in_edges[names['c_state']]
                node_name = state_in_edge.params.name
                allocate = state_in_edge.from_node.allocate
            else:
                node_name = node.name.capitalize()
            if not allocate:
                nodeq = self.G.quantization[node.name].out_qs[0]
                code_block.write(
                    f"L2_MEM {CTYPE[nodeq.ctype]} {node_name}[{node.out_dims[0].size()}];")
            if test_outputs:
                code_block.write(
                    'L2_MEM {} {}_gt[] = {{{}}};',
                    dtype2ctype(test_outputs[i]),
                    node_name,
                    ",".join(f'{elem}{dtype2typesuffix(test_outputs[i])}' if not np.isposinf(elem) else "INFINITY"
                             for elem in test_outputs[i].flatten()))
        return str(code_block)

    def generate_main_input_copy(self, test_inputs=None, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("/* Copy inputs function */")
        code_block.write('void copy_inputs() {')
        code_block.indent()
        code_block.write("switch_fs_t fs;")
        code_block.write("__FS_INIT(fs);\n")
        if test_inputs:
            for i, node in enumerate(self.input_nodes):
                if not node.at_options.allocate:
                    continue
                inp = test_inputs[i]
                name = node.name.capitalize()
                size = len(inp.flatten())

                code_block.write(f"/* Reading from file {name} */")
                code_block.write(f"void *File_{name};")
                code_block.write(f"int ret_{name} = 0;")
                code_block.write(f"#ifdef __EMUL__")
                code_block.write(f"File_{name} = __OPEN_READ(fs, \"{name}.bin\");")
                code_block.write(f"#else")
                code_block.write(f"File_{name} = __OPEN_READ(fs, \"../../../{name}.bin\");")
                code_block.write(f"#endif")
                code_block.write(f"ret_{name} = __READ(File_{name}, {name}, {size}*sizeof({dtype2ctype(inp)}));")
                code_block.write(f"__CLOSE(File_{name});\n")
                # code_block.write(
                #     f'for (int i = 0; i<{size}; i++) {name}[i] = {name}_init[i];')

        code_block.write("__FS_DEINIT(fs);")
        code_block.deindent()
        code_block.write('}')
        code_block.write('')
        return str(code_block)

    def gen_inout_list(self):
        inout_str = []
        has_rnn = self.hidden_graph.nodes(node_classes=RNNNodeBase)
        for decl in self.globals_decl:
            if decl.arg_scope != 'ARG_SCOPE_ARG' or decl.extern_pointer:
                continue
            if has_rnn and decl.arg_name.endswith('Reset'):
                inout_str.append("1")
            else:
                inout_str.append(decl.arg_name)
        return ", ".join(inout_str)

    def generate_output_check(self, tol=0.0, indent=0):
        code = CodeBlock(starting_indent=indent)
        code.write('int errors;')
        for idx, out_node in enumerate(self.output_nodes):
            out_sz = out_node.out_dims[0].size()
            nodeq = self.G.quantization[out_node.name].out_qs[0]
            in_edge = self.hidden_graph.in_edges(out_node)[0]
            if isinstance(in_edge.from_node, RNNNodeBase) and in_edge.from_idx > 0:
                from_node = in_edge.from_node
                names = from_node.get_name_indexes()
                indexed_in_edges = self.hidden_graph.indexed_in_edges(from_node)
                if in_edge.from_idx == 1:
                    node_name = indexed_in_edges[names['h_state'] if isinstance(from_node, GRUNode) else names['i_state']].params.name
                elif in_edge.from_idx == 2:
                    node_name = indexed_in_edges[names['c_state']].params.name
            else:
                node_name = out_node.name.capitalize()
            dtype = "%f" if nodeq.is_floating else "%d"
            code.write('errors = 0;')
            code.write(
                f"{'float' if nodeq.is_floating else 'int'} max_diff_{idx} = 0;")
            code.write(f'for (int j=0; j<{out_sz}; j++) {{')
            code.indent()
            code.write(
                f"{'float' if nodeq.is_floating else 'int'} diff = {node_name}[j] - "
                f"{node_name}_gt[j];")
            code.write("diff = (diff>0)?diff:(-diff);")
            code.write(f"if (diff > max_diff_{idx}) max_diff_{idx} = diff;")
            if tol:
                code.write(
                    f'if (diff > {nodeq.quantize(np.array(tol)).item()}) {{')
            else:
                code.write(
                    'if (diff > 0) {')
            code.indent()
            code.write('errors++;')
            code.write(f'printf("Error @ %d: {dtype} instead of {dtype}\\n", j, '
                       f'{node_name}[j], {node_name}_gt[j]);')
            code.deindent()
            code.write("}")
            code.deindent()
            code.write('}')
            code.write(
                f'printf("{node_name}: %d/{out_sz} errors\\n", errors);')
            code.write(f'printf("Max error: {dtype}\\n", max_diff_{idx});')
        return str(code)

    def std_float_type_define(self):
        dtypes = set()
        for node in self.G.all_expressions:
            qrec = self.G.quantization[node.name]
            func = qrec.cache['qfunc_col']
            dtypes |= func.dtypes_in_use
        dtypes |= set(qtype.dtype for qrec in self.G.quantization.values() for qtype in qrec.out_qs if qtype)
        if bfloat16 in dtypes:
            if np.float16 in dtypes:
                raise NotImplementedError("mixed use of float16 and bfloat16 is not supported")
            return False
        elif np.float16 in dtypes:
            if bfloat16 in dtypes:
                raise NotImplementedError("mixed use of float16 and bfloat16 is not supported")
            return True
        return False
