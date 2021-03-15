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
    def output_names(self):
        return self._func_col.output_names

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
        for sym in ["H", "W"]:
            kernel_args.append((sym, "unsigned int"))
        if self.kernel_dims == 3:
            kernel_args.append(("InFeatures", "unsigned int"))
        for input_name in self.input_names:
            kernel_args.append((input_name, "signed char *"))
        for output_name in self.output_names:
            kernel_args.append((output_name, "signed char *"))
        return kernel_args

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
        code.write("Call(\"{0}\", LOC_LOOP,", kname)
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
        kargs = self.gen_kargs()
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
        bandwidth = sum([np.prod(self._func_col.var_shapes[var_name])
                         for var_name in self._func_col.input_names + self._func_col.output_names])
        kinfos = [
            "AddKernelInfos(Name, AT_KERINFO_OPER, {0}, 0)".format(self._func_col.ops),
            "AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, {0}, 0)".format(bandwidth)
        ]
        for name in self.input_names + self.output_names:
            shape = self.shapes[name]
            kinfos.append("AddKernelArgDim(Name, \"{0}\", {1}, {2})".format(
                name, len(shape), ", ".join(str(dim) for dim in shape)))
        return kinfos

    def gen_cargs(self):
        cargs = []
        for name in self.input_names + self.output_names:
            cargs.append("TCArg(CNN_ArgDataType(1,1,1),\"{0}\")".format(name))
        return cargs

    def gen_kargs(self):
        kargs = []
        if self.kernel_dims == 3:
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
            dim_w = 2
            dim_h = 1
        elif self.kernel_dims == 2:
            argspaces = {
                tuple(): "KerArgSpace(1, KER_ITER_TILE0)",
                (0,): "KerArgSpace(1, KER_ITER_TILE0)",
                (1,): "KerArgSpace(1, KER_ITER_TILE0)",
                (0, 1): "KerArgSpace(1, KER_ITER_TILE0)",
            }
            dim_w = 1
            dim_h = 0
        elif self.kernel_dims == 1:
            argspaces = {
                tuple(): "KerArgSpace(1, KER_ITER_TILE0)",
                (0,): "KerArgSpace(1, KER_ITER_TILE0)",
            }
            dim_w = -1
            dim_h = 0
        else:
            raise NotImplementedError()
        for input_name in self.input_names:
            arg_indexes = self._func_col.variable_indexes[input_name]
            argspace = argspaces[arg_indexes]
            constraints = "O_IN|O_DB|O_CONST" if input_name in self._constant_input_names else "O_IN|O_DB"
            dims = [index.shape[0] if idx in arg_indexes else 1 for idx,
                    index in enumerate(self._func_col.iterators)]
            width = dims[dim_w] if dim_w > -1 else 1
            height = dims[dim_h] if dim_h > -1 else 1
            kargs.append("KerArg(\"{0}\", {1}, {2}, {3}, {4}, 1, 0, 0, 0, \"{0}\")".format(
                input_name,
                argspace,
                constraints,
                height,
                width))

        for output_name in self.output_names:
            arg_indexes = self._func_col.variable_indexes[output_name]
            argspace = argspaces[arg_indexes]
            name = output_name
            constraints = "O_OUT|O_DB"
            dims = [index.shape[0] if idx in arg_indexes else 1 for idx,
                    index in enumerate(self._func_col.iterators)]
            width = dims[dim_w] if dim_w > -1 else 1
            height = dims[dim_h] if dim_h > -1 else 1
            kargs.append("KerArg(\"{0}\", {1}, {2}, {3}, {4}, 1, 0, 0, 0, \"{0}\")".format(
                name, argspace, constraints, height, width))
        return kargs

    def gen_iterspace(self):
        if self.kernel_dims == 3:
            return "KernelIterSpace(2, IterParSpace(KER_ITER_D0, {0}, 1), IterTiledSpace(KER_ITER_TILE0))".format(
                self._func_col.iterators[0].shape[0])
        return "KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0))"

    def gen_kerbingings(self):
        bindings = [
            "K_Arg(\"{0}\", KER_ARG_TILE_W)".format(self.output_names[0]),
            "K_Arg(\"{0}\", KER_ARG_TILE_H)".format(self.output_names[0])
        ]
        if self.kernel_dims == 3:
            threed_outname = next(
                iter(name for name in self.output_names if len(self.shapes[name]) == 3))
            bindings.append("K_ArgPar(\"{0}\", KER_ARG_PARTILE_SIZE, KER_ITER_D0)".format(
                threed_outname))
        for name in self.input_names + self.output_names:
            bindings.append("K_Arg(\"{0}\", KER_ARG_TILE)".format(name))
        return bindings

    def gen_function(self, kernel_name, kernel_arg_type_name, code=None):
        if code is None:
            code = CodeBlock()

        code.comment("Output iteration space reduced to %s iteration spaces" % (self.kernel_dims))
        code.write("void {}({} *Args) {{", kernel_name, kernel_arg_type_name)
        code.indent()
        for kerarg_name, kerarg_type in self.kernel_args:
            code.write('{0} {1} = Args->{1};', kerarg_type, kerarg_name)
        if self.kernel_dims == 1:
            last_first = "Sz"
            code.write('unsigned int Sz = W * H;')
        elif self.kernel_dims == 2:
            last_first = "H"
        elif self.kernel_dims == 3:
            last_first = "InFeatures"
        else:
            raise ValueError("expression has too many dimensions")

        code.write('unsigned int CoreId = gap_coreid();')
        code.write('unsigned int Chunk = ChunkSize({});', last_first)
        code.write('unsigned int First = Chunk*CoreId;')
        code.write('unsigned int Last = gap_min(First+Chunk, {});', last_first)
        self._func_col.create_kernel(code)
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
