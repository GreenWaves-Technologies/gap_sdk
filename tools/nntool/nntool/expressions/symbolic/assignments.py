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

from collections.abc import Sequence as ABCSequence
from copy import deepcopy

from nntool.expressions.symbolic.quantization_base import QuantizationHandlerBase
from nntool.generation.code_block import CodeBlock
from nntool.utils.disjoint_reduction import disjoint_reduction

from .symbol import Symbol, Variable

# Vec levels
# 0 - nothing indexed in outer direction
class Assignment():
    def __init__(self, variable, expression) -> None:
        self._ass = (variable, expression)
        self._vec_levels = 0

    def __getitem__(self, idx):
        return self._ass[idx]

class Assignments(ABCSequence):
    def __init__(self, assignments=None, returns=None, qrecs=None) -> None:
        super().__init__()
        self._assignments = []
        self._returns = set(returns if returns is not None else [])
        self._outputs = None
        self._inputs = None
        self._inters = None
        self._vars = []
        self._qrecs = qrecs
        self._infos = None
        self._output_shapes = None
        if assignments:
            for assignment in assignments:
                self.add(*assignment)
        self.update()

    @property
    def infos(self):
        return self._infos

    @infos.setter
    def infos(self, val):
        self._infos = val

    @property
    def max_shape(self):
        return tuple(max(elems) for elems in zip(*Symbol.extend_shapes(*[ass[1].shape for ass in self._assignments])))

    @property
    def unbound_shapes(self):
        return tuple(self._vars[name].shape for name in self.unbound_variables)

    @property
    def input_names(self):
        return self._inputs

    @property
    def output_names(self):
        return self._outputs

    @property
    def unbound_variables(self):
        return self._inputs

    @property
    def intermediate_names(self):
        return self._inters

    @property
    def variables(self):
        return self._vars

    @property
    def output_shapes(self):
        if self._output_shapes is None:
            self._output_shapes = dict(
                zip(
                    self.output_names,
                    Symbol.extend_shapes(*[self.variables[name].shape
                                           for name in self.output_names],
                                         max_length=len(self.max_shape))))
        return self._output_shapes

    @property
    def axes(self):
        var_shapes = Symbol.extend_shapes(
            *self.unbound_shapes, max_length=len(self.max_shape))
        axes = disjoint_reduction(set(frozenset(idx for idx, dim in enumerate(
            shape) if dim != 1) for shape in var_shapes))
        if not axes:  # special case when all scalar 1
            return ((0,),)
        return tuple(sorted([tuple(x) for x in axes]))

    @property
    def var_shapes(self):
        return {var.name: var.shape for var in self._vars.values()}

    @property
    def ops(self):
        # TODO: Implement
        return 1

    @property
    def qrecs(self):
        return self._qrecs

    @property
    def c_header_set(self):
        return set().union(*[assignment[1].c_header_set
                             for assignment in self._assignments])

    @property
    def dtypes_in_use(self):
        return set.union(*[assignment[1].dtypes_in_use
                           for assignment in self._assignments])

    def variable(self, name):
        return self._vars[name]

    def _add_int(self, var, func):
        for uname, uvar in func.unbound_variables.items():
            if uname in self._vars:
                uvar.shape = self._vars[uname].shape
                uvar.qrec = self._vars[uname].qrec
        if isinstance(var, str):
            if var in self._vars:
                var = self._vars[var]
            else:
                var = Variable(var, shape=func.shape, dtype=func.dtype)
        self._assignments.append(Assignment(var, func))

    def add(self, var, func):
        self._add_int(var, func)
        self.update()

    def update(self):
        self._output_shapes = None
        self._vars = {}
        free_var_names = set()
        for var, func in self._assignments:
            self._vars[var.name] = var
            var.shape = func.calculate_shape()
            for name, uvar in func.unbound_variables.items():
                self._vars[name] = uvar
                free_var_names.add(name)

        # these are all the produced variables
        prod_var_names = set(
            [assignment[0].name for assignment in self._assignments])
        # sort all the variable names to keep a determined order
        # the outputs are things produced that are not consumed
        self._outputs = sorted(
            list((prod_var_names - free_var_names) | self._returns))
        # the inputs are variables that are not produced
        self._inputs = sorted(list(free_var_names - prod_var_names))
        # the intermediates are the produced variables that are not in the outputs
        self._inters = sorted(list(prod_var_names - set(self._outputs)))

    def c_block(self, code_block: CodeBlock = None, iteration_space: 'IterationSpace' = None,
                with_loops=False, with_comment=True, with_fixed=False, tags=None):
        if code_block is None:
            code_block = CodeBlock()
        # create loops from iteration space
        if with_loops:
            assert iteration_space, "must have space"
            if with_comment:
                # write some comments describing the iteration space
                code_block.comment(
                    f"Max shape: {iteration_space.shape} var shapes:")
                writer = code_block.start_long_comment()
                for shape_comment in [f'{name}: {shape}'
                                      for name, shape in iteration_space.var_shapes.items()]:
                    writer.write(shape_comment)
                writer.end()
                code_block.comment(
                    f'Iteration reduced to spaces {iteration_space.spaces}')
                code_block.comment(
                    f'Fixed spaces {iteration_space.fixed_spaces}')
                code_block.comment(
                    f'Parameteric spaces {iteration_space.parametric_spaces}')
                code_block.comment(
                    f'Paralelized space {iteration_space.paralellized_space}')
                code_block.comment(
                    f'Interior spaces {iteration_space.interior_spaces}')
            # write the loops
            for space in iteration_space.spaces:
                if not with_fixed and space in iteration_space.fixed_spaces:
                    continue
                code_block.write(
                    f"{iteration_space.c_for(space, with_fixed=with_fixed)} {{")
                code_block.indent()
        # write each assignment
        for var, func in self._assignments:
            this_tags = {} if tags is None else tags.copy()

            # write comment with quantization if present
            if with_comment:
                uvars = [f'{uvar.name}: {uvar.qrec}'
                         for uvar in func.unbound_variables.values()
                         if uvar.qrec]
                if uvars:
                    writer = code_block.start_long_comment()
                    writer.write('inputs')
                    for uvar in uvars:
                        writer.write(uvar)
                    writer.end()
                code_block.comment(f'{var.name} = {repr(func)}')
            # if iteration space is present pick up if this is a temporary or an output
            # assignment from that
            if iteration_space:
                if var.name in iteration_space.temporary_names:
                    this_tags[func] = (var, True)
                else:
                    this_tags[func] = (var, False)
            else:
                this_tags[func] = (var, var.name in self.intermediate_names)

            # The iteration space will be passed down the symbol structure
            func.tag = True
            func.c_block(code_block=code_block,
                         tags=this_tags,
                         iteration_space=iteration_space,
                         with_comment=with_comment)
            func.tag = False

        if with_loops:
            for space in iteration_space.spaces:
                if not with_fixed and space in iteration_space.fixed_spaces:
                    continue
                code_block.deindent()
                code_block.write("}")
        return code_block

    def quantize(self, quantizer: QuantizationHandlerBase, symbol_control, quantize_inputs=False, qtypes=None):
        funcs = []
        out_qrecs = {}
        in_qrecs = {}
        for var, func in self._assignments:
            qfunc, qrec = quantizer.quantize(
                func,
                symbol_control,
                quantize_inputs=quantize_inputs,
                prequantized_variables=out_qrecs,
                qtypes=qtypes)
            in_qrecs.update(qfunc.variable_quantization)
            if var.name in self._outputs:
                qfunc, qrec = quantizer.quantize_output(
                    func,
                    qfunc,
                    var,
                    symbol_control,
                    qrec,
                    quantize_inputs=quantize_inputs,
                    prequantized_variables=out_qrecs,
                    qtypes=qtypes)
            qfunc = qfunc.resolve()
            var = deepcopy(var)
            var.qrec = qrec
            funcs.append((var, qfunc(substitute=True)))
            out_qrecs[var.name] = qrec
        in_qrecs.update(out_qrecs)
        return Assignments(funcs, returns=self._returns, qrecs=in_qrecs)

    def extract_constants(self, other):
        if len(self) != len(other):
            return None
        res = ([], [])
        for self_ass, other_ass in zip(self, other):
            consts = self_ass[1].extract_constants(other_ass[1])
            res[0].extend(consts[0])
            res[1].extend(consts[1])
        return res

    def equivalent(self, other):
        if len(self) != len(other):
            return False
        return all(self_ass[1].equivalent(other_ass[1])
                   for self_ass, other_ass in zip(self, other))

    def find_class(self, sym_class):
        res = []
        for var, func in self._assignments:
            if isinstance(var, sym_class):
                res.append(var)
            res.extend(func.find_class(sym_class))
        return res

    def substitute(self, find_sym, sub_sym, update=False):
        has_found = False
        assignments = []
        for var, func in self._assignments:
            if func == find_sym:
                has_found = True
                assignments.append((var, sub_sym))
            elif var == find_sym:
                # TODO - should check returns
                has_found = True
                assignments.append((sub_sym, func))
            else:
                new_func = func.substitute(find_sym, sub_sym, update=update)
                if new_func is None:
                    assignments.append((var, func))
                else:
                    has_found = True
                    assignments.append((var, new_func))
        if has_found:
            if update:
                self._assignments = assignments
                self.update()
                return self
            return Assignments(
                assignments=assignments, qrecs=self.qrecs, returns=self._returns)
        return None

    def __getitem__(self, idx):
        return self._assignments[idx]

    def __len__(self) -> int:
        return len(self._assignments)

    def __iter__(self):
        return iter(self._assignments)

    def __call__(self, quantize_inputs=False, dequantize_outputs=False, **subs):
        subs = dict(subs)
        if quantize_inputs:
            subs = {name: self.qrecs[name].quantize_and_clip(val) if name in self.qrecs else val
                    for name, val in subs.items()}
        for var, func in self._assignments:
            subs[var.name] = func(
                dequantize_outputs=dequantize_outputs, **subs)
        res = dict(filter(lambda elem: elem[0] in self._outputs, subs.items()))
        if dequantize_outputs:
            if self.qrecs is None:
                raise ValueError('assignments are not quantized')
            res = {name: self.qrecs[name].dequantize(
                val) for name, val in res.items()}
        res = {name: val.reshape(
            self.output_shapes[name]) for name, val in res.items()}
        return res
