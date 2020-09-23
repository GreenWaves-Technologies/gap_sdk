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
from ctypes import POINTER, c_char, c_int, c_uint
from functools import reduce
from typing import Mapping, Sequence
from mpmath.functions.functions import arg

import numpy as np
from sympy import IndexedBase, Integer, Symbol
from sympy.codegen.ast import (Assignment, CodeBlock, Comment, Declaration,
                               FunctionCall, FunctionDefinition, Pointer,
                               QuotedString, SignedIntType, String,
                               Type, UnsignedIntType, Variable, int8, int32,
                               uint32)
from sympy.codegen.cnodes import restrict
from sympy.core import symbol
from sympy.core.function import count_ops

from generation.code_block import CodeBlock as NNToolCodeBlock
from graph.types.fusions import FusionInputParameters, FusionOutputParameters

from .expr_state import ExprState
from .sympy_codegen import (VOID, AssignmentEx, IndexedEx, Struct,
                            StructMember, StructVariable, TypeDef, VarFor,
                            VarRange, ccode)

LOG = logging.getLogger("nntool." + __name__)

CFUNC_DEFS = {'Min': 'gap_min', 'Max': 'gap_max',
              'sqrt': 'sqrt_17_15', 'log': 'logn_17_15', 'Pow': 'pow_17_15',
              'ATClip': 'gap_clip', 'ATNorm': 'AT_NORM'
              }

INDEX_NAMES = ['c', 'h', 'w']

TYPE_MAPPINGS = {
    SignedIntType(String('int8'), nbits=Integer(8)): 'signed char',
    SignedIntType(String('int32'), nbits=Integer(32)): 'int',
    UnsignedIntType(String('uint32'), nbits=Integer(32)): 'unsigned int'
}


def indexed_symbol(symbol, shape, idx_symbols):
    base = IndexedBase(symbol, shape=tuple(s for s in shape if s > 1), strides='C')
    index = tuple(idx_sym for idx, idx_sym in enumerate(idx_symbols) if shape[idx] > 1)
    return IndexedEx(base, *index)


def split_indexes(unique_axis_groups):
    res = set()
    uaq = sorted(unique_axis_groups, key=len)
    while uaq:
        first = uaq[0]
        res.add(first)
        rest = []
        for item in uaq[1::]:
            if first < item:
                rest.append(item - first)
            else:
                rest.append(item)
        uaq = sorted(rest, key=len)
    return res


def create_indexes(node_shapes):
    """Discovers iteration space for basic kernel. All 1 and uniformly indexed shapes are reduced"""
    shapes = np.array([list(val) for val in node_shapes.values()])
    # get the maximal shape which will be the out shape
    max_shape = tuple(shapes.max(axis=0))
    # get the index list of axis > 1 per input shape
    node_axis = {n: frozenset(np.nonzero(np.array(list(shape)) != 1)[
                              0]) for n, shape in node_shapes.items()}
    # get the unique variants
    unique_axis_groups = np.unique(np.array(list(node_axis.values())))
    # remove the empty set
    unique_axis_groups = np.delete(unique_axis_groups, np.where(unique_axis_groups == frozenset()))
    # reduce all axis that are supersets of others
    unique_indexes = sorted(split_indexes(unique_axis_groups), key=lambda x: next(i for i in x))
    idx_names = ["_".join(["d%s" % idx for idx in sorted(list(idxes))]) for idxes in unique_indexes]
    idx_dims = [reduce(lambda x, y: x*max_shape[y], idxes, 1) for idxes in unique_indexes]
    # The length is the number of iteration spaces for the kernel
    kernel_dims = len(unique_indexes)
    node_unique_indexes = {n: tuple([idx for idx, idx_set in enumerate(
        unique_indexes) if idx_set <= node_axis[n]]) for n in node_shapes}
    if kernel_dims == 1:
        ranges = ((Variable("First"), Variable("Last")),)
    elif kernel_dims == 2:
        ranges = (
            (Variable("First"), Variable("Last")),
            (Integer(0), Variable("W"))
        )
    elif kernel_dims == 3:
        ranges = (
            (Variable("First"), Variable("Last")),
            (Integer(0), Variable("H")),
            (Integer(0), Variable("W"))
        )
    else:
        raise ValueError("Too many kernel dimensions")
    return list(zip(idx_names, idx_dims, ranges)), node_unique_indexes, max_shape, kernel_dims


class BasicKernel():
    def __init__(self, fusion_inputs: Sequence[FusionInputParameters], fusion_outputs: Sequence[FusionOutputParameters],
                 expr_states: Sequence[ExprState], symbol_dim_map: Mapping[str, tuple], constant_inputs: Sequence[bool],
                 temporaries=None) -> None:
        self._inputs = fusion_inputs
        self._outputs = fusion_outputs
        self._constant_inputs = constant_inputs
        self._expr_states = expr_states
        self._symbol_dim_map = {Symbol(k): v for k, v in symbol_dim_map.items()}
        self._indexes, self._symbol_indexes, self._max_shape, self._kernel_dims = create_indexes(
            self._symbol_dim_map)
        self._output_symbols = [
            expr_state.symbol for expr_state in expr_states if not expr_state.is_intermediate]
        self._temporaries = temporaries
        if self._temporaries is None:
            self._temporaries = set(
                [expr_state.symbol for expr_state in expr_states if expr_state.is_intermediate])

    @property
    def max_shape(self):
        return self._max_shape

    @property
    def kernel_args(self):
        kernel_args = []
        for sym in ["H", "W"]:
            kernel_args.append((sym, "uint32"))
        if self._kernel_dims == 3:
            kernel_args.append(("InFeatures", "uint32"))
        for sym in self._symbol_dim_map:
            kernel_args.append((sym.name, "pint8"))
        for sym in self._output_symbols:
            kernel_args.append((sym.name, "pint8"))
        return kernel_args

    def gen_user_kernel(self, block: NNToolCodeBlock, ukname: str, kname: str):
        block.write("int {0}(char *Name) {{", ukname)
        block.indent()
        block.write("Kernel_T *Kernel = UserKernel(")
        block.indent()
        block.write("Name,")
        block.write("{0},", self.gen_iterspace())
        block.write("TILE_HOR,")
        cargs = self.gen_cargs()
        block.write("CArgs({0},", len(cargs))
        block.indent()
        for carg in cargs[:-1:]:
            block.write("{0},", carg)
        block.write("{0}", cargs[-1])
        block.deindent()
        block.write("),")
        block.write("Calls(1,")
        block.indent()
        block.write("Call(\"{0}\", LOC_LOOP,", kname)
        block.indent()
        bindings = self.gen_kerbingings()
        block.write("Bindings({0},", len(bindings))
        block.indent()
        for binding in bindings[:-1:]:
            block.write("{0},", binding)
        block.write("{0}", bindings[-1])
        block.deindent()
        block.write(")")
        block.deindent()
        block.write(")")
        block.deindent()
        block.write("),")
        kargs = self.gen_kargs()
        block.write("KerArgs({0},", len(cargs))
        block.indent()
        for karg in kargs[:-1:]:
            block.write("{0},", karg)
        block.write("{0}", kargs[-1])
        block.deindent()
        block.write(")")
        block.deindent()
        block.write(");")
        block.write("if (Kernel) {{")
        block.indent()
        for kinfo in self.gen_kinfos():
            block.write("{0};", kinfo)
        block.deindent()
        block.write("}}")
        block.write("return (Kernel!=0);")
        block.deindent()
        block.write("}}")

    def gen_kinfos(self):
        bandwidth = sum([np.prod(dim_tuple) for dim_tuple in self._symbol_dim_map.values()])
        bandwidth += len(self._outputs) * np.prod(self._max_shape)
        kinfos = [
            "AddKernelInfos(Name, AT_KERINFO_OPER, {0}, 0)".format(sum(expr_state.expr.count_ops()
                                                                       for expr_state in self._expr_states)),
            "AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, {0}, 0)".format(bandwidth)
        ]
        for node in self._inputs + self._outputs:
            shape = node.out_dims[0].shape
            kinfos.append("AddKernelArgDim(Name, \"{0}\", {1}, {2})".format(
                node.name, len(shape), ", ".join(str(dim) for dim in shape)))
        return kinfos

    def gen_cargs(self):
        cargs = []
        for node in self._inputs + self._outputs:
            cargs.append("TCArg(CNN_ArgDataType(1,1,1),\"{0}\")".format(node.name))
        return cargs

    def gen_kargs(self):
        kargs = []
        if len(self._indexes) == 3:
            argspaces = {
                tuple(): "KerArgSpace(1, KER_ITER_TILE0)",
                (0,): "KerArgSpace(1, KER_ITER_D0)",
                (1,): "KerArgSpace(1, KER_ITER_TILE0)",
                (2,): "KerArgSpace(1, KER_ITER_TILE0)",
                (0, 1): "KerArgSpace(2, KER_ITER_D0, KER_ITER_TILE0)",
                (0, 2): "KerArgSpace(2, KER_ITER_D0, KER_ITER_TILE0)",
                (1, 2): "KerArgSpace(1, KER_ITER_TILE0)",
                (0, 1, 2): "KerArgSpace(2, KER_ITER_D0, KER_ITER_TILE0)",
            }
            out_argspace = "KerArgSpace(2, KER_ITER_D0, KER_ITER_TILE0)"
            dim_w = 2
            dim_h = 1
        elif len(self._indexes) == 2:
            argspaces = {
                tuple(): "KerArgSpace(1, KER_ITER_TILE0)",
                (0,): "KerArgSpace(1, KER_ITER_TILE0)",
                (1,): "KerArgSpace(1, KER_ITER_TILE0)",
                (0, 1): "KerArgSpace(1, KER_ITER_TILE0)",
            }
            out_argspace = "KerArgSpace(1, KER_ITER_TILE0)"
            dim_w = 1
            dim_h = 0
        elif len(self._indexes) == 1:
            argspaces = {
                tuple(): "KerArgSpace(1, KER_ITER_TILE0)",
                (0,): "KerArgSpace(1, KER_ITER_TILE0)",
            }
            out_argspace = "KerArgSpace(1, KER_ITER_TILE0)"
            dim_w = -1
            dim_h = 0
        else:
            raise NotImplementedError()
        for input_node in self._inputs:
            sym = Symbol(input_node.name)
            arg_indexes = self._symbol_indexes[sym]
            argspace = argspaces[arg_indexes]
            constraints = "O_IN|O_DB|O_CONST" if self._constant_inputs[input_node.idx] else "O_IN|O_DB"
            dims = [index[1] if idx in arg_indexes else 1 for idx, index in enumerate(self._indexes)]
            width = dims[dim_w] if dim_w > -1 else 1
            height = dims[dim_h] if dim_h > -1 else 1
            kargs.append("KerArg(\"{0}\", {1}, {2}, {3}, {4}, 1, 0, 0, 0, \"{0}\")".format(
                         sym.name, argspace, constraints, height, width))
        dims = [index[1] for index in self._indexes]
        for output_node in self._outputs:
            name = output_node.name
            constraints = "O_OUT|O_DB"
            width = dims[dim_w] if dim_w > -1 else 1
            height = dims[dim_h] if dim_h > -1 else 1
            kargs.append("KerArg(\"{0}\", {1}, {2}, {3}, {4}, 1, 0, 0, 0, \"{0}\")".format(
                         name, out_argspace, constraints, height, width))
        return kargs

    def gen_iterspace(self):
        if len(self._indexes) == 3:
            return "KernelIterSpace(2, IterParSpace(KER_ITER_D0, {0}, 1), IterTiledSpace(KER_ITER_TILE0))".format(self._indexes[0][1])
        return "KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0))"

    def gen_kerbingings(self):
        bindings = [
            "K_Arg(\"{0}\", KER_ARG_TILE_W)".format(self._outputs[0].name),
            "K_Arg(\"{0}\", KER_ARG_TILE_H)".format(self._outputs[0].name)
        ]
        if self._kernel_dims == 3:
            bindings.append("K_ArgPar(\"{0}\", KER_ARG_PARTILE_SIZE, KER_ITER_D0)".format(
                self._outputs[0].name))
        for node in self._inputs + self._outputs:
            bindings.append("K_Arg(\"{0}\", KER_ARG_TILE)".format(node.name))
        return bindings

    def setup_codegen(self):
        args_var = Pointer("Args")
        block = self.kernel_arg_local_assign(args_var)
        if self._kernel_dims == 1:
            last_first = Variable("Sz", type=uint32)
            block.append(AssignmentEx(Declaration(last_first),
                                      Symbol("W") * Symbol("H")))
        elif self._kernel_dims == 2:
            last_first = Variable("H", type=uint32)
        elif self._kernel_dims == 3:
            last_first = Variable("InFeatures", type=uint32)
        else:
            raise ValueError("expression has too many dimensions")

        var_chunk = Variable("Chunk", type=uint32)
        var_first = Variable("First", type=uint32)
        var_last = Variable("Last", type=uint32)
        var_coreid = Variable("CoreId", type=uint32)
        # unsigned int CoreId = gap_coreid();
        block.append(AssignmentEx(Declaration(var_coreid), FunctionCall("gap_coreid", [])))
        # unsigned int Chunk = ChunkSize(OutFeatures);
        block.append(AssignmentEx(Declaration(var_chunk), FunctionCall("ChunkSize", [last_first])))
        # unsigned int First = Chunk*CoreId;
        block.append(AssignmentEx(Declaration(var_first), Symbol("Chunk") * Symbol("CoreId")))
        # unsigned int Last = Min(First+Chunk, OutFeatures);
        block.append(AssignmentEx(Declaration(var_last),
                                  FunctionCall("gap_min", [Symbol("First") + Symbol("Chunk"), last_first])))
        return block

    def gen_function(self, kernel_name, kernel_arg_type_name):
        main_block = []
        main_block.append(
            Comment("Output iteration space %s reduced to %s iteration spaces" % (self._max_shape, len(self._indexes))))
        # dereference all symbols that are not indexed
        dereference = [symbol for symbol in self._symbol_indexes if not self._symbol_indexes[symbol]]

        for expr_state in self._expr_states:
            free_symbols = [symbol for symbol in expr_state.expr.free_symbols
                            if not symbol in self._temporaries]
            # create substitutions for symbols that are indexed
            subs = [(symbol, indexed_symbol(symbol,
                                            [self._indexes[idx][1]
                                                for idx in self._symbol_indexes[symbol]],
                                            [self._indexes[idx][0] for idx in self._symbol_indexes[symbol]]))
                    for symbol in free_symbols
                    if self._symbol_indexes[symbol]]
            # indexed results. non indexed temps
            if not expr_state.is_intermediate:
                main_block.append(Comment("Produce output symbol %s" % expr_state.symbol))
                res_symbol = IndexedBase(expr_state.symbol, shape=tuple(
                    tuple(index[1] for index in self._indexes)))
                res_symbol = res_symbol[tuple(index[0] for index in self._indexes)]
            else:
                main_block.append(Comment("Produce intermediate symbol %s" % expr_state.symbol))
                res_symbol = expr_state.symbol
            main_block.append(Assignment(res_symbol, expr_state.expr.subs(subs)))

        # create all the for loops
        def func(block, idx):
            return [VarFor(Declaration(Variable(Symbol(idx[0]), type=uint32)),
                           VarRange(idx[2][0], idx[2][1], Integer(1)), block)]
        for_block = reduce(func, self._indexes[::-1], main_block)[0]
        # Create the initial code with args assignments and paralellization calcluation
        ast = self.setup_codegen()
        # declare temporaries
        ast.extend([Declaration(Variable(symbol, type=int32)) for symbol in self._temporaries])
        ast.append(for_block)
        ast.append(FunctionCall("gap_waitbarrier", (Integer(0),)))
        # create function definition
        func_def = FunctionDefinition(VOID, kernel_name, [Pointer(
            "Args", type=Type(kernel_arg_type_name))], ast)
        # generate code
        return ccode(func_def, contract=False,
                     dereference=dereference,
                     user_functions=CFUNC_DEFS,
                     type_mappings=TYPE_MAPPINGS)

    def kernel_arg_type_codegen(self, type_name):
        member_types = {
            "int32": lambda x: Variable(x, type=int32),
            "uint32": lambda x: Variable(x, type=uint32),
            "pint8": lambda x: Pointer(x, type=int8),
        }
        main_block = []
        main_block.append(
            TypeDef(
                type_name,
                Struct(
                    type_name, (
                        StructMember(member_types[kernel_arg[1]](kernel_arg[0])) for kernel_arg in self.kernel_args
                    )
                )
            )
        )
        return ccode(CodeBlock(*main_block),
                     contract=False,
                     type_mappings=TYPE_MAPPINGS)

    def kernel_arg_ctype_structure(self, type_name):
        member_types = {
            "int32": lambda x: (x, c_int),
            "uint32": lambda x: (x, c_uint),
            "pint8": lambda x: (x, POINTER(c_char)),
        }
        return type(type_name, (Struct,), {
            '_fields_': [member_types[kernel_arg[1]](kernel_arg[0]) for kernel_arg in self.kernel_args]
        })

    def kernel_arg_local_assign(self, args_var):
        member_types = {
            "int32": lambda x: Variable(x, type=int32),
            "uint32": lambda x: Variable(x, type=uint32),
            "pint8": lambda x: Pointer(x, type=int8, attrs=(restrict, )),
        }
        return [AssignmentEx(Declaration(member_types[kernel_arg[1]](kernel_arg[0])),
                             StructVariable(args_var, Symbol(kernel_arg[0])))
                for kernel_arg in self.kernel_args]

    def gen_kernel_model(self, kernel_name, kernel_arg_type_name):
        member_types = {
            "int32": lambda x: (QuotedString("int"), QuotedString(x)),
            "uint32": lambda x: (QuotedString("unsigned int"), QuotedString(x)),
            "pint8": lambda x: (QuotedString("signed char *"), QuotedString(x))
        }
        kernel_args = self.kernel_args
        code_block = CodeBlock(
            FunctionCall(
                "LibKernelTemplate",
                (
                    QuotedString(kernel_arg_type_name),
                    FunctionCall(
                        "CArgs",
                        tuple(
                            [Integer(len(kernel_args))] + [
                                FunctionCall("TCArg", member_types[kernel_arg[1]](kernel_arg[0]))
                                for kernel_arg in kernel_args]
                        )
                    )
                )
            ),
            FunctionCall(
                "LibKernel",
                (
                    QuotedString(kernel_name),
                    String("CALL_PARALLEL"),
                    Integer(0),
                    QuotedString(kernel_arg_type_name),
                    Integer(0)
                )
            )
        )
        return ccode(code_block,
                     contract=False,
                     type_mappings=TYPE_MAPPINGS)
