# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from abc import ABC, abstractmethod

from .code_generators import (gen_gnode_arg, gen_imm_arg, gen_at_bindings,
                              gen_at_func_bindings, gen_g_node_c_arg)

TT_TENSOR_TYPES = {
    'TT_INPUT': 0,
    'TT_OUTPUT': 1,
    'TT_WEIGHTS': 2,
    'TT_BIASES': 3
}

class Binding(ABC):
    @abstractmethod
    def gen_binding(self, generator):
        pass

# pylint: disable=abstract-method
class GNodeArg(Binding):
    def __init__(self, direction):
        self.direction = direction

class GArgEdge(Binding):
    def __init__(self, eparams):
        self.eparams = eparams

    def gen_binding(self, generator):
        return gen_g_node_c_arg(generator.get_edge_name(self.eparams))

class GArgNode(Binding):
    def __init__(self, node, target):
        self.node = node
        self.target = target

    def gen_binding(self, generator):
        return gen_g_node_c_arg(generator.get_node_name(self.node, self.target))

class GNodeArgEdge(GNodeArg):
    def __init__(self, eparams, direction="GNA_IN"):
        super().__init__(direction)
        self.eparams = eparams

    def gen_binding(self, generator):
        return gen_gnode_arg(self.direction, generator.get_edge_name(self.eparams))

class GNodeArgNode(GNodeArg):
    def __init__(self, node, target, direction="GNA_IN"):
        super().__init__(direction)
        self.node = node
        self.target = target

    def gen_binding(self, generator):
        return gen_gnode_arg(self.direction, generator.get_node_name(self.node, self.target))

class Imm(Binding):
    def __init__(self, symbol):
        self.symbol = symbol

    def gen_binding(self, generator):
        return gen_imm_arg(self.symbol)

class BindingList(ABC):
    @abstractmethod
    def gen_bindings(self, generator, code_block):
        pass

class CommentBindingList(BindingList):
    def __init__(self, fmt, *args, **kwargs):
        self.comment = fmt.format(*args, **kwargs)

    def gen_bindings(self, _, code_block):
        code_block.write("// {}".format(self.comment))

class NodeBindingList(BindingList):
    def __init__(self, node, *binding_list):
        self.node = node
        self.binding_list = binding_list

    def gen_bindings(self, generator, code_block):
        gen_at_bindings(self.node,
                        [binding.gen_binding(generator) for binding in self.binding_list],
                        code_block)

class FunctionBindingList(NodeBindingList):
    def __init__(self, node, func_name, *binding_list, before=False):
        super().__init__(node, *binding_list)
        self.func_name = func_name
        self.where = 'CALL_PROLOG' if before else 'CALL_EPILOG'

    def gen_bindings(self, generator, code_block):
        gen_at_func_bindings(self.node,
                             self.func_name,
                             self.where,
                             [binding.gen_binding(generator) for binding in self.binding_list],
                             code_block)

    