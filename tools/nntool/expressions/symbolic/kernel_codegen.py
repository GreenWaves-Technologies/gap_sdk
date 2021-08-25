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

from email import iterators
import logging
from typing import Sequence

import numpy as np
from generation.code_block import CodeBlock

from .function_collection import FunctionCollection

LOG = logging.getLogger("nntool." + __name__)


class BasicKernel():
    def __init__(self, func_col: FunctionCollection, constant_input_names: Sequence[str]) -> None:
        self._func_col = func_col
        self._constant_input_names = constant_input_names

    @property
    def func_col(self):
        return self._func_col

    @property
    def input_names(self):
        return self._func_col.input_names

    @property
    def input_names_and_ctypes(self):
        return [(name, self.func_col.qrecs[name].ctype) for name in self.input_names]

    @property
    def output_names(self):
        return self._func_col.output_names

    @property
    def output_names_and_ctypes(self):
        return [(name, self.func_col.qrecs[name].ctype) for name in self.output_names]

    @property
    def intermediate_names(self):
        return self._func_col.intermediate_names

    @property
    def shapes(self):
        return self._func_col.var_shapes

    @property
    def kernel_dims(self):
        return self._func_col.kernel_dims

    @property
    def kernel_args(self):
        kernel_args = []
        for kiter in self.func_col.iterators:
            kernel_args.append((kiter.name.upper(), "unsigned int"))
        for input_name, ctype in self.input_names_and_ctypes:
            kernel_args.append((input_name, f"{ctype} *"))
        for output_name, ctype in self.output_names_and_ctypes:
            kernel_args.append((output_name, f"{ctype} *"))
        return kernel_args

    def ctype_len(self, sym_name):
        dtype = self.func_col.qrecs[sym_name].dtype
        if dtype == np.int8:
            return 1
        if dtype == np.int16:
            return 2
        if dtype == np.float32:
            return 4
        assert False, f"don't know dtype {dtype}"

    def gen_kernel_headers(self, code: CodeBlock = None):
        if code is None:
            code = CodeBlock()
        for include in self._func_col.c_header_set:
            code.write('#include {}', include)
        return code

    def gen_user_kernel(self, ukname: str, kname: str, code: CodeBlock = None):
        if code is None:
            code = CodeBlock()
        code.write("int {0}(char *Name) {{", ukname)
        code.indent()
        code.write("Kernel_T *Kernel = UserKernel(")
        code.indent()
        code.write("Name,")
        code.write("{0},", self.gen_iterspace())
        kargs = self.gen_kargs()
        code.write("TILE_HOR,")
        cargs = self.gen_cargs()
        code.write("CArgs({0},", len(cargs))
        code.indent()
        for carg in cargs[:-1:]:
            code.write("{0},", carg)
        code.write("{0}", cargs[-1])
        code.deindent()
        code.write("),")
        code.write("Calls(1,")
        code.indent()
        code.write("Call(\"{0}\", LOC_D{1},", kname,
                   len(self.func_col.iterators) - 1)
        code.indent()
        bindings = self.gen_kerbingings()
        code.write("Bindings({0},", len(bindings))
        code.indent()
        for binding in bindings[:-1:]:
            code.write("{0},", binding)
        code.write("{0}", bindings[-1])
        code.deindent()
        code.write(")")
        code.deindent()
        code.write(")")
        code.deindent()
        code.write("),")
        code.write("KerArgs({0},", len(cargs))
        code.indent()
        for karg in kargs[:-1:]:
            code.write("{0},", karg)
        code.write("{0}", kargs[-1])
        code.deindent()
        code.write(")")
        code.deindent()
        code.write(");")
        code.write("if (Kernel) {{")
        code.indent()
        for kinfo in self.gen_kinfos():
            code.write("{0};", kinfo)
        code.deindent()
        code.write("}}")
        code.write("return (Kernel!=0);")
        code.deindent()
        code.write("}}")
        return code

    def gen_kinfos(self):
        in_sizes = [np.prod(self._func_col.var_shapes[var_name])
                    for var_name in self._func_col.input_names]
        bandwidth = sum([np.prod(self._func_col.var_shapes[var_name])
                         for var_name in self._func_col.output_names]) + sum(in_sizes)
        kinfos = [
            "AddKernelInfos(Name, AT_KERINFO_OPER, {0}, 0)".format(
                self._func_col.ops * max(in_sizes)),
            "AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, {0}, 0)".format(
                bandwidth)
        ]
        for name in self.input_names + self.output_names:
            shape = self.shapes[name]
            kinfos.append("AddKernelArgDim(Name, \"{0}\", {1}, {2}, {3})".format(
                name, len(shape) + 1, ", ".join(str(dim) for dim in shape),
                self.ctype_len(name)))
        return kinfos

    def gen_cargs(self):
        cargs = []
        for name_type in self.input_names_and_ctypes + self.output_names_and_ctypes:
            name = name_type[0]
            if name_type[1] == 'float':
                cargs.append("TCArg(CNN_ArgDataTypeF({0},1,1),\"{1}\")".format(
                    self.ctype_len(name), name))
            else:
                cargs.append("TCArg(CNN_ArgDataType({0},1,1),\"{1}\")".format(
                    self.ctype_len(name), name))
        return cargs

    def gen_kargs(self):
        kargs = []
        for input_name in self.input_names:
            arg_indexes = self._func_col.variable_indexes[input_name]
            argspaces = ", ".join(f'KER_ITER_D{idx}' for idx in arg_indexes)
            argspace = f'KerArgSpace({len(arg_indexes)}, {argspaces})' if arg_indexes else f'KerArgSpace(1, KER_ITER_TILE0)'
            if input_name in self._constant_input_names:
                constraints = "O_IN|O_DB|O_CONST"
            else:
                constraints = "O_IN|O_DB" if arg_indexes else "O_IN|O_BUFF|O_NTILED"
            kargs.append("KerArg(\"{0}\", {1}, {2}, {3}, {4}, {5}, 0, 0, 0, \"{0}\")".format(
                input_name,
                argspace,
                constraints,
                1,
                1,
                self.ctype_len(input_name)))

        for output_name in self.output_names:
            arg_indexes = self._func_col.variable_indexes[output_name]
            argspaces = ", ".join(f'KER_ITER_D{idx}' for idx in arg_indexes)
            argspace = f'KerArgSpace({len(arg_indexes)}, {argspaces})' if arg_indexes else 'KerArgSpace(1, KER_ITER_TILE0)'
            name = output_name
            constraints = "O_OUT|O_DB" if arg_indexes else "O_OUT|O_BUFF|O_NTILED"
            kargs.append("KerArg(\"{0}\", {1}, {2}, {3}, {4}, {5}, 0, 0, 0, \"{0}\")".format(
                name, argspace, constraints, 1, 1,
                self.ctype_len(output_name)))
        return kargs

    def gen_iterspace(self):
        # All iterators are in parametric spaces. The iterator we will
        # parallelize on has its preferred div set to 8
        # since only 3 tiled spaces are allowed including the dummy TILE0 space if there are scalars
        # we check for that and only tile the first 3 spaces
        tiled_iterators = self.tiled_iterators
        iterators = [
            f'IterFixedSpace(KER_ITER_D{idx}, {iterator.shape[0]})'
            if iterator not in tiled_iterators else
            f'IterParSpace(KER_ITER_D{idx}, {iterator.shape[0]}, '
            f'{min(8, iterator.shape[0]) if iterator == self.parallel_iterator else 1})'
            for idx, iterator in enumerate(self._func_col.iterators)]
        # append dummy TILE0 space to put scalars into if there are scalar inputs (which is unlikely)
        if self.has_scalar_parameters:
            iterators.append('IterTiledSpace(KER_ITER_TILE0)')
        return f'KernelIterSpace({len(iterators)}, {",".join(iterators)})'

    def gen_kerbingings(self):
        max_dim_var = max(self.output_names, key=lambda x: len(self.shapes[x]))
        bindings = [
            f"K_ArgPar(\"{max_dim_var}\", KER_ARG_PARTILE_SIZE, KER_ITER_D{idx})"
            for idx in range(len(self._func_col.iterators))
        ] + [
            f"K_Arg(\"{name}\", KER_ARG_TILE)"
            for name in self.input_names + self.output_names
        ]
        return bindings

    @property
    def parallel_iterator(self):
        return max(self.func_col.iterators, key=lambda x: x.shape[0])

    @property
    def tiled_iterators(self):
        return sorted(self.func_col.iterators, key=lambda x: x.shape[0])[-2::]

    @property
    def fixed_iterators(self):
        tiled_iterators = self.tiled_iterators
        return [iterator for iterator in self.func_col.iterators if iterator not in tiled_iterators]

    @property
    def has_scalar_parameters(self):
        return any(not self._func_col.variable_indexes[input_name]
                   for input_name in self.input_names + self.output_names)

    def gen_function(self, kernel_name, kernel_arg_type_name, code=None):
        if code is None:
            code = CodeBlock()

        code.comment(
            "Output iteration space reduced to %s iteration spaces" % (self.kernel_dims))
        code.write("void {}({} *Args) {{", kernel_name, kernel_arg_type_name)
        code.indent()
        for kerarg_name, kerarg_type in self.kernel_args:
            code.write('{0} {1} = Args->{1};', kerarg_type, kerarg_name)
        # paralellize on largest dimension
        last_first = self.parallel_iterator.name.upper()
        code.write('unsigned int CoreId = gap_coreid();')
        code.write('unsigned int Chunk = ChunkSize({});', last_first)
        code.write('unsigned int First = Chunk*CoreId;')
        code.write('unsigned int Last = gap_min(First+Chunk, {});', last_first)
        self._func_col.create_kernel(self.parallel_iterator, self.fixed_iterators, code)
        code.write('gap_waitbarrier(0);')
        code.deindent()
        code.write('}}')
        return code

    def kernel_arg_type_codegen(self, type_name, code=None):
        if code is None:
            code = CodeBlock()
        code.write('typedef struct {{')
        code.indent()
        for kerarg_name, kerarg_type in self.kernel_args:
            code.write('{} {};', kerarg_type, kerarg_name)
        code.deindent()
        code.write('}} {};', type_name)
        return code

    def gen_kernel_model(self, kernel_name, kernel_arg_type_name, code=None):
        if code is None:
            code = CodeBlock()
        code.write('LibKernelTemplate(')
        code.indent()
        code.write('"{}",', kernel_arg_type_name)
        code.write('CArgs({},', len(self.kernel_args))
        code.indent()
        for idx, (kerarg_name, kerarg_type) in enumerate(self.kernel_args):
            code.write('TCArg("{}", "{}"){}',
                       kerarg_type,
                       kerarg_name,
                       "," if idx < (len(self.kernel_args) - 1) else '')
        code.deindent()
        code.write(')')
        code.deindent()
        code.write(');')
        code.write('LibKernel(')
        code.indent()
        code.write('"{}",', kernel_name)
        code.write('CALL_PARALLEL,')
        code.write('0,')
        code.write('"{}",', kernel_arg_type_name)
        code.write('0')
        code.deindent()
        code.write(');')

        return code
