# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
import os

from utils.node_id import NodeId

from graph.types import (ActivationParameters, Conv2DParameters, FcParameters,
                         FilterParameters, FusionParameters, InputParameters,
                         PoolingParameters, SoftMaxParameters, OutputParameters)
from .bindings import (TT_TENSOR_TYPES, CommentBindingList,
                       FunctionBindingList, GArgEdge, GArgNode, GNodeArgEdge,
                       GNodeArgNode, Imm, NodeBindingList)
from .checksums import calc_value_checksum, checksum_func
from .code_block import CodeBlock
from .code_generators import (gen_const_info, gen_conv_pool_relu,
                              gen_global_decl, gen_input_decl, gen_linear_relu,
                              gen_local_decl, gen_output_decl, gen_softmax)
from .memory_device_info import MemoryDeviceInfos
from .write_constants import write_constants

LOG = logging.getLogger("nntool." + __name__)

DEFAULT_GEN_OPTS = {
    'default_input_home_location': 'AT_MEM_L2',
    'default_input_exec_location': 'AT_MEM_L2',
    'default_output_home_location': 'AT_MEM_L2',
    'default_output_exec_location': 'AT_MEM_L2',
    'default_global_home_location': 'AT_MEM_L3_HFLASH',
    'default_global_exec_location': 'AT_MEM_UNDEF',
    'default_local_location': 'AT_MEM_UNDEF',
    'generate_checksums': False,
    'include_project_header': False,
    'checksum_file': '',
    'dump_tensors': False,
    'tensor_directory': '.',
    'model_directory': '.',
    'model_file': 'model.c',
    'memory_devices': MemoryDeviceInfos.default()
}

DEFAULT_GEN_OPTS_DESCRIPTIONS = {
    'default_input_home_location': 'default home location for inputs for code generation',
    'default_input_exec_location': 'default exec location for inputs for code generation',
    'default_output_home_location': 'default home location for outputs for code generation',
    'default_output_exec_location': 'default exec location for outputs for code generation',
    'default_global_home_location': 'default home location for globals for code generation',
    'default_global_exec_location': 'default exec location for globals for code generation',
    'default_local_location': 'default location for locals for code generation',
    'include_project_header': 'Include a header file called "GraphName.h" in generated code',
    'tensor_directory': 'directory to dump tensors to',
    'model_directory': 'directory to dump model to',
    'model_file': 'filename for model',
    'dump_tensors': 'write the tensors to files. currently only works in emulation mode.'
}

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
        if self.opts['dump_tensors']:
            self.include_files.append('helpers.h')

    @property
    def project_name(self):
        return self.naming_convension.get_project_name()

    def get_edge_name(self, eparams):
        return self.name_cache[eparams]['edge']

    def get_node_name(self, params, target):
        return self.name_cache[params][target]

    def memory_device_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        self.opts['memory_devices'].gen(self.G, code_block)
        return str(code_block)

    def binding_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        for binding in self.bindings + self.func_bindings:
            binding.gen_bindings(self, code_block)
        return str(code_block)

    def local_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent + 1)
        num_locals = 0
        edges = set(edge.params for edge in self.G.edges())
        sorted_edges = list(edges)
        sorted_edges.sort(key=lambda eparams: eparams.creating_step)
        for eparams in sorted_edges:
            cname = self.naming_convension.get_edge_name(eparams.creating_node.name,
                                                         eparams.creating_step,
                                                         eparams.edge_type,
                                                         eparams.edge_order)
            out_q = self.G.quantization[NodeId(eparams.creating_node, None)]\
                .out_qs[eparams.creating_node_idx]
            self.name_cache[eparams] = {'edge': cname}
            if eparams.edge_type != "in_out":
                continue
            if num_locals != 0:
                code_block.append_last(',')
            gen_local_decl(eparams, out_q, self.opts['default_local_location'], code_block)
            num_locals += 1
        code_block.deindent()
        code_block.write_start("CArgs({},", num_locals)
        code_block.write(")")
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
        for node in self.G.outputs():
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
                                code_block)
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
        return num_globals

    def generate_inputs(self, code_block):
        num_globals = 0
        inputs = set()
        for node in self.G.inputs():
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
                               code_block)
                num_globals += 1
        return num_globals

    def kernel_generator(self, indent=0):
        code_block = CodeBlock(starting_indent=indent)
        last_node_was_input = False
        for step_idx, node, _, fnode in self.G.nodes_iterator():
            if fnode:
                continue
            name = node.name
            cname = self.naming_convension.get_node_name(name, step_idx, node)
            self.name_cache[node] = {'node': cname}
            code_block.comment(name)
            in_eparams = self.G.get_in_params(name)
            out_eparams = self.G.get_out_params(name)
            qrec = self.G.quantization[NodeId(node)]
            if isinstance(node, Conv2DParameters):
                self.set_conv_bindings(step_idx, in_eparams, out_eparams, cname, node, qrec)
                gen_conv_pool_relu(cname, node, qrec, None,
                                   None, None, None, code_block=code_block)
            elif isinstance(node, PoolingParameters):
                self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_conv_pool_relu(cname, None, None,
                                   node, qrec, None, None, code_block=code_block)
            elif isinstance(node, ActivationParameters):
                self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_conv_pool_relu(cname, None, None,
                                   None, None, node, qrec, code_block=code_block)
            elif isinstance(node, FcParameters):
                self.set_fc_bindings(step_idx, in_eparams, out_eparams, cname, node, qrec)
                gen_linear_relu(cname, node, qrec, None, None, code_block=code_block)
            elif isinstance(node, SoftMaxParameters):
                self.set_softmax_bindings(in_eparams, out_eparams, cname, node, qrec)
                gen_softmax(cname, node, qrec, code_block=code_block)
            elif isinstance(node, FusionParameters):
                cnodes = node.contained_nodes()
                quants = [self.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
                self.set_conv_bindings(step_idx, in_eparams, out_eparams, cname, cnodes[0], quants[0])
                if node.fusion_type == "conv_active_pool":
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], cnodes[2], quants[2],
                                       cnodes[1], quants[1], code_block=code_block)
                elif node.fusion_type == "conv_pool_active":
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], cnodes[1], quants[1],
                                       cnodes[2], quants[2], code_block=code_block)
                elif node.fusion_type == "conv_active":
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], None, None, cnodes[1],
                                       quants[1], code_block=code_block)
                elif node.fusion_type == "conv_pool":
                    gen_conv_pool_relu(cname, cnodes[0], quants[0], cnodes[1], quants[1], None,
                                       None, code_block=code_block)
            elif not isinstance(node, (InputParameters, OutputParameters)):
                LOG.error("Don't know how to generate kernel for parameter type %s. Perhaps you need to run some fusions.", node.__class__.__name__)
                return ""
            else:
                last_node_was_input = True
                continue
            if self.opts['generate_checksums']:
                if last_node_was_input:
                    self.add_checksum_binding(cname, name, step_idx, in_eparams, True)
                self.add_checksum_binding(cname, name, step_idx, out_eparams, False)
            if self.opts['dump_tensors']:
                if last_node_was_input:
                    self.add_dump_tensors_binding(cname, name, step_idx, in_eparams, qrec, True)
                self.add_dump_tensors_binding(cname, name, step_idx, out_eparams, qrec, False)
            last_node_was_input = False
        return str(code_block)

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

    def set_in_out_bindings(self, in_eparams, out_eparams, cname, node, node_q):
        self.bindings.append(
            CommentBindingList("Node {} inq {} outq {}", node.name,
                               node_q.in_qs[0].q, node_q.out_qs[0].q)
        )
        self.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))

    def set_softmax_bindings(self, in_eparams, out_eparams, cname, params, node_q):
        self.bindings.append(
            CommentBindingList("Node {} inq {} outq {}", params.name,
                               node_q.in_qs[0].q, node_q.out_qs[0].q)
        )
        self.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            Imm(node_q.in_qs[0].q)))

    def set_conv_bindings(self, step_idx, in_eparams, out_eparams, cname, params, conv_q):
        self.bindings.append(
            CommentBindingList("Node {} inq {} weightsq {} outq {}", cname,
                               conv_q.in_qs[0].q, conv_q.weights_q.q, conv_q.out_qs[0].q)
        )
        self.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                            GNodeArgNode(params, 'biases'),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            Imm(conv_q.in_qs[0].q + conv_q.weights_q.q - conv_q.out_qs[0].q)))
        if self.opts['dump_tensors']:
            self.add_dump_params_binding(cname, params, conv_q, step_idx)

    def set_fc_bindings(self, step_idx, in_eparams, out_eparams, cname, params, linear_q):
        self.bindings.append(
            CommentBindingList("Node {} inq {} weightsq {} outq {}", params.name,
                               linear_q.in_qs[0].q, linear_q.weights_q.q, linear_q.out_qs[0].q)
        )
        self.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgNode(params, 'weights'),
                            GNodeArgNode(params, 'biases'),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            Imm(linear_q.in_qs[0].q + linear_q.weights_q.q - linear_q.out_qs[0].q),
                            Imm(linear_q.in_qs[0].q + linear_q.weights_q.q - linear_q.biases_q.q)))
        if self.opts['dump_tensors']:
            self.add_dump_params_binding(cname, params, linear_q, step_idx)

    def write_constants(self):
        write_constants(self.G, self.naming_convension, self.opts['tensor_directory'])
