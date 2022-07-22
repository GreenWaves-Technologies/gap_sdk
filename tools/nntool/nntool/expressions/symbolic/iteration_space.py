# Copyright (C) 2022  GreenWaves Technologies, SAS

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


from functools import partial, reduce
from itertools import groupby
from operator import itemgetter
from typing import List, Sequence

import numpy as np
from bfloat16 import bfloat16
from nntool.generation.code_block import CodeBlock
from nntool.quantization.qtype import DTYPE_GAP_CTYPE, DTYPES
from nntool.utils.exception import NNToolInternelError

from .assignments import Assignments
from .symbol import Symbol


def split_list(sequence: Sequence):
    return [list(map(itemgetter(1), g)) for k, g in groupby(enumerate(sequence), lambda x: x[0]-x[1])]


def tabulate(lines):
    """Takes a list of lists of strings and lines up lengths to improve formating"""
    # get max length of each segment
    max_len = tuple(max(elems) for elems
                    in zip(*[tuple(len(line_elem) for line_elem in line)
                             for line in lines]))
    # add spaces to each line segment

    def reduction(s, x):
        s.append(
            f'{"".join(elem + " " * (max_len[idx] - len(elem)) for idx, elem in enumerate(x[:-1]))}{x[-1]}')
        return s
    return reduce(reduction, lines, [])


AT_ARG_DATATYPES = {
    np.uint8:   ('CNN_ArgDataTypeUns',  1, False, False),
    np.uint16:  ('CNN_ArgDataTypeUns',  2, False, False),
    np.uint32:  ('CNN_ArgDataTypeUns',  4, False, False),
    np.uint64:  ('CNN_ArgDataTypeUns',  8, False, False),
    np.int8:    ('CNN_ArgDataType',     1, True, False),
    np.int16:   ('CNN_ArgDataType',     2, True, False),
    np.int32:   ('CNN_ArgDataType',     4, True, False),
    np.int64:   ('CNN_ArgDataType',     8, True, False),
    np.float16: ('CNN_ArgDataTypeF',    2, True, True),
    bfloat16:   ('CNN_ArgDataTypeF',    2, True, True),
    np.float32: ('CNN_ArgDataTypeF',    4, True, True),
}


class IterationSpace():
    def __init__(self, assignments: Assignments, constants=None, min_interior_space=1000, max_interior_space=10000, num_parameteric_spaces=2) -> None:
        self._assignments = assignments
        self._var_shapes = None
        self._var_axes = None
        self._var_is_constant = {}
        self._spaces = None
        self._var_axes_idx = None
        self._min_interior_space = min_interior_space
        self._max_interior_space = max_interior_space
        self._num_workers = 8
        self._var_strides = {}
        self._num_parameteric_spaces = num_parameteric_spaces
        if constants:
            self.vars_are_constant(*constants)

    @staticmethod
    def _var_name(idx):
        return f"i{idx}"

    @staticmethod
    def _var_max_name(idx):
        return f"I{idx}"

    @staticmethod
    def _par_name(idx):
        return f"d{idx}"

    @staticmethod
    def _par_first_name(idx):
        return f"D{idx}First"

    @staticmethod
    def _par_last_name(idx):
        return f"D{idx}Last"

    @staticmethod
    def _par_max_name(idx):
        return f"D{idx}"

    def set_var_stride(self, vname, stride):
        self._var_shapes = None
        self._var_axes = None
        self._spaces = None
        self._var_strides[vname] = stride
        self._var_axes_idx = None

    def is_var_constant(self, var_name):
        return self._var_is_constant.get(var_name, False)

    def vars_are_constant(self, *var_names):
        for var_name in var_names:
            self._var_is_constant[var_name] = True
        return self

    @property
    def assignments(self):
        return self._assignments

    @property
    def real_shape(self):
        shape = tuple(dim for dim in self.shape if dim != 1)
        if not shape:
            return ((1,),)
        return shape

    @property
    def shape(self):
        return self.assignments.max_shape

    @property
    def full_rank(self):
        return len(self.shape)

    @property
    def extended_strides(self):
        full_len = self.full_rank
        if not self._var_strides:
            return [frozenset([1])] * full_len
        vstrides = self._var_strides.copy()
        vshapes = self.var_shapes
        for vname in self.variables:
            if vname not in vstrides:
                vstrides[vname] = [{} if dim == 1 else {1}
                                   for dim in vshapes[vname]]
            else:
                vstrides[vname] = [{dim} if vshapes[vname][idx] != 1 else {
                } for idx, dim in enumerate(vstrides[vname])]
            vstrides[vname] = (
                [{}] * (full_len - len(vstrides[vname]))) + list(vstrides[vname])
        return tuple(reduce(lambda state, x: state + [frozenset(set.union(*x))], zip(*tuple(vstrides.values())), []))

    @property
    def output_names(self):
        return self._assignments.output_names

    @property
    def input_names(self):
        return self._assignments.input_names

    @property
    def temporary_names(self):
        return self._assignments.intermediate_names

    def space_for_axis(self, axis):
        return next((axes for axes in self.spaces if axis in axes), None)

    @property
    def var_axes(self):
        """Map of variable name to tuple of axes of iteration space used

        Returns:
            dict: Map of variable name to tuple of axes of iteration space used
        """
        if self._var_axes is None:
            self._var_axes = {vname: tuple(sorted(set(self.space_for_axis(axis)
                                                      for axis, dim in enumerate(shape) if dim != 1)))
                              for vname, shape in self.var_shapes.items()}
        return self._var_axes

    @property
    def var_axes_idx(self):
        """Map of variable name to index of iteration space used

        Returns:
            dict: Map of variable name to index of iteration space used
        """
        if self._var_axes_idx is None:
            self._var_axes_idx = {vname: tuple(self.spaces.index(dim) for dim in axes)
                                  for vname, axes in self.var_axes.items()}
        return self._var_axes_idx

    @property
    def variables(self):
        """Set of variable names
        """
        return set(self.var_shapes)

    @property
    def spaces_size(self):
        return tuple(int(np.prod([self.shape[idx] for idx in space])) for space in self.spaces)

    @property
    def spaces(self):
        """Set of disjoint iteration spaces that have the same set of strides
        """
        if self._spaces is None:
            spaces = self._assignments.axes
            # here we modify grouped spaces so that continuous spaces have the same stride
            if self._var_strides:
                final_spaces = list(spaces)
                for var_stride in self._var_strides.values():
                    # expand the stride to the full shape
                    # The [1] may be wrong when we expand to supporting non unit strides
                    # perhaps set to None and then remove and add back to the largest space in the reduction?
                    var_stride = (
                        [1] * (self.full_rank - len(var_stride))) + list(var_stride)

                    def reduction(var_stride, state: List, space):
                        space_strides = {}
                        for dim in space:
                            space_strides.setdefault(
                                var_stride[dim], []).append(dim)
                        for space_group in space_strides.values():
                            state.extend(split_list(space_group))
                        return state

                    final_spaces = reduce(
                        partial(reduction, var_stride), final_spaces, [])
                self._spaces = tuple(sorted(tuple(sorted(space))
                                            for space in final_spaces))
            else:
                self._spaces = spaces

        return self._spaces

    @property
    def expanded_spaces(self):
        res = []
        last = 0
        for space in self.spaces:
            res.append(tuple(range(last, min(space))) + space)
        if res:
            res[-1] = res[-1] + tuple(range(max(res[-1])+1, len(self.shape)))
        return tuple(res)

    @property
    def space_total_items(self):
        return tuple(np.stack(list(self.var_shapes.values())).sum(axis=0))

    @property
    def space_total_bytes(self):
        variables = [self.assignments.variables[name]
                     for name in self.var_shapes]
        sizes = [1 if var.dtype is None else AT_ARG_DATATYPES[var.dtype][1]
                 for var in variables]
        return tuple((np.stack(list(self.var_shapes.values())) * np.array(sizes).reshape((-1, 1))).sum(axis=0))

    @property
    def var_shapes(self):
        if self._var_shapes is None:
            self._assignments.update()
            self._var_shapes = self._assignments.var_shapes.copy()
            self._var_shapes = dict(
                zip(self._var_shapes.keys(), Symbol.extend_shapes(*self._var_shapes.values())))

        return self._var_shapes

    @property
    def has_scalar_parameters(self):
        return any(len(shape) == 1 and shape[0] == 1 for shape in self.var_shapes.values())

    @property
    def iterator_names(self):
        return [self._var_name(idx) for idx in range(len(self.spaces))]

    @property
    def interior_spaces(self):
        """This provides the minimum tile space if it is more than one axis"""
        expanded_spaces = list(self.expanded_spaces)
        if len(expanded_spaces) <= 1:
            return tuple()
        dims = []
        num_bytes = self.space_total_bytes
        total = 1
        while len(expanded_spaces) > 1 and total < self._min_interior_space:
            new_size = total * np.prod([num_bytes[idx]
                                        for idx in expanded_spaces[-1]])
            if new_size * 8 > self._max_interior_space:
                break
            dims.append(len(expanded_spaces) - 1)
            total = new_size
            expanded_spaces = expanded_spaces[0:-1]
        return tuple(self.spaces[idx] for idx in reversed(dims))

    @property
    def interior_shape(self):
        return tuple(self.shape[self.spaces.index(space)] for space in self.interior_spaces)

    @property
    def exterior_spaces(self):
        return tuple(self.spaces[idx] for idx in range(len(self.spaces_size) - len(self.interior_spaces)))

    @property
    def exterior_space_names(self):
        return tuple(self._par_name(self.spaces.index(space)) for space in range(len(self.exterior_spaces)))

    @property
    def exterior_shape(self):
        shape = list(self.spaces_size)
        num_ext_spaces = len(self.exterior_spaces)
        return tuple(shape[:num_ext_spaces - 1] + [shape[num_ext_spaces - 1] * np.prod(self.interior_shape)])

    @property
    def parametric_spaces(self):
        return tuple(self.exterior_spaces[-self._num_parameteric_spaces:])

    @property
    def paralellized_space(self):
        if self.parametric_spaces:
            return max([(space, self.real_shape[self.spaces.index(space)]) for space in self.parametric_spaces], key=lambda x: x[1])[0]
        return 0

    @property
    def interior_shape_size(self):
        return np.prod(self.interior_shape)

    @property
    def fixed_spaces(self):
        return tuple(self.exterior_spaces[:-self._num_parameteric_spaces])

    def preferred_divisor(self, space):
        if space == 0:
            if len(self.exterior_shape) == 1:
                return self.interior_shape_size * self._num_workers
            else:
                return self._num_workers
        elif space == len(self.exterior_shape) - 1:
            return self.interior_shape_size
        return 1

    def c_indexed_var(self, var_name, declare=False, assignment=False):
        # if var_name.startswith('_SYMBOL'):
        #     return var_name
        if var_name in self.temporary_names:
            if declare or assignment:
                dtype = self._assignments.variable(var_name).dtype
                return f"{DTYPE_GAP_CTYPE[dtype]} {var_name}"
            return var_name
        if declare:
            dtype = self._assignments.variable(var_name).dtype
            return f"{DTYPE_GAP_CTYPE[dtype]} *{var_name}"
        c_index = self.c_index(var_name)
        if c_index:
            return f'{var_name}{c_index}'
        return f'*{var_name}'

    def c_index(self, var_name):
        var_ext_shape = self.var_shapes[var_name]
        var_shape = [np.prod([var_ext_shape[dim] for dim in space])
                     for space in self.var_axes[var_name]]
        if var_name in self._var_strides:
            var_stride = self._var_strides[var_name]
            var_stride_in_space = []
            for space in self.var_axes[var_name]:
                var_stride_in_space.append(var_stride[space[0]])
                assert all(var_stride_in_space[-1] ==
                           var_stride[dim] for dim in space[1:])
        else:
            var_stride_in_space = [1] * len(self.var_axes[var_name])

        def reduction(state, x):
            var_space, space_dim, var_stride = x
            var_space_idx = self.spaces.index(var_space)
            # fixed spaces are iterated by tiler code
            if var_space in self.fixed_spaces:
                return state
            space_size = str(
                space_dim) if var_space == self.interior_spaces else self._var_max_name(var_space_idx)
            assert abs(var_stride) == 1, "non unit strides not supported yet"
            if var_stride < 0:
                if var_space == 0:
                    index = f'(Last-1-{self._var_name(var_space_idx)})'
                else:
                    index = f'({space_size}-1-{self._var_name(var_space_idx)})'
            else:
                index = f'{self._var_name(var_space_idx)}'
            if state[0]:
                state[1].insert(
                    0, f"({index}*{'*'.join(state[0])})")
            else:
                state[1].insert(0, index)

            state[0].insert(0, str(
                space_dim) if var_space == self.interior_spaces else self._var_max_name(var_space_idx))
            return state
        index = reduce(reduction, zip(reversed(self.var_axes[var_name]),
                                      reversed(var_shape),
                                      reversed(var_stride_in_space)), ([], []))[1]
        return f"[{'+'.join(index)}]" if index else ""

    def get_iterator_vars(self):
        iters = []
        for idx, space in enumerate(self.spaces):
            if space in self.interior_spaces:
                continue
            if space == self.paralellized_space:
                iters.extend([('First', 0), ('Last', self.spaces_size[idx]),
                              (self._var_max_name(idx), self.spaces_size[idx])])
            else:
                iters.append(
                    (self._var_max_name(idx), self.spaces_size[idx]))
        return iters

    def c_for(self, space, with_fixed=False):
        if not with_fixed and space == self.fixed_spaces:
            raise ValueError(
                "space is fixed so not iterated inside basic kernel and requires no for loop")
        space_index = self.spaces.index(space)
        var_name = self._var_name(space_index)
        if space in self.interior_spaces:
            return f"for (int {var_name}=0; {var_name}<{self.real_shape[space_index]}; {var_name}++)"
        if space == self.paralellized_space:
            return f"for (int {var_name}=First; {var_name}<Last; {var_name}++)"
        return f"for (int {var_name}=0; {var_name}<{self._var_max_name(space_index)}; {var_name}++)"

    def ctype_len(self, var_name):
        return DTYPES[self._assignments.variables[var_name].dtype][0]//8

    def ctype(self, var_name, pointer=False, restrict=False):
        return (f'{DTYPE_GAP_CTYPE[self._assignments.variables[var_name].dtype]}'
                f'{" *" if pointer else ""}'
                f'{"__restrict__ " if restrict else ""}')

    def at_argdatatype(self, var_name, restrict=False, pointer=False):
        dtype = self._assignments.variables[var_name].dtype
        dt_func, datasize, _, _ = AT_ARG_DATATYPES[dtype]
        return f'{dt_func}({datasize}, {1 if pointer else 0}, {1 if restrict else 0})'

    @property
    def at_uk_iterspace(self):
        # All iterators are in parametric spaces. The iterator we will
        # parallelize on has its preferred div set to 8
        # since only 3 tiled spaces are allowed including the dummy TILE0 space if there are scalars
        # we check for that and only tile the first 3 spaces
        iterators = (
            [f'IterFixedSpace(KER_ITER_D{self.spaces.index(space)}, {self.spaces_size[self.spaces.index(space)]})'
             for space in self.fixed_spaces] +  # fixed spaces
            [f'IterParSpace(KER_ITER_D{self.spaces.index(space)}, {self.spaces_size[self.spaces.index(space)]}, '
             f'{8 if space == self.paralellized_space else 1})'  # other par spaces
             for space in self.parametric_spaces])
        # always include tile space?
        iterators.append('IterTiledSpace(KER_ITER_TILE0)')
        return f'KernelIterSpace({len(iterators)}, {", ".join(iterators)})'

    @property
    def at_uk_kargs(self):
        kargs = []
        for var_name, axis_idxes in self.var_axes_idx.items():
            if var_name in self.temporary_names:
                continue
            iter_space_descrs = []
            int_size = 1
            var_stride = self._var_strides.get(var_name)
            for var_dim_idx, space_idx in enumerate(axis_idxes):
                if space_idx >= len(self.exterior_spaces):
                    int_size *= self.spaces_size[space_idx]
                    continue
                if var_stride and var_stride[var_dim_idx] < 0:
                    iter_space_descrs.append(
                        f'KER_ITER_D{space_idx}|SPACE_PROP_REVERT')
                else:
                    iter_space_descrs.append(f'KER_ITER_D{space_idx}')
            if iter_space_descrs:
                argspace = f'KerArgSpace({len(iter_space_descrs)}, {", ".join(iter_space_descrs)})'
            else:
                argspace = 'KerArgSpace(1, KER_ITER_TILE0)'
            if var_name in self.output_names:
                constraints = "O_OUT|O_DB" if iter_space_descrs else "O_OUT|O_BUFF|O_NTILED"
            elif self.is_var_constant(var_name):
                constraints = "O_IN|O_DB|O_CONST"
            else:
                constraints = "O_IN|O_DB" if iter_space_descrs else "O_IN|O_BUFF|O_NTILED"
            kargs.append(
                (f'KerArg("{var_name}", ',
                 f'{argspace}, ',
                 f'{constraints}, ',
                 '1, 1, ',
                 f'{self.ctype_len(var_name) * int_size}, ',
                 f'0, 0, 0, "{var_name}")'))
        return tabulate(kargs)

    @property
    def at_uk_cargs(self):
        return ([f'TCArg({self.at_argdatatype(var_name, pointer=True, restrict=True)}, "{var_name}")'
                 for var_set in [self.input_names, self.output_names]
                 for var_name in sorted(var_set)])

    @property
    def at_uk_kinfos(self):
        cvars = sorted(self.input_names)
        pvars = sorted(self.output_names)
        in_sizes = [np.prod(self.var_shapes[var_name])
                    for var_name in cvars]
        out_sizes = [np.prod(self.var_shapes[var_name])
                     for var_name in pvars]
        bandwidth = sum(in_sizes + out_sizes)
        kinfos = [
            f"AddKernelInfos(Name, AT_KERINFO_OPER, {self._assignments.ops * max(in_sizes)}, 0)",
            f"AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, {bandwidth}, 0)"
        ]
        ksize_infos = []
        for var_name in cvars + pvars:
            shape = reduce(
                lambda s, x: s + [x] if x > 1 or s else s, self.var_shapes[var_name], [])
            if not shape:
                shape = [1]
            _, item_size, _, is_float = AT_ARG_DATATYPES[
                self._assignments.variables[var_name].dtype]
            add_arg_func = "AddKernelFloatArgDim" if is_float else "AddKernelArgDim"
            ksize_infos.append(
                (f'{add_arg_func}(Name, "{var_name}", ',
                 f'{len(shape) + 1}, ',
                 f'{", ".join(str(dim) for dim in shape)}, ',
                 f'{item_size})'))
        return kinfos + tabulate(ksize_infos)

    @property
    def kernel_args(self):
        return ([(self._var_max_name(self.spaces.index(space)), 'unsigned int') for space in self.exterior_spaces] +
                [(var_name, self.ctype(var_name, pointer=True, restrict=True))
                 for var_set in [self.input_names, self.output_names]
                 for var_name in sorted(var_set)])

    @property
    def at_uk_kerbindings(self):
        bindings = [
            f"K_ArgPar(\"{max(self.output_names, key=lambda x: self.var_shapes[x][idx])}\","
            f" KER_ARG_PARTILE_SIZE, KER_ITER_D{idx})"
            for idx in range(len(self.exterior_shape))
        ] + [
            f"K_Arg(\"{var_name}\", KER_ARG_TILE)"
            for var_set in [self.input_names, self.output_names]
            for var_name in sorted(var_set)
        ]
        return bindings

    def comment_attrs(self, code, *attrs):
        code.comment("".join(f'{name}: {getattr(self, name)} ' if getattr(self, name) else ''
                             for name in attrs))

    def gen_kernel_headers(self, code: CodeBlock = None):
        if code is None:
            code = CodeBlock()
        for include in self._assignments.c_header_set:
            code.write('#include {}', include)
        return code

    def gen_user_kernel(self, ukname: str, kname: str, code: CodeBlock = None):
        if code is None:
            code = CodeBlock()
        code.write(f"int {ukname}(char *Name) {{")
        code.indent()
        code.write("Kernel_T *Kernel = UserKernel(")
        code.indent()
        code.write("Name,")
        # include some useful parameters as comment
        self.comment_attrs(code,
                           'shape',
                           'spaces')
        self.comment_attrs(code,
                           'fixed_spaces',
                           'parametric_spaces',
                           'interior_spaces')
        self.comment_attrs(code,
                           'exterior_shape',
                           'interior_shape')
        code.write(f'{self.at_uk_iterspace},')
        kargs = self.at_uk_kargs
        code.write("TILE_VER,")
        cargs = self.at_uk_cargs
        code.write(f"CArgs({len(cargs)},")
        code.indent()
        for carg in cargs[: -1:]:
            code.write(f"{carg},")
        code.write(f"{cargs[-1]}")
        code.deindent()
        code.write("),")
        code.write("Calls(1,")
        code.indent()
        code.write(f'Call("{kname}", LOC_D{len(self.exterior_shape) - 1},')
        code.indent()
        bindings = self.at_uk_kerbindings
        code.write(f"Bindings({len(bindings)},")
        code.indent()
        for binding in bindings[: -1:]:
            code.write(f"{binding},")
        code.write(f"{bindings[-1]}")
        code.deindent()
        code.write(")")
        code.deindent()
        code.write(")")
        code.deindent()
        code.write("),")
        for var_name, idxes in self.var_axes_idx.items():
            if var_name in self.temporary_names:
                continue
            stride = f" stride: {self._var_strides[var_name]}" if var_name in self._var_strides else ""
            code.comment(f'var: {var_name} axes: {idxes}{stride}')
        code.write("KerArgs({0},", len(kargs))
        code.indent()
        for karg in kargs[: -1:]:
            code.write("{0},", karg)
        code.write("{0}", kargs[-1])
        code.deindent()
        code.write(")")
        code.deindent()
        code.write(");")
        code.write("if (Kernel) {")
        code.indent()
        for kinfo in self.at_uk_kinfos:
            code.write("{0};", kinfo)
        code.deindent()
        code.write("}")
        code.write("return (Kernel!=0);")
        code.deindent()
        code.write("}")
        return code

    def gen_function(self, kernel_name: str, kernel_arg_type_name: str, code: CodeBlock = None):
        if code is None:
            code = CodeBlock()

        code.comment(
            f'Output iteration space reduced to {len(self.interior_spaces)} internal '
            f'and {len(self.exterior_spaces)} external iteration spaces')
        code.write(f"void {kernel_name}({kernel_arg_type_name} *Args) {{")
        code.indent()
        comments = []
        for kerarg_name, _ in self.kernel_args:
            # TODO - add qrecs for quantized kernels
            comments.append([
                f'{self.var_shapes[kerarg_name]} ' if kerarg_name in self.var_shapes else '',
                f'{self._assignments.qrecs[kerarg_name]}' if kerarg_name in self._assignments.qrecs else ''
            ])
        comments = tabulate(comments)
        for idx, (kerarg_name, kerarg_type) in enumerate(self.kernel_args):
            # TODO - add qrecs for quantized kernels
            comment = comments[idx]
            if comment.strip():
                comment = f' // {comment}'
            else:
                comment = ''
            code.write(
                f'{kerarg_type} {kerarg_name} = Args->{kerarg_name};{comment}')
        # paralellize on largest dimension
        last_first = self._var_max_name(
            self.spaces.index(self.paralellized_space))
        code.write('unsigned int CoreId = gap_coreid();')
        code.write(f'unsigned int Chunk = ChunkSize({last_first});')
        code.write('unsigned int First = Chunk*CoreId;')
        code.write(f'unsigned int Last = gap_min(First+Chunk, {last_first});')
        self._assignments.c_block(code, iteration_space=self,
                                  with_loops=True, with_comment=True)
        code.write('gap_waitbarrier(0);')
        code.deindent()
        code.write('}')
        return code

    def gen_custom_act_function(self, kernel_name: str, kernel_arg_type_name: str, code: CodeBlock = None):
        if code is None:
            code = CodeBlock()

        code.comment(
            f'Output iteration space reduced to {len(self.interior_spaces)} internal '
            f'and {len(self.exterior_spaces)} external iteration spaces')
        code.write(f"void {kernel_name}({kernel_arg_type_name} *Args) {{")
        code.indent()
        comments = []
        if not (self._assignments.infos  and self._assignments.infos[0].values):
            for kerarg_name, _ in self.kernel_args:
                # TODO - add qrecs for quantized kernels
                comments.append([
                    f'{self.var_shapes[kerarg_name]} ' if kerarg_name in self.var_shapes else '',
                    f'{self._assignments.qrecs[kerarg_name]}' if kerarg_name in self._assignments.qrecs else ''
                ])
            comments = tabulate(comments)
        args = [(var_name, self.ctype(var_name, pointer=True, restrict=True))
                for var_set in [self.input_names, self.output_names]
                for var_name in sorted(var_set)]
        for idx, (kerarg_name, kerarg_type) in enumerate(args):
            # TODO - add qrecs for quantized kernels
            if not (self._assignments.infos  and self._assignments.infos[0].values):
                comment = comments[idx]
                if comment.strip():
                    comment = f' // {comment}'
                else:
                    comment = ''
            else:
                comment = ''
            code.write(
                f'{kerarg_type} {kerarg_name} = Args->{kerarg_name};{comment}')
        if self._assignments.infos and self._assignments.infos[0].values:
            code.write('signed char * __restrict__ Infos = Args->Infos;')

        # paralellize on largest dimension
        last_first = self._var_max_name(
            self.spaces.index(self.paralellized_space))
        code.write('unsigned int CoreId = gap_coreid();')
        code.write(f'unsigned int {last_first} = Args->W*Args->H*Args->Feat;')
        code.write(f'unsigned int Chunk = ChunkSize({last_first});')
        code.write('unsigned int First = Chunk*CoreId;')
        code.write(f'unsigned int Last = gap_min(First+Chunk, {last_first});')
        self._assignments.c_block(code, iteration_space=self,
                                  with_loops=True, with_comment=True)
        code.write('gap_waitbarrier(0);')
        code.deindent()
        code.write('}')
        return code

    def gen_kernel_arg_typedecl(self, type_name, code=None):
        if code is None:
            code = CodeBlock()
        code.write('typedef struct {')
        code.indent()
        for kerarg_name, kerarg_type in self.kernel_args:
            code.write(f'{kerarg_type} {kerarg_name};')
        code.deindent()
        code.write(f'}} {type_name};')
        return code

    def gen_custom_act_kernel_arg_typedecl(self, type_name, code=None):
        if code is None:
            code = CodeBlock()
        code.write('typedef struct {')
        code.indent()
        args = [(var_name, self.ctype(var_name, pointer=True, restrict=True))
                for var_set in [self.input_names, self.output_names]
                for var_name in sorted(var_set)]
        if len(args) != 2:
            raise NNToolInternelError(
                'custom activation function has more than one input or output')
        for kerarg_name, kerarg_type in args:
            code.write(f'{kerarg_type} {kerarg_name};')
        code.write('unsigned short int W;')
        code.write('unsigned short int H;')
        code.write('unsigned short int Feat;')
        if self._assignments.infos  and self._assignments.infos[0].values:
            code.write('signed char * __restrict__ Infos;')
        code.deindent()
        code.write(f'}} {type_name};')
        return code

    def gen_kernel_model(self, kernel_name, kernel_arg_type_name, code=None):
        if code is None:
            code = CodeBlock()
        code.write('LibKernelTemplate(')
        code.indent()
        code.write(f'"{kernel_arg_type_name}",')
        code.write(f'CArgs({len(self.kernel_args)},')
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
        code.write('')
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

    def gen_custom_act_kernel_model(self, kernel_name, kernel_arg_type_name, code=None):
        if code is None:
            code = CodeBlock()
        code.write('LibKernelTemplate(')
        code.indent()
        code.write(f'"{kernel_arg_type_name}",')
        margs = []
        args = [(var_name, self.ctype(var_name, pointer=True, restrict=True))
            for var_set in [self.input_names, self.output_names]
            for var_name in sorted(var_set)]
        if len(args) != 2:
            raise NNToolInternelError(
                'custom activation function has more than one input or output')

        for idx, (kerarg_name, kerarg_type) in enumerate(args):
            margs.append(f'TCArg("{kerarg_type}", "{kerarg_name}")')
        margs.append('TCArg("unsigned short int", "W")')
        margs.append('TCArg("unsigned short int", "H")')
        margs.append('TCArg("unsigned short int", "Feat")')
        if self._assignments.infos  and self._assignments.infos[0].values:
            margs.append('TCArg("signed char * __restrict__", "Infos")')
        code.write(f'CArgs({len(margs)},')
        code.indent()
        for idx, marg in enumerate(margs):
            code.write(f'{marg}{"," if idx < (len(margs) - 1) else ""}')
        code.deindent()
        code.write(')')
        code.deindent()
        code.write(');')
        code.write('')
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
