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

import numpy as np
from bfloat16 import bfloat16
from expressions.symbolic.kernel_codegen import BasicKernel
from graph.types import (ConcatParameters, ConstantInputParameters,
                         InputParameters, OutputParameters, ReshapeParameters,
                         SplitParameters, SSDDetectorParameters,
                         TransposeParameters)
from graph.types.lstm import LSTMParameters
from graph.types.others import CopyParameters, QuantizeParameters
from graph.types.rnn import RNNBaseParameters
from utils.node_id import NodeId

from generation.generator_decorators import RegisteredGeneratorsMixin
# pylint: disable=wildcard-import,unused-wildcard-import
from generation.generators import *
from generation.generators.globals.global_names import *
from generation.name_cache import NameCache
from generation.new_generators.generator_helpers import NewGenerator

from .at_types.gen_ctrl import gen_graph_ctrl, gen_kernel_ctrl
from .at_types.tc_arg_info import LocalArgInfo
from .at_types.tensor_stack import TensorStack
from .autotiler_options import AUTO_TILER_OPTIONS, DEFAULT_GEN_OPTS
from .bindings import FunctionBindingList, GArgEdge, Imm
from .checksums import calc_value_checksum, checksum_func
from .code_block import CodeBlock
from .write_constants import write_constants

LOG = logging.getLogger("nntool." + __name__)

CTYPE = {"int8": "signed char",
         "uint8": "unsigned char",
         "int16": "short int",
         "uint16": "unsigned short int",
         "int32": "int",
         "uint32": "unsigned int",
         "bfloat16": "F16",
         "float16": "F16"}


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


class CodeGenerator(NewGenerator, RegisteredGeneratorsMixin):
    def __init__(self, G, naming_convension, opts=None):
        super().__init__()
        self.G = G
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
        self.include_files = ["CNN_Basic_Kernels.h"]
        self.opts = DEFAULT_GEN_OPTS.copy()
        if opts is not None:
            self.opts.update(opts)
        if self.opts['include_project_header']:
            self.include_files.append(self.project_name + '.h')
        has_vcd = False
        for step in G.graph_state.steps:
            node = step['node']
            if node.at_options.vcd_trace_on is not None:
                has_vcd = True
        if has_vcd:
            self.include_files.append('hal/gvsoc/gvsoc.h')

    @property
    def project_name(self):
        return self.naming_convension.get_project_name()

    @property
    def force_relu(self):
        return self.opts['AT_force_relu']

    @property
    def flash_pointer(self):
        return "AT_HYPERFLASH_FS_EXT_ADDR_TYPE" if self.opts["l3_flash_device"] == "AT_MEM_L3_HFLASH" else "AT_QSPIFLASH_FS_EXT_ADDR_TYPE"

    def get_edge_name(self, eparams):
        return self.name_cache[eparams]['edge']

    def get_node_name(self, params, target):
        try:
            return self.name_cache[params][target]
        except:
            raise ValueError(f"Name Cache: {params.name} {target} not found")

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
    def real_up_connection(G, eparams, set_real=False):
        while isinstance(eparams.creating_node, ReshapeParameters) or \
                (isinstance(eparams.creating_node, TransposeParameters) and eparams.creating_node.does_nothing()):
            set_real = True
            eparams = G.in_edges(eparams.creating_node.name)[0].params
        return eparams, set_real

    @staticmethod
    def real_down_connection(G, eparams):
        oedge = G.out_edges(eparams.creating_node.name)[
            eparams.creating_node_idx]
        while isinstance(oedge.to_node, ReshapeParameters) or \
                (isinstance(oedge.to_node, TransposeParameters) and oedge.to_node.does_nothing()):
            assert len(G.out_edges(oedge.to_node.name)) <= 1
            oedge = G.out_edges(oedge.to_node.name)[0]
        return oedge

    def local_generator(self, indent=0):
        edges = set(edge.params for edge in self.G.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        for eparams in sorted_edges:
            # check if the following real node is an output
            if isinstance(eparams.creating_node, ConcatParameters):
                rout_edge = self.real_down_connection(self.G, eparams)
                if isinstance(rout_edge.to_node, OutputParameters):
                    rout_eparams = rout_edge.params
                    cname = self.naming_convension.get_edge_name(rout_eparams.creating_node,
                                                                 rout_eparams.creating_step,
                                                                 rout_eparams.edge_type,
                                                                 rout_eparams.edge_order)
                    LOG.info("edge from step %s %s is not used and is replaced with edge to step %s:%s %s cname: %s",
                             eparams.creating_node.step_idx, eparams.creating_node.name,
                             rout_eparams.creating_node.name, rout_eparams.creating_node.step_idx,
                             rout_eparams.creating_step, cname)
                    self.name_cache.set(eparams, 'edge', cname)
                    continue

            rin_eparams, set_real = self.real_up_connection(self.G, eparams)
            if rin_eparams.edge_type == "out":
                # The edge was marked as an output so find the real edge down
                rin_eparams = self.real_down_connection(
                    self.G, rin_eparams).params
                self.name_cache.set(eparams, 'edge', rin_eparams.name)
                continue
            else:
                if set_real:
                    # Code will not be generated for reshape or empty transpose so the input to the
                    # following node is the input to this node
                    cname = self.naming_convension.get_edge_name(rin_eparams.creating_node,
                                                                 rin_eparams.creating_step,
                                                                 rin_eparams.edge_type,
                                                                 rin_eparams.edge_order)
                    LOG.info("edge from step %s %s is not used and is replaced with edge from step %s:%s %s cname: %s",
                             eparams.creating_node.step_idx, eparams.creating_node.name,
                             rin_eparams.creating_node.name, rin_eparams.creating_node.step_idx,
                             rin_eparams.creating_step, cname)
                    self.name_cache.set(eparams, 'edge', cname)
                    continue

            cname = self.naming_convension.get_edge_name(eparams.creating_node,
                                                         eparams.creating_step,
                                                         eparams.edge_type,
                                                         eparams.edge_order)

            qrec = self.G.quantization[NodeId(eparams.creating_node, None)]
            out_q = qrec.out_qs[eparams.creating_node_idx]
            self.name_cache.set(eparams, 'edge', cname)
            if isinstance(eparams.creating_node, ConstantInputParameters):
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
            code_block.write_start("CArgs({},", len(self.locals))
        else:
            code_block.write_start("CArgs(0")
        code_block.indent()
        first = True
        for local_def in self.locals:
            if first:
                first = False
            else:
                code_block.append_last(',')
            code_block.write(str(local_def))
        code_block.deindent()
        code_block.write(")")
        return str(code_block)

    def stack_generator(self, indent=0):
        edges = set(edge.params for edge in self.G.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        concat_edges = list([eparams for eparams in sorted_edges if isinstance(
            eparams.creating_node, ConcatParameters)])
        for eparams in concat_edges:
            node = eparams.creating_node
            cname_out = self.name_cache[eparams]['edge']
            in_edge_names = [self.name_cache[edge.params]['edge']
                             for edge in self.G.indexed_in_edges(node.name)]
            self.stacked_tensors.append(TensorStack(cname_out, in_edge_names))

        split_nodes = [node for node in self.G.nodes(
        ) if isinstance(node, SplitParameters)]
        for split_node in split_nodes:
            eparams_in = self.G.in_edges(split_node.name)[0].params
            eparams_out = [
                edge_bundle[0].params for edge_bundle in self.G.indexed_out_edges(split_node.name)]
            cname_in = self.name_cache[eparams_in]['edge']
            cnames_out = [self.name_cache[eparams]['edge']
                          for eparams in eparams_out]
            self.stacked_tensors.append(TensorStack(cname_in, cnames_out))

        code_block = CodeBlock(starting_indent=indent)
        if len(self.stacked_tensors) == 0:
            code_block.comment(
                "no concats in graph so not stacked tensors created")
        else:
            for stacked_tensor in self.stacked_tensors:
                code_block.write(str(stacked_tensor))
        return str(code_block)

    def global_generator(self, indent=0):
        self.generate_inputs()
        self.generate_constants()
        self.generate_outputs()
        code_block = CodeBlock(starting_indent=indent + 1)
        code_block.write("CArgs({}", len(self.globals))
        code_block.indent()
        first = False
        for global_def in self.globals:
            if first:
                first = False
            else:
                code_block.append_last(',')
            if global_def.comment is not None:
                code_block.comment(global_def.comment)
            code_block.write(str(global_def))

        code_block.deindent()
        code_block.write(")")
        return str(code_block)

    def generate_outputs(self):
        outputs = set()
        count_outputs = 0
        for node in self.G.output_nodes():
            qrec = self.G.quantization[NodeId(node)]
            for edge in self.G.in_edges(node.name):
                if isinstance(edge.from_node, (LSTMParameters, )) and count_outputs:
                    continue
                eparams, _ = self.real_up_connection(self.G, edge.params)
                if eparams in outputs:
                    continue
                eparams.edge_type = "out"
                outputs.add(eparams)
                self.execute_phase("outputs", node, qrec, edge)
                count_outputs += 1

    def generate_constants(self):
        for _, pnode, _, fnode in self.G.nodes_iterator():
            anode = pnode if not fnode else fnode
            qrec = self.G.quantization.get(NodeId(pnode, fnode))
            if not self.new_execute_phase("globals", anode, qrec, pnode, fnode):
                self.execute_phase("globals", anode, qrec, pnode, fnode)

    def generate_inputs(self):
        inputs = set()
        for node in self.G.input_nodes():
            qrec = self.G.quantization[NodeId(node)]
            for edge in self.G.out_edges(node.name):
                eparams = edge.params
                if eparams in inputs:
                    continue
                inputs.add(eparams)
                self.execute_phase("inputs", node, qrec, edge)

    def cnn_includes_generator(self):
        includes = []
        if any(qrec.ktype == "scaled" for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators_SQ8.h"')
            if self.G.has_rnn:
                includes.append('"RNN_Generators_SQ8.h"')
        if any(qrec.ktype == "symmetric" for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators.h"')
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators_fp16.h"')
            if self.G.has_rnn:
                includes.append('"RNN_Generators_fp16.h"')
        if any(qrec.cache.get('ne16') for qrec in self.G.quantization.values()):
            includes.append('"CNN_Generators_NE16.h"')
            if self.G.has_rnn:
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
            if self.G.has_rnn:
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
        kernels = self.cnn_kernels() + \
            ["\"" + self.project_name + ".h\""]  # default
        if self.G.has_ssd_postprocess:
            if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
                kernels.append("\"SSD_BasicKernels_fp16.h\"")
            kernels.append("\"SSD_BasicKernels.h\"")
        if self.G.has_resizer:
            kernels.append("\"ResizeBasicKernels.h\"")
        if self.G.has_expressions:
            kernels.append('"CNN_BasicKernels_SQ8.h"')
            kernels.append("\"%s\"" % self.opts['basic_kernel_header_file'])
        if self.G.nodes(node_classes=(CopyParameters,)):
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
        for _, node, _, _ in self.G.nodes_iterator(yield_fusions=False):
            name = node.name
            cname = self.get_node_cname(node)
            if node.at_options.vcd_trace_on is not None:
                self.add_vcd_trace_binding(cname, node.at_options.vcd_trace_on)
            self.name_cache.set(node, 'node', cname)
            in_eparams = self.G.get_in_params(name)
            out_eparams = self.G.get_out_params(name)
            try:
                qrec = self.G.quantization[NodeId(node)]
            except KeyError as err:
                LOG.error("Quantization record not found for node %s", node.name)
                raise err
            if isinstance(node, ReshapeParameters):
                continue
            if isinstance(node, TransposeParameters) and node.does_nothing():
                continue
            elif isinstance(node, (InputParameters, OutputParameters)):
                continue
            elif isinstance(node, ConstantInputParameters):
                # constants that are initializers need to do a binding
                if not self.new_execute_phase("bindings", node, qrec,
                                              in_eparams, out_eparams, cname):
                    self.execute_phase("bindings", node, qrec,
                                       in_eparams, out_eparams, cname)
                continue
            elif not isinstance(node, (ConcatParameters, SplitParameters)):
                if not self.new_execute_phase("bindings", node, qrec,
                                              in_eparams, out_eparams, cname):
                    self.execute_phase("bindings", node, qrec,
                                       in_eparams, out_eparams, cname)
                if not (self.new_execute_phase("kernels", node, qrec, in_eparams, out_eparams, cname) or
                        self.execute_phase("kernels", node, qrec, in_eparams, out_eparams, cname)):
                    raise NotImplementedError(f"Don't know how to generate kernel for parameter type {node.name} {node.CLS_OP_NAME}. "
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
        node = self.G[name]
        if before:
            size = node.in_dims[0].size()
        else:
            size = node.out_dims[0].size()
        self.bindings.append(
            FunctionBindingList(cname,
                                checksum_func(self.G, name),
                                Imm(step_idx),
                                Imm(calc_value_checksum(self.G, name)),
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
            if self.G.has_rnn:
                code_block.write("Load_RNN_SQ8_Library();")
            if self.G.has_ssd_postprocess:
                code_block.write("LoadSSDLibrary();")
        if "POW2" in self.G.quantization.schemes_present:
            code_block.write("LoadCNNLibrary();")
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            code_block.write("LoadCNNLibrary_fp16();")
            if self.G.has_rnn:
                code_block.write("LoadRNN_fp16_Library();")
            if self.G.has_ssd_postprocess:
                code_block.write("LoadSSDLibrary_fp16();")
        if any(qrec.cache.get('ne16') for qrec in self.G.quantization.values()):
            # We need to ensure that also LoadCNN_SQ8_Library is called
            code_block.write("LoadCNN_NE16_SQ8_Library();")
            if self.G.has_rnn:
                code_block.write("Load_RNN_NE16_Library();")
        if self.G.has_resizer:
            code_block.write("LoadResizeLibrary();")
        if self.G.nodes(node_classes=(CopyParameters, QuantizeParameters, TransposeParameters)):
            code_block.write("LoadCNN_Copy_Library();")
        if self.G.has_dsp:
            code_block.write("LoadMFCCLibrary();")

        return str(code_block)

    def header_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for _, node, _, fnode in self.G.nodes_iterator():
            if fnode:
                continue
            cname = self.name_cache[node]['node']
            qrec = self.G.quantization[NodeId(node)]
            code_block.comment(cname)
            if any(qrec.ktype == "scaled" for qrec in self.G.quantization.values()):
                code_block.write("#define {}_{}_OUT_SCALE\t{}".format(
                    self.G.name, cname, qrec.out_qs[0].scale[0]))
                qscales, qnorms, zero_point = qrec.out_qs[0].get_quantized_scale(
                )
                code_block.write(
                    "#define {}_{}_OUT_QSCALE\t{}".format(self.G.name, cname, qscales[0]))
                code_block.write(
                    "#define {}_{}_OUT_QNORM\t{}".format(self.G.name, cname, qnorms[0]))
                code_block.write(
                    "#define {}_{}_OUT_ZERO_POINT\t{}".format(self.G.name, cname, zero_point[0]))

            if any(qrec.ktype == "symmetric" for qrec in self.G.quantization.values()):
                for out_q in qrec.out_qs:
                    code_block.write("#define {}_{}_Q\t{}".format(self.G.name, cname, out_q.q))
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
                qrec = self.G.quantization[NodeId(node)]
                basic_kernel = BasicKernel(qrec.cache['qfunc_col'],
                                           [inp.name for inp in node.constant_inputs])
                self.expressions_kernel_cache[node] = basic_kernel
            yield node, basic_kernel

    def expressions_kernel_source_generator(self):
        code_block = CodeBlock(starting_indent=0)
        for node, basic_kernel in self.expressions_foreach_basic_kernel():

            func_name, arg_name = self.expressions_get_names(node)
            basic_kernel.gen_function(func_name,
                                      arg_name,
                                      code=code_block)
        return str(code_block)

    def expressions_kernel_types_generator(self):
        code_block = CodeBlock(starting_indent=0)
        for node, basic_kernel in self.expressions_foreach_basic_kernel():
            _, arg_name = self.expressions_get_names(node)
            basic_kernel.kernel_arg_type_codegen(arg_name, code=code_block)
        return str(code_block)

    def expressions_kernel_includes_generator(self):
        code_block = CodeBlock(starting_indent=0)
        includes = set.union(*[basic_kernel.func_col.c_header_set for node,
                               basic_kernel in self.expressions_foreach_basic_kernel()])
        for include in includes:
            code_block.write('#include {}', include)
        return str(code_block)

    def expressions_kernel_prototypes_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for node in self.G.all_expressions:
            func_name, arg_name = self.expressions_get_names(node)
            code_block.write("void {}({} *Args);", func_name, arg_name)
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

    def generate_main_appl_inout_def(self, test_inputs=None, test_outputs=None, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("/* Inputs */")
        for i, node in enumerate(self.G.input_nodes()):
            if node.at_options.allocate or node.at_options.extern_input_pointer:
                continue
            nodeq = self.G.quantization[NodeId(node, None)].out_qs[0]
            if test_inputs:
                inp = test_inputs[i]
                code_block.write(
                    'L2_MEM {} {}[] = {{{}}};',
                    dtype2ctype(inp),
                    node.name.capitalize(),
                    ",".join(f'{elem}{dtype2typesuffix(inp)}' if not np.isposinf(elem) else "INFINITY" for elem in inp.flatten()))
            else:
                code_block.write(
                    f"L2_MEM {CTYPE[nodeq.ctype]} {node.name.capitalize()}[{node.out_dims[0].size()}];")
        code_block.write("/* Outputs */")
        for node in self.G.output_nodes():
            if node.at_options.allocate:
                continue
            nodeq = self.G.quantization[NodeId(node, None)].out_qs[0]
            code_block.write(
                f"L2_MEM {CTYPE[nodeq.ctype]} {node.name.capitalize()}[{node.out_dims[0].size()}];")

        if test_outputs:
            for out_n, outp in zip(self.G.output_nodes(), test_outputs):
                code_block.write(
                    'L2_MEM {} {}_gt[] = {{{}}};',
                    dtype2ctype(outp),
                    out_n.name.capitalize(),
                    ",".join(f'{elem}{dtype2typesuffix(outp)}' if not np.isposinf(elem) else "INFINITY" for elem in outp.flatten()))
        return str(code_block)

    def gen_inout_list(self):
        inout_str = ""
        for node in self.G.input_nodes():
            if node.at_options.allocate or node.at_options.extern_input_pointer:
                continue
            inout_str += f"{node.name.capitalize()}, "
        rnn_present = any([isinstance(node, RNNBaseParameters)
                           for node in self.G.nodes()])
        if rnn_present:
            inout_str += "1, "
        for node in self.G.output_nodes():
            if node.at_options.allocate:
                continue
            inout_str += f"{node.name.capitalize()}, "
        return inout_str[:-2]

    def generate_output_check(self, tol=0.0, indent=0):
        code = CodeBlock(starting_indent=indent)
        code.write('int errors;')
        for idx, out_node in enumerate(self.G.output_nodes()):
            out_sz = out_node.out_dims[0].size()
            nodeq = self.G.quantization[NodeId(out_node, None)].out_qs[0]
            dtype = "%f" if nodeq.is_floating else "%d"
            code.write('errors = 0;')
            code.write('for (int j=0; j<{}; j++) {{', out_sz)
            code.indent()
            if tol:
                code.write(f"{dtype2ctype(nodeq)} diff = {out_node.name.capitalize()}[j] - {out_node.name.capitalize()}_gt[j];")
                code.write(f'if (diff > {nodeq.quantize(np.array(tol)).item()} || diff < -{nodeq.quantize(np.array(tol)).item()}) ' + '{{')
            else:
                code.write(f'if ({out_node.name.capitalize()}[j] != {out_node.name.capitalize()}_gt[j]) ' + '{{')
            code.indent()
            code.write('errors++;')
            code.write(
                f'printf("Error @ %d: {dtype} instead of {dtype}\\n", j, {out_node.name.capitalize()}[j], {out_node.name.capitalize()}_gt[j]);')
            code.deindent()
            code.write("}}")
            code.deindent()
            code.write('}}')
            code.write(
                f'printf("{out_node.name.capitalize()}: %d/{out_sz} errors\\n", errors);')
        return str(code)
