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

from abc import ABC, abstractmethod
from graph.types import EdgeParameters

TT_TENSOR_TYPES = {
    'TT_INPUT': 0,
    'TT_OUTPUT': 1,
    'TT_WEIGHTS': 2,
    'TT_BIASES': 3
}


def gen_gnode_arg(direction, name, alias=None):
    if direction == "GNA_INOUT":
        return 'GNodeArg({}, "{}", "{}")'.format(direction, name, alias or name)
    return 'GNodeArg({}, "{}", 0)'.format(direction, name)


def gen_g_arg(name):
    return 'GArg("{}")'.format(name)


def gen_g_node_c_arg(name):
    return 'GNodeCArg("{}")'.format(name)


def gen_imm_arg(symbol):
    return "Imm({})".format(symbol)


def gen_at_bindings(name, binding_list, code_block):
    code_block.write('AddNode("{0}", Bindings({1}, {2}));'
                     .format(name, len(binding_list), ", ".join(binding_list)))


def gen_at_func_bindings(name, func_name, where, binding_list, code_block):
    code_block.write('AddCallToNode("{0}", {1}, "{2}", Bindings({3}, {4}));'
                     .format(name, where, func_name, len(binding_list), ", ".join(binding_list)))


class Binding(ABC):
    @abstractmethod
    def gen_binding(self, generator):
        pass

# pylint: disable=abstract-method


class InfoListName(Binding):
    def __init__(self, cname):
        self.cname = cname

    def gen_binding(self, generator):
        return "{}_infos".format(self.cname)


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

class GArgName(Binding):
    def __init__(self, ename):
        self.ename = ename

    def gen_binding(self, generator):
        return gen_g_node_c_arg(self.ename)


class GNodeArgEdge(GNodeArg):
    def __init__(self, name, direction="GNA_IN", alias=None):
        super().__init__(direction)
        self.name = name
        self.alias = alias

    def gen_binding(self, generator):
        if isinstance(self.name, EdgeParameters):
            name = generator.get_edge_name(self.name)
        else:
            name = self.name
        if self.alias:
            if isinstance(self.alias, EdgeParameters):
                alias = generator.get_edge_name(self.alias)
            else:
                alias = self.alias
        else:
            alias = None
        return gen_gnode_arg(self.direction, name, alias=alias)

class NoArg(Binding):
    def gen_binding(self, generator):
        return "AT_NO_ARG_BINDING"

class GNodeArgNode(GNodeArg):
    def __init__(self, node, target, direction="GNA_IN"):
        super().__init__(direction)
        self.node = node
        self.target = target

    def gen_binding(self, generator):
        return gen_gnode_arg(self.direction, generator.get_node_name(self.node, self.target))

class GNodeArgNamed(GNodeArg):
    def __init__(self, name, alias=None, direction="GNA_IN"):
        super().__init__(direction)
        self.name = name
        self.alias = alias

    def gen_binding(self, generator):
        return gen_gnode_arg(self.direction, self.name, alias=self.alias)


class Imm(Binding):
    def __init__(self, symbol):
        self.symbol = symbol

    def gen_binding(self, generator):
        return gen_imm_arg(self.symbol)


class BindingList(ABC):
    @abstractmethod
    def gen_bindings(self, generator, code_block):
        pass


class InfosList(BindingList):
    def __init__(self, cname, infos):
        self.cname = cname
        self.infos = infos

    def gen_bindings(self, _, code_block):
        code_block.write("char {}_infos[] = {{{}}};".format(self.cname, ", ".join(self.infos)))


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
