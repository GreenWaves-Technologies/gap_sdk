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

from generation.generators import RegisteredGeneratorsMixin
from generation.name_cache import NameCache
from graph.types import (ConcatParameters, ConstantInputParameters, SplitParameters,
                         FilterParameters, InputParameters, OutputParameters,
                         ReshapeParameters, TransposeParameters, SSDDetectorParameters)
from utils.node_id import NodeId

from .at_types.gen_ctrl import gen_graph_ctrl, gen_kernel_ctrl
from .at_types.tc_arg_info import LocalArgInfo
from .at_types.tensor_stack import TensorStack
from .autotiler_options import AUTO_TILER_OPTIONS, DEFAULT_GEN_OPTS
from .bindings import (TT_TENSOR_TYPES, FunctionBindingList, GArgEdge,
                       GArgNode, Imm)
from .checksums import calc_value_checksum, checksum_func
from .code_block import CodeBlock
from .write_constants import write_constants
from generation.generators.globals.global_names import *

LOG = logging.getLogger("nntool." + __name__)


class CodeGenerator(RegisteredGeneratorsMixin):
    def __init__(self, G, naming_convension, opts=None):
        self.G = G
        self.naming_convension = naming_convension
        self.name_cache = NameCache()
        self.bindings = []
        self.kernels = []
        self.globals = []
        self.stacked_tensors = []
        self.locals = []
        self.inputs_by_name = {}
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
    def basic_ker_header(self):
        if self.G.graph_identity.quantization_type == 'SQ8':
            return "CNN_BasicKernels_SQ8.h"
        if self.G.graph_identity.quantization_type == 'POW2':
            return "CNN_BasicKernels.h"
        return ValueError("Quantization type not known %s", self.G.graph_identity.quantization_type)

    @property
    def force_relu(self):
        return self.opts['AT_force_relu']

    def get_edge_name(self, eparams):
        return self.name_cache[eparams]['edge']

    def get_node_name(self, params, target):
        return self.name_cache[params][target]

    def memory_device_generator(self, indent=0):
        self.opts['memory_devices'].set_l2_ram_ext_managed(self.opts['l2_ram_ext_managed'])
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
            (isinstance(eparams.creating_node, TransposeParameters) and
             eparams.creating_node.transpose_dimension == 1):
            set_real = True
            eparams = G.in_edges(eparams.creating_node.name)[0].params
        return eparams, set_real

    @staticmethod
    def real_down_connection(G, eparams):
        oedge = G.out_edges(eparams.creating_node.name)[0]
        while isinstance(oedge.to_node, ReshapeParameters) or \
            (isinstance(oedge.to_node, TransposeParameters) and
             oedge.to_node.transpose_dimension == 1):
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
                rin_eparams = self.real_down_connection(self.G, rin_eparams).params
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

            out_q = self.G.quantization[NodeId(eparams.creating_node, None)]\
                .out_qs[eparams.creating_node_idx]
            self.name_cache.set(eparams, 'edge', cname)
            if isinstance(eparams.creating_node, ConstantInputParameters):
                cnode = eparams.creating_node
                if cnode.is_constant or cnode.is_global or not cnode.generate_value:
                    continue
            elif eparams.edge_type != "in_out" or eparams.is_alias:
                continue
            home_location = eparams.creating_node.at_options.out_home_mem_loc if eparams.creating_node.at_options.out_home_mem_loc \
                            is not None else self.opts['default_local_location']
            exec_location = eparams.creating_node.at_options.out_exec_mem_loc if eparams.creating_node.at_options.out_exec_mem_loc \
                            is not None else self.opts['default_local_location']

            self.locals.append(LocalArgInfo(out_q.ctype, eparams.name, home_location, exec_location))

        code_block = CodeBlock(starting_indent=indent)
        code_block.write_start("CArgs({},", len(self.locals))
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

        split_nodes = [node for node in self.G.nodes() if isinstance(node, SplitParameters)]
        for split_node in split_nodes:
            eparams_in = self.G.in_edges(split_node.name)[0].params
            eparams_out = [edge_bundle[0].params for edge_bundle in self.G.indexed_out_edges(split_node.name)]
            cname_in = self.name_cache[eparams_in]['edge']
            cnames_out = [self.name_cache[eparams]['edge'] for eparams in eparams_out]
            self.stacked_tensors.append(TensorStack(cname_in, cnames_out))

        code_block = CodeBlock(starting_indent=indent)
        if len(self.stacked_tensors) == 0:
            code_block.comment("no concats in graph so not stacked tensors created")
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
                if isinstance(edge.from_node, SSDDetectorParameters) and count_outputs:
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
        if self.G.graph_identity.quantization_type == 'SQ8':
            includes = ['"CNN_Generators_SQ8.h"', '"RNN_Generators_SQ8.h"']
        elif self.G.graph_identity.quantization_type == 'POW2':
            includes = ['"CNN_Generators.h"']
        else:
            raise ValueError()
        return "".join(["#include %s\n"%include for include in includes])

    def cnn_kernels(self):
        if self.G.graph_identity.quantization_type == 'SQ8':
            return "\"CNN_BasicKernels_SQ8.h\""
        if self.G.graph_identity.quantization_type == 'POW2':
            return "\"CNN_BasicKernels.h\""
        raise ValueError()

    def extra_includes_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        if self.G.has_ssd_postprocess:
            code_block.write("#include \"SSD_Generators.h\"")
        code_block.write("#include \"nntool_extra_generators.h\"")
        return str(code_block)

    def used_filenames(self):
        kernels = ["\"nntool_extra_kernels.h\"", self.cnn_kernels(), "\"" + self.project_name + ".h\""] # default
        if self.G.has_ssd_postprocess:
            kernels.append("\"SSD_BasicKernels.h\"")
        return "\tSetUsedFilesNames(0, {}, {});".format(len(kernels), str(', '.join(kernels)))

    def extra_includes_kernels(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        code_block.write("\"nntool_extra_kernels.h\"")
        return str(code_block)

    def kernel_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for step_idx, node, _, _ in self.G.nodes_iterator(yield_fusions=False):
            name = node.name
            cname = self.naming_convension.get_node_name(name, step_idx, node)
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
                if node.transpose_in is not None or node.transpose_out is not None:
                    LOG.error("Don't know how to generate kernel \
                        for a reshape that has a transpose.")
                    return ""
                continue
            elif isinstance(node, (InputParameters, OutputParameters)):
                continue
            elif isinstance(node, ConstantInputParameters):
                # constants that are initializers need to do a binding
                self.execute_phase("bindings", node, qrec, in_eparams, out_eparams, cname)
                continue
            elif not isinstance(node, (ConcatParameters, SplitParameters)):
                self.execute_phase("bindings", node, qrec, in_eparams, out_eparams, cname)
                if not self.execute_phase("kernels", node, qrec, in_eparams, out_eparams, cname):
                    raise NotImplementedError(("Don't know how to generate kernel for parameter type %s %s. " +
                                               "Perhaps you need to run some fusions.") % (node.name,
                                                                                           node.__class__.__name__))

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

    def write_constants(self):
        write_constants(self.globals, tensor_directory=self.opts['tensor_directory'])

    def load_basic_kernel_library(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        if self.G.graph_identity.quantization_type == 'SQ8':
            code_block.write("LoadCNN_SQ8_Library();")
            code_block.write("Load_RNN_SQ8_Library();")
            if self.G.has_ssd_postprocess:
                code_block.write("LoadSSDLibrary();")
            return str(code_block)
        if self.G.graph_identity.quantization_type == 'POW2':
            code_block.write("LoadCNNLibrary();")
            return str(code_block)
        return ValueError("Quantization type not known %s", self.G.graph_identity.quantization_type)

    def header_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for _, node, _, fnode in self.G.nodes_iterator():
            if fnode:
                continue
            cname = self.name_cache[node]['node']
            qrec = self.G.quantization[NodeId(node)]
            code_block.comment(cname)
            if self.G.graph_identity.quantization_type == 'SQ8':
                code_block.write("#define {}_OUT_SCALE\t{}".format(cname, qrec.out_qs[0].scale[0]))
                qscales, qnorms = qrec.out_qs[0].get_quantized_scale()
                code_block.write("#define {}_OUT_QSCALE\t{}".format(cname, qscales[0]))
                code_block.write("#define {}_OUT_QNORM\t{}".format(cname, qnorms[0]))

            elif self.G.graph_identity.quantization_type == 'POW2':
                for out_q in qrec.out_qs:
                    code_block.write("#define {}_Q\t{}".format(cname, out_q.q))
        return str(code_block)
