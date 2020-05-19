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

from graph.types import (ActivationParameters, ConcatParameters,
                         ConstantInputParameters, Conv2DParameters,
                         ConvFusionParameters, FcParameters, FilterParameters,
                         GlobalPoolParameters, InputParameters,
                         MatrixAddParameters, MatScaleFusionParameters,
                         OutputParameters, PoolingParameters,
                         ReshapeParameters, SoftMaxParameters,
                         TransposeParameters, MultiplicativeBiasParameters)
from utils.node_id import NodeId

from .bindings import (TT_TENSOR_TYPES, CommentBindingList,
                       FunctionBindingList, GArgEdge, GArgNode, GNodeArgEdge,
                       GNodeArgNode, Imm, NodeBindingList)
from .checksums import calc_value_checksum, checksum_func
from .code_block import CodeBlock
from .code_generators import (gen_3d_transpose, gen_const_info,
                              gen_conv_pool_relu, gen_global_decl,
                              gen_globalpool, gen_graph_ctrl, gen_input_decl,
                              gen_kernel_ctrl, gen_linear_relu, gen_local_decl,
                              gen_matrixadd, gen_matrixadddyn, gen_matscale,
                              gen_output_decl, gen_pool_relu, gen_softmax,
                              gen_stack_decl)
from .memory_device_info import MemoryDeviceInfos
from .write_constants import write_constants

LOG = logging.getLogger("nntool." + __name__)

AUTO_TILER_OPTIONS = [
    {
        'name': 'KERNEL_BUFFER_PROMOTE', 'type': 'kernel',
        'descr': 'When all user kernel arguments can fit into given L1 memory promote them to buffer', 'default': True},
    {
        'name': 'KERNEL_PARTIAL_BUFFER_PROMOTE', 'type': 'kernel',
        'descr': 'When all tile of a user kernel argument across Input Features can fit into given L1 memory promote them to partial buffer', 'default': True},
    {
        'name': 'KERNEL_NOSOLUTION_ERROR', 'type': 'kernel',
        'descr': 'Report an error when no tiling solution is found', 'default': True},
    {
        'name': 'GRAPH_MONITOR_CYCLES', 'type': 'graph',
        'descr': 'Enable automatic cycle capture for each node of the graph', 'default': False},
    {
        'name': 'GRAPH_MONITOR_CVAR_NAME', 'type': 'graph',
        'descr': 'When monitor cycles is on name of the C var array to receive results', 'default': 'AT_GraphPerf'},
    {
        'name': 'GRAPH_PRODUCE_NODE_NAMES', 'type': 'graph',
        'descr': 'Enable production of an array containing the name of each graph node', 'default': False},
    {
        'name': 'GRAPH_PRODUCE_NODE_CVAR_NAME', 'type': 'graph',
        'descr': 'When producing node names is on name of the C array receiving the names as strings', 'default': 'AT_GraphNodeNames'},
    {
        'name': 'GRAPH_PRODUCE_OPERINFOS', 'type': 'graph',
        'descr': 'Enable production of number of macs for each layer', 'default': False},
    {
        'name': 'GRAPH_PRODUCE_OPERINFOS_CVAR_NAME', 'type': 'graph',
        'descr': 'When Number of oper Infos is on name of the C array receiving mac infos for each node', 'default': 'AT_GraphOperInfosNames'},
    {
        'name': 'GRAPH_REORDER_CONSTANT_IN', 'type': 'graph',
        'descr': 'Enable reodering of constant inputs in order to transform 2D accesses into 1D accesses', 'default': True},
    {
        'name': 'GRAPH_TRACE_EXEC', 'type': 'graph', 'descr': 'Enable trace of activity', 'default': True},
    {
        'name': 'GRAPH_NOINLINE_NODE', 'type': 'graph',
        'descr': 'If on, all user kernel function are marked as noinline', 'default': False},
    {
        'name': 'GRAPH_PREF_L3_EXEC', 'type': 'graph',
        'descr': 'In case a symbol must be allocated in L3 for execution this is the prefered memory', 'default': 'AT_MEM_L3_HRAM'},
    {
        'name': 'GRAPH_CONST_EXEC_FROM_FLASH', 'type': 'graph',
        'descr': 'If on, constant symbols executes from home location', 'default': False},
    {
        'name': 'GRAPH_PREF_L3_HOME', 'type': 'graph',
        'descr': 'For constant symbols which L3 flash prefered memory', 'default': 'AT_MEM_L3_HFLASH'},
]

DEFAULT_GEN_OPTS = {
    'default_input_home_location': 'AT_MEM_L2',
    'default_input_exec_location': 'AT_MEM_L2',
    'default_output_home_location': 'AT_MEM_L2',
    'default_output_exec_location': 'AT_MEM_L2',
    'default_global_home_location': 'AT_MEM_L3_HFLASH',
    'default_global_exec_location': 'AT_MEM_UNDEF',
    'default_local_location': 'AT_MEM_UNDEF',
    'l2_ram_ext_managed': True,
    'l3_ram_ext_managed': False,
    'l3_flash_ext_managed': False,
    'generate_checksums': False,
    'include_project_header': False,
    'checksum_file': '',
    'dump_tensors': False,
    'tensor_directory': '.',
    'model_directory': '.',
    'model_file': 'model.c',
    'at_ver': 3,
    'memory_devices': MemoryDeviceInfos.default()
}

DEFAULT_GEN_OPTS.update({(elem['name'].lower()): elem['default'] for elem in AUTO_TILER_OPTIONS})

DEFAULT_GEN_OPTS_DESCRIPTIONS = {
    'default_input_home_location': 'default home location for inputs for code generation',
    'default_input_exec_location': 'default exec location for inputs for code generation',
    'default_output_home_location': 'default home location for outputs for code generation',
    'default_output_exec_location': 'default exec location for outputs for code generation',
    'default_global_home_location': 'default home location for globals for code generation',
    'default_global_exec_location': 'default exec location for globals for code generation',
    'default_local_location': 'default location for locals for code generation',
    'l2_ram_ext_managed': 'Externally manage L2 RAM',
    'l3_ram_ext_managed': 'Externally manage L3 RAM',
    'l3_flash_ext_managed': 'Externally manage L3 flash',
    'include_project_header': 'Include a header file called "GraphName.h" in generated code',
    'tensor_directory': 'directory to dump tensors to',
    'model_directory': 'directory to dump model to',
    'model_file': 'filename for model',
    'at_ver': 'AutoTiler version',
    'dump_tensors': 'write the tensors to files. currently only works in emulation mode.',
}

DEFAULT_GEN_OPTS_DESCRIPTIONS.update(
    {elem['name'].lower(): elem['descr'] for elem in AUTO_TILER_OPTIONS})


class CodeGenerator():
    def __init__(self, G, naming_convension, opts=None):
        self.G = G
        self.naming_convension = naming_convension
        self.name_cache = {}
        self.bindings = []
        self.inputs_by_name = {}
        self.func_bindings = []
        self.include_files = ["CNN_Basic_Kernels.h"]
        self.opts = DEFAULT_GEN_OPTS.copy()
        if opts is not None:
            self.opts.update(opts)
        if self.opts['include_project_header']:
            self.include_files.append(self.project_name + '.h')
        has_dump = False
        has_vcd = False
        for step in G.graph_state.steps:
            node = step['node']
            if node.at_options.vcd_trace_on is not None:
                has_vcd = True
            if node.at_options.dump_tensors is not None:
                has_dump = True
        if self.opts['dump_tensors'] or has_dump:
            self.include_files.append('helpers.h')
        if has_vcd:
            self.include_files.append('hal/gvsoc/gvsoc.h')

    @property
    def project_name(self):
        return self.naming_convension.get_project_name()

    def get_edge_name(self, eparams):
        return self.name_cache[eparams]['edge']

    def get_node_name(self, params, target):
        return self.name_cache[params][target]

    def memory_device_generator(self, indent=0):
        self.opts['memory_devices'].set_l2_ram_ext_managed(self.opts['l2_ram_ext_managed'])
        self.opts['memory_devices'].set_l3_ram_ext_managed(self.opts['l3_ram_ext_managed'])
        self.opts['memory_devices'].set_l3_flash_ext_managed(self.opts['l3_flash_ext_managed'])

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
        code_block = CodeBlock(starting_indent=indent + 1)
        num_locals = 0
        edges = set(edge.params for edge in self.G.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        for eparams in sorted_edges:
            # check if the following real node is an output
            if isinstance(eparams.creating_node, ConcatParameters):
                rout_edge = self.real_down_connection(self.G, eparams)
                if isinstance(rout_edge.to_node, OutputParameters):
                    rout_eparams = rout_edge.params
                    cname = self.naming_convension.get_edge_name(rout_eparams.creating_node.name,
                                                                 rout_eparams.creating_step,
                                                                 rout_eparams.edge_type,
                                                                 rout_eparams.edge_order)
                    LOG.info("edge from step %s %s is not used and is replaced with edge to step %s:%s %s cname: %s",
                             eparams.creating_node.step_idx, eparams.creating_node.name,
                             rout_eparams.creating_node.name, rout_eparams.creating_node.step_idx,
                             rout_eparams.creating_step, cname)
                    self.name_cache[eparams] = {'edge': cname}
                    continue

            rin_eparams, set_real = self.real_up_connection(self.G, eparams)
            if set_real:
                # Code will not be generated for reshape or empty transpose so the input to the
                # following node is the input to this node
                cname = self.naming_convension.get_edge_name(rin_eparams.creating_node.name,
                                                             rin_eparams.creating_step,
                                                             rin_eparams.edge_type,
                                                             rin_eparams.edge_order)
                LOG.info("edge from step %s %s is not used and is replaced with edge from step %s:%s %s cname: %s",
                         eparams.creating_node.step_idx, eparams.creating_node.name,
                         rin_eparams.creating_node.name, rin_eparams.creating_node.step_idx,
                         rin_eparams.creating_step, cname)
                self.name_cache[eparams] = {'edge': cname}
                continue
            cname = self.naming_convension.get_edge_name(eparams.creating_node.name,
                                                         eparams.creating_step,
                                                         eparams.edge_type,
                                                         eparams.edge_order)

            out_q = self.G.quantization[NodeId(eparams.creating_node, None)]\
                .out_qs[eparams.creating_node_idx]
            self.name_cache[eparams] = {'edge': cname}
            if eparams.edge_type != "in_out" or eparams.is_alias:
                continue
            if num_locals != 0:
                code_block.append_last(',')
            gen_local_decl(eparams, out_q, self.opts['default_local_location'], code_block)
            num_locals += 1
        code_block.deindent()
        code_block.write_start("CArgs({},", num_locals)
        code_block.write(")")
        return str(code_block)

    def stack_generator(self, indent=0):
        if self.opts['at_ver'] < 2:
            LOG.warning("AutoTiler version is less than 3. Stacked tensors are not supported.")
        edges = set(edge.params for edge in self.G.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        concat_edges = list([eparams for eparams in sorted_edges if isinstance(
            eparams.creating_node, ConcatParameters)])
        code_block = CodeBlock(starting_indent=indent)
        if len(concat_edges) == 0:
            code_block.comment("no concats in graph so not stacked tensors created")
        for eparams in concat_edges:
            node = eparams.creating_node
            cname_out = self.name_cache[eparams]['edge']
            in_edge_names = [self.name_cache[edge.params]['edge']
                             for edge in self.G.in_edges(node.name)]
            gen_stack_decl(cname_out, in_edge_names, code_block)
        return str(code_block)

    def global_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent + 1)

        num_globals = self.generate_inputs(code_block)
        num_globals = self.generate_constants(num_globals, code_block)
        num_globals = self.generate_outputs(num_globals, code_block)

        code_block.deindent()
        code_block.write_start("CArgs({},", num_globals)
        code_block.write(")")
        return str(code_block)

    def generate_outputs(self, num_globals, code_block):
        outputs = set()
        for node in self.G.output_nodes():
            in_qs = self.G.quantization[NodeId(node)].in_qs
            for edge in self.G.in_edges(node.name):
                eparams = edge.params
                if eparams in outputs:
                    continue
                outputs.add(eparams)
                if num_globals != 0:
                    code_block.append_last(',')
                gen_output_decl(eparams,
                                in_qs[edge.to_idx],
                                self.opts['default_output_home_location'],
                                self.opts['default_output_exec_location'],
                                code_block,
                                allocate=node.at_options.allocate)
                num_globals += 1
        return num_globals

    def generate_constants(self, num_globals, code_block):
        for step_idx, pnode, _, fnode in self.G.nodes_iterator():
            anode = pnode if not fnode else fnode
            if isinstance(anode, FilterParameters):
                qrec = self.G.quantization[NodeId(pnode, fnode)]
                cname = self.naming_convension.\
                    get_global_name(pnode.name, step_idx, pnode, "weights")
                c_entry = self.name_cache.get(anode)
                if not c_entry:
                    c_entry = {}
                    self.name_cache[anode] = c_entry
                c_entry['weights'] = cname
                if num_globals != 0:
                    code_block.append_last(',')
                const_info = gen_const_info(os.path.join(self.opts['tensor_directory'],
                                                         cname+".tensor"), qrec.weights_q)
                gen_global_decl(cname, qrec.weights_q,
                                self.opts['default_global_home_location'],
                                self.opts['default_global_exec_location'],
                                code_block,
                                const_info=const_info)
                num_globals += 1

                # biases are always generated even if they are 0
                if anode.has_bias:
                    biases_q = qrec.biases_q
                else:
                    biases_q = qrec.out_q

                cname = self.naming_convension.\
                    get_global_name(pnode.name, step_idx, pnode, "biases")
                c_entry['biases'] = cname
                if num_globals != 0:
                    code_block.append_last(',')
                const_info = gen_const_info(os.path.join(self.opts['tensor_directory'],
                                                         cname+".tensor"), biases_q)
                gen_global_decl(cname, biases_q,
                                self.opts['default_global_home_location'],
                                self.opts['default_global_exec_location'],
                                code_block,
                                const_info=const_info)
                num_globals += 1

                if isinstance(anode, MultiplicativeBiasParameters) and anode.has_mul_bias:
                    mul_biases_q = qrec.mul_biases_q

                    cname = self.naming_convension.get_global_name(pnode.name, step_idx,
                                                                   pnode, "mul_biases")
                    c_entry['mul_biases'] = cname
                    if num_globals != 0:
                        code_block.append_last(',')
                    const_info = gen_const_info(os.path.join(self.opts['tensor_directory'],
                                                             cname+".tensor"), mul_biases_q)
                    gen_global_decl(cname, mul_biases_q,
                                    self.opts['default_global_home_location'],
                                    self.opts['default_global_exec_location'],
                                    code_block,
                                    const_info=const_info)
                    num_globals += 1
            elif isinstance(anode, ConstantInputParameters):
                qrec = self.G.quantization[NodeId(pnode, fnode)]
                # the name cache will be updated when all the edges are analysed by local_generator
                # the name of the constant is attached to the output edge so find it
                out_edge = self.G.out_edges(pnode.name)[0]
                eparams = out_edge.params
                cname = self.naming_convension.get_edge_name(eparams.creating_node.name,
                                                             eparams.creating_step,
                                                             eparams.edge_type,
                                                             eparams.edge_order)
                if num_globals != 0:
                    code_block.append_last(',')
                const_info = gen_const_info(os.path.join(self.opts['tensor_directory'],
                                                         cname+".tensor"), qrec.out_qs[0])
                gen_global_decl(cname, qrec.out_qs[0],
                                self.opts['default_global_home_location'],
                                self.opts['default_global_exec_location'],
                                code_block,
                                const_info=const_info)
                num_globals += 1
        return num_globals

    def generate_inputs(self, code_block):
        num_globals = 0
        inputs = set()
        for node in self.G.input_nodes():
            out_qs = self.G.quantization[NodeId(node)].out_qs
            for edge in self.G.out_edges(node.name):
                eparams = edge.params
                if eparams in inputs:
                    continue
                if num_globals != 0:
                    code_block.append_last(',')
                inputs.add(eparams)
                gen_input_decl(eparams,
                               out_qs[edge.from_idx],
                               self.opts['default_input_home_location'],
                               self.opts['default_input_exec_location'],
                               code_block,
                               allocate=node.at_options.allocate)
                num_globals += 1
        return num_globals

    def extra_includes_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        return str(code_block)

    def kernel_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        dump_input = False
        at_ver = self.opts['at_ver']
        for step_idx, node, _, fnode in self.G.nodes_iterator():
            if fnode:
                continue
            name = node.name
            cname = self.naming_convension.get_node_name(name, step_idx, node)
            if node.at_options.vcd_trace_on is not None:
                self.add_vcd_trace_binding(cname, node.at_options.vcd_trace_on)
            self.name_cache[node] = {'node': cname}
            if not isinstance(node, (InputParameters, OutputParameters,
                                     ConcatParameters, ReshapeParameters)):
                code_block.comment("generator for {}", name)
            in_eparams = self.G.get_in_params(name)
            out_eparams = self.G.get_out_params(name)
            try:
                qrec = self.G.quantization[NodeId(node)]
            except KeyError as err:
                LOG.error("Quantization record not found for node %s", node.name)
                raise err

            if isinstance(node, Conv2DParameters):
                self.set_conv_bindings(step_idx, in_eparams, out_eparams, cname, node, qrec,
                                       dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                gen_conv_pool_relu(cname, node, qrec, None,
                                   None, None, None, code_block=code_block, at_ver=at_ver,
                                   gen_ctrl=node.get_gen_ctrl())
            elif isinstance(node, PoolingParameters):
                self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_conv_pool_relu(cname, None, None,
                                   node, qrec, None, None, code_block=code_block, at_ver=at_ver,
                                   gen_ctrl=node.get_gen_ctrl())
            elif isinstance(node, ActivationParameters):
                self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_conv_pool_relu(cname, None, None,
                                   None, None, node, qrec, code_block=code_block, at_ver=at_ver,
                                   gen_ctrl=node.get_gen_ctrl())
            elif isinstance(node, FcParameters):
                self.set_fc_bindings(step_idx, in_eparams, out_eparams, cname, node, qrec,
                                     dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                gen_linear_relu(cname, node, qrec, None, None, code_block=code_block,
                                at_ver=at_ver, gen_ctrl=node.get_gen_ctrl())
            elif isinstance(node, GlobalPoolParameters):
                self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_globalpool(cname, node, qrec, code_block=code_block, at_ver=at_ver)
            elif isinstance(node, SoftMaxParameters):
                self.set_softmax_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_softmax(cname, node, qrec, code_block=code_block, at_ver=at_ver)
            elif isinstance(node, ConvFusionParameters):
                cnodes = node.contained_nodes()
                quants = [self.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
                if node.fusion_type == "conv_active_pool":
                    self.set_conv_bindings(step_idx, in_eparams, out_eparams,
                                           cname, cnodes[0], quants[0], out_q=quants[1],
                                           dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], cnodes[2], quants[2],
                                       cnodes[1], quants[1], code_block=code_block, at_ver=at_ver,
                                       gen_ctrl=node.get_gen_ctrl())
                elif node.fusion_type == "conv_pool_active":
                    self.set_conv_bindings(step_idx, in_eparams, out_eparams,
                                           cname, cnodes[0], quants[0], out_q=quants[2],
                                           dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], cnodes[1], quants[1],
                                       cnodes[2], quants[2], code_block=code_block, at_ver=at_ver,
                                       gen_ctrl=node.get_gen_ctrl())
                elif node.fusion_type == "conv_active":
                    self.set_conv_bindings(step_idx, in_eparams, out_eparams,
                                           cname, cnodes[0], quants[0], out_q=quants[1],
                                           dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], None, None, cnodes[1],
                                       quants[1], code_block=code_block, at_ver=at_ver,
                                       gen_ctrl=node.get_gen_ctrl())
                elif node.fusion_type == "conv_pool":
                    self.set_conv_bindings(step_idx, in_eparams, out_eparams,
                                           cname, cnodes[0], quants[0], out_q=quants[1],
                                           dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], cnodes[1], quants[1], None,
                                       None, code_block=code_block, at_ver=at_ver, gen_ctrl=node.get_gen_ctrl())
                elif node.fusion_type == "linear_active":
                    self.set_fc_bindings(step_idx, in_eparams, out_eparams,
                                         cname, cnodes[0], quants[0], out_q=quants[1],
                                         dump_tensors=self.opts['dump_tensors'] or node.at_options.dump_tensors)
                    gen_linear_relu(cname, cnodes[0], quants[0],
                                    cnodes[1], quants[1], code_block=code_block, at_ver=at_ver,
                                    gen_ctrl=node.get_gen_ctrl())
                elif node.fusion_type == "pool_active":
                    self.set_in_out_bindings(in_eparams, out_eparams, cname,
                                             node, qrec, out_q=quants[1])
                    gen_pool_relu(cname, cnodes[0], quants[0],
                                  cnodes[1], quants[1], code_block=code_block, at_ver=at_ver,
                                  gen_ctrl=node.get_gen_ctrl())
                else:
                    raise NotImplementedError("this fusion type is not implemented")
            elif isinstance(node, MatScaleFusionParameters):
                if at_ver < 3:
                    raise NotImplementedError(
                        "matscale not imlemented before version 3 of AUtoTiler")
                self.set_matscale_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_matscale(cname, node, qrec, code_block=code_block)
            elif isinstance(node, MatrixAddParameters):
                self.set_matrixadd_bindings(in_eparams, out_eparams, cname, node, qrec)
                if qrec.in_qs[0].q == qrec.in_qs[1].q and qrec.in_qs[0].q == qrec.out_qs[0].q:
                    gen_matrixadd(cname, node, qrec, code_block=code_block, at_ver=at_ver)
                else:
                    gen_matrixadddyn(cname, node, qrec, code_block=code_block, at_ver=at_ver)
            elif isinstance(node, ReshapeParameters):
                if node.transpose_in is not None or node.transpose_out is not None:
                    LOG.error("Don't know how to generate kernel \
                        for a reshape that has a transpose.")
                    return ""
                continue
            elif isinstance(node, TransposeParameters):
                if node.transpose_dimension == 1:
                    continue
                if node.transpose_dimension < 2 or node.transpose_dimension > 3:
                    raise NotImplementedError("only 2D or 3D transposes are currently supported")
                code_block.comment("transpose from {} to {}", node.in_dims[0], node.out_dims[0])
                self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_3d_transpose(cname, node, qrec, code_block=code_block)
            elif isinstance(node, (InputParameters, ConstantInputParameters)):
                if self.opts['dump_tensors'] or node.at_options.dump_tensors:
                    dump_input = True
                continue
            elif isinstance(node, (OutputParameters)):
                continue
            elif not isinstance(node, (ConcatParameters)):
                LOG.error("Don't know how to generate kernel for parameter type %s. \
                    Perhaps you need to run some fusions.", node.__class__.__name__)
                return ""
            # if self.opts['generate_checksums']:
            #     if last_node_was_input:
            #         self.add_checksum_binding(cname, name, step_idx, in_eparams, True)
            #     self.add_checksum_binding(cname, name, step_idx, out_eparams, False)
            if dump_input:
                self.add_dump_tensors_binding(cname, name, step_idx, in_eparams, qrec, True)
                dump_input = False
            if self.opts['dump_tensors'] or node.at_options.dump_tensors:
                self.add_dump_tensors_binding(cname, name, step_idx, out_eparams, qrec, False)
        return str(code_block)

    def add_vcd_trace_binding(self, cname, enable):
        self.func_bindings.append(
            FunctionBindingList(cname,
                                "gv_vcd_configure",
                                Imm(1 if enable else 0),
                                Imm(0),
                                before=True))

    def add_dump_tensors_binding(self, cname, name, step_idx, eparams, qrec, is_input):
        node = self.G[name]
        if is_input:
            dims = node.in_dims[0]
            qtype = qrec.in_qs[0]
            tensor_type = TT_TENSOR_TYPES['TT_INPUT']
            step_idx = self.G.in_edges(name)[0].from_node.step_idx
        else:
            dims = node.out_dims[0]
            qtype = qrec.out_qs[0]
            tensor_type = TT_TENSOR_TYPES['TT_OUTPUT']

        self.func_bindings.append(
            FunctionBindingList(cname,
                                "dt_write_tensor",
                                GArgEdge(eparams[0]),
                                Imm(step_idx),
                                Imm(tensor_type),
                                Imm(dims.size()),
                                Imm(qtype.bits),
                                Imm(len(dims.shape)),
                                *[Imm(v) for v in dims.shape],
                                before=is_input))

    def add_dump_params_binding(self, cname, node: FilterParameters, qrec, step_idx):
        dims = node.filter
        qtype = qrec.weights_q
        tensor_type = TT_TENSOR_TYPES['TT_WEIGHTS']
        self.func_bindings.append(
            FunctionBindingList(cname,
                                "dt_write_tensor",
                                GArgNode(node, 'weights'),
                                Imm(step_idx),
                                Imm(tensor_type),
                                Imm(dims.size()),
                                Imm(qtype.bits),
                                Imm(len(dims.actual_shape)),
                                *[Imm(v) for v in dims.actual_shape],
                                before=True))
        if node.has_bias:
            qtype = qrec.biases_q
            tensor_type = TT_TENSOR_TYPES['TT_BIASES']
            self.func_bindings.append(
                FunctionBindingList(cname,
                                    "dt_write_tensor",
                                    GArgNode(node, 'biases'),
                                    Imm(step_idx),
                                    Imm(tensor_type),
                                    Imm(node.out_dims[0].c),
                                    Imm(qtype.bits),
                                    Imm(1),
                                    Imm(node.out_dims[0].c),
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

    def set_in_out_bindings(self, in_eparams, out_eparams, cname, node, node_q, out_q=None):
        if out_q is None:
            out_q = node_q
        self.bindings.append(
            CommentBindingList("Node {} inq {} outq {}", node.name,
                               node_q.in_qs[0].q, out_q.out_qs[0].q)
        )
        self.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))

    def set_matrixadd_bindings(self, in_eparams, out_eparams, cname, params, node_q):
        self.bindings.append(
            CommentBindingList("Node {} inq1 {} inq2 {} outq {}", params.name,
                               node_q.in_qs[0].q, node_q.in_qs[1].q, node_q.out_qs[0].q)
        )
        self.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))

    def set_matscale_bindings(self, in_eparams, out_eparams, cname, params, node_q):
        if self.opts['at_ver'] < 3:
            raise NotImplementedError("matscale is only implemented in AutoTiler v3")
        if params.fusion_type == "vec_scalar":
            self.bindings.append(
                CommentBindingList("Node {} inq1 {} inq2 {} inq3 {} outq {}", params.name,
                                   node_q.in_qs[0].q, node_q.in_qs[1].q, node_q.in_qs[2].q, node_q.out_qs[0].q)
            )
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[1]), GNodeArgEdge(in_eparams[2]),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT")))
        else:
            self.bindings.append(
                CommentBindingList("Node {} inq1 {} inq2 {} outq {}", params.name,
                                   node_q.in_qs[0].q, node_q.in_qs[1].q, node_q.out_qs[0].q)
            )
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[1]),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT")))

    def set_softmax_bindings(self, in_eparams, out_eparams, cname, params, node_q):
        self.bindings.append(
            CommentBindingList("Node {} inq {} outq {}", params.name,
                               node_q.in_qs[0].q, node_q.out_qs[0].q)
        )
        if self.opts['at_ver'] > 2:
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT")))
        else:
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                                Imm(node_q.in_qs[0].q)))

    def set_conv_bindings(self, step_idx, in_eparams, out_eparams, cname, params, conv_q,
                          out_q=None, dump_tensors=False):
        if out_q is None:
            out_q = conv_q
        self.bindings.append(
            CommentBindingList("Node {} inq {} weightsq {} outq {} biasesq {}", cname,
                               conv_q.in_qs[0].q, conv_q.weights_q.q, out_q.out_qs[0].q, conv_q.biases_q.q)
        )
        if self.opts['at_ver'] > 2:
            if params.has_mul_bias:
                self.bindings.append(
                    NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                                    GNodeArgNode(params, 'biases'), GNodeArgNode(params, 'mul_biases'),
                                    GNodeArgEdge(out_eparams[0], "GNA_OUT")))
            else:
                self.bindings.append(
                    NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                                    GNodeArgNode(params, 'biases'),
                                    GNodeArgEdge(out_eparams[0], "GNA_OUT")))
        else:
            norm = conv_q.in_qs[0].q + conv_q.weights_q.q - out_q.out_qs[0].q
            normbias = conv_q.in_qs[0].q + conv_q.weights_q.q - \
                out_q.out_qs[0].q * 2 + conv_q.biases_q.q
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                                GNodeArgNode(params, 'biases'),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                                Imm(norm),
                                Imm(normbias)))
        if dump_tensors:
            self.add_dump_params_binding(cname, params, conv_q, step_idx)

    def set_fc_bindings(self, step_idx, in_eparams, out_eparams, cname,
                        params, linear_q, out_q=None, dump_tensors=False):
        if out_q is None:
            out_q = linear_q
        self.bindings.append(
            CommentBindingList("Node {} inq {} weightsq {} outq {}", params.name,
                               linear_q.in_qs[0].q, linear_q.weights_q.q, out_q.out_qs[0].q)
        )
        if self.opts['at_ver'] > 2:
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                                GNodeArgNode(params, 'biases'),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT")))
        else:
            self.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                                GNodeArgNode(params, 'biases'),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                                Imm(linear_q.in_qs[0].q +
                                    linear_q.weights_q.q - linear_q.out_qs[0].q),
                                Imm(linear_q.in_qs[0].q + linear_q.weights_q.q - linear_q.biases_q.q)))
        if dump_tensors:
            self.add_dump_params_binding(cname, params, linear_q, step_idx)

    def write_constants(self):
        write_constants(self.G, self.naming_convension, self.opts['tensor_directory'])

    def header_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for step_idx, node, _, fnode in self.G.nodes_iterator():
            if fnode:
                continue
            if not isinstance(node, InputParameters) and not isinstance(node, OutputParameters):
                continue
            name = node.name
            cname = self.naming_convension.get_node_name(name, step_idx, node)
            qrec = self.G.quantization[NodeId(node)]
            for i, out_q in enumerate(qrec.out_qs):
                code_block.write("#define {}_Q\t{}".format(cname, out_q.q))
        return str(code_block)