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

from functools import cmp_to_key, reduce
from typing import Any, Sequence, Tuple

import numpy as np
from generation.code_block import CodeBlock
from expressions.symbolic.quantization_base import QuantizationHandlerBase
from expressions.symbolic.symbol import Symbol, Variable


def prod(x):
    return reduce(lambda a, b: a * b, x, 1)


class FunctionCollection():

    def __init__(self, functions: Sequence[Tuple[Variable, Symbol]], qrecs=None) -> None:
        self._qrecs = qrecs
        # save map from produced variable to function
        self._functions = {var: symbol for var, symbol in functions}
        # now create a map with producted variable name to free variables in function
        self._freevars = {var.name: set([name for name in func.unbound_variables.keys()])
                          for var, func in self._functions.items()}
        # make a set of all the variable names
        free_var_names = set().union(*self._freevars.values())
        # these are all the produced variables
        prod_var_names = set([var.name for var in self._functions.keys()])
        # the outputs are things produced that are not consumed
        self._outputs = list(prod_var_names - free_var_names)
        # the inputs are variables that are not produced
        self._inputs = list(free_var_names - prod_var_names)
        # the intermediates are the produced variables that are not in the outputs
        self._inters = list(prod_var_names - set(self._outputs))
        self._vars = {}
        # vars will contain variable name -> variable dict
        for res_symbol, func in self._functions.items():
            for name, symbol in func.unbound_variables.items():
                if name in self._vars:
                    if self._vars[name] != symbol:
                        raise ValueError('%s points to more than one variable' % name)
                else:
                    self._vars[name] = symbol
            if res_symbol.name in self._vars:
                if self._vars[res_symbol.name] != res_symbol:
                    raise ValueError('%s points to more than one variable' % res_symbol.name)
            else:
                self._vars[res_symbol.name] = res_symbol
        # iterators contains list of iterators
        self._iterators = None
        self._variable_indexes = None
        self._create_indexes()
        for var_name in self._inputs + self._outputs:
            sym = self._vars[var_name]
            sym.ispointer = True
            sym.index_vars = self._index_bases[self._variable_indexes[var_name]]
        for var_name in self._inters:
            sym = self._vars[var_name]
            sym.ispointer = False

    @property
    def input_names(self):
        return self._inputs

    @property
    def qrecs(self):
        return self._qrecs

    @property
    def output_names(self):
        return self._outputs

    @property
    def intermediate_names(self):
        return self._inters

    @property
    def var_shapes(self):
        return {sym.name: sym.shape for sym in self._vars.values()}

    @property
    def kernel_dims(self):
        return len(self._iterators)

    @property
    def iterators(self):
        return self._iterators

    @property
    def variable_indexes(self):
        return self._variable_indexes

    @property
    def variables(self):
        return self._vars

    @property
    def functions(self):
        return self._functions

    @property
    def ops(self):
        # add one for the assignment
        return sum(sym.ops + 1 for sym in self._functions.values())

    @property
    def c_header_set(self):
        return set().union(*[func.c_header_set for func in self._functions.values()])

    @staticmethod
    def split_indexes(unique_axis_groups):
        res = set()
        uaq = sorted(unique_axis_groups, key=len)
        stable = False
        while not stable and uaq:
            stable = True
            res = set()
            first = uaq[0]
            res.add(first)
            for item in uaq[1::]:
                if first < item:
                    res.add(item - first)
                    stable = False
                else:
                    res.add(item)
            uaq = sorted(list(res), key=len)
        return uaq

    def _create_indexes(self):
        """Discovers iteration space for basic kernel. All 1 and uniformly indexed shapes are reduced"""
        # broadcast all shapes to maximum length
        max_len = max([len(val) for val in self.var_shapes.values()])
        shapes = np.array([[1]*(max_len - len(val)) + list(val)
                           for val in self.var_shapes.values()])
        # get the maximal shape which will be the out shape
        max_shape = tuple(shapes.max(axis=0))
        # get the index list of axis > 1 per input shape
        node_axis = {n: frozenset(np.nonzero(np.array([1]*(max_len - len(shape)) + list(shape)) != 1)[
            0]) for n, shape in self.var_shapes.items()}
        # get the unique variants
        unique_axis_groups = np.unique(np.array(list(node_axis.values())))
        # remove the empty set
        unique_axis_groups = np.delete(
            unique_axis_groups, np.where(unique_axis_groups == frozenset()))
        # reduce all axis that are supersets of others
        unique_indexes = sorted(self.split_indexes(unique_axis_groups),
                                key=lambda x: next(i for i in x))
        idx_names = ["_".join(["d%s" % idx for idx in sorted(list(idxes))])
                     for idxes in unique_indexes]
        idx_dims = [reduce(lambda x, y: x*max_shape[y], idxes, 1) for idxes in unique_indexes]
        self._iterators = [Variable(idx_name, shape=tuple([idx_dim]), dtype=np.int32)
                           for idx_name, idx_dim in zip(idx_names, idx_dims)]
        self._variable_indexes = {n: tuple([idx for idx, idx_set in enumerate(
            unique_indexes) if idx_set <= node_axis[n]]) for n in self.var_shapes}
        # creates a map from the dimensions indexed to a list of tuples
        # containing the index variable and it's multiplier
        self._index_bases = {tuple(base): [(self._iterators[dim], prod(idx_dims[i]
                                                                       for i in base[dim_idx+1::]))
                                           for dim_idx, dim in enumerate(base)]
                             for base in set(self._variable_indexes.values())}

    @property
    def execution_order(self):
        func_deps = [(self._inters + list(func.unbound_variables.keys()), var)
                     for var, func in self._functions.items()]

        def sort_func(x, y):
            if y[1].name in x[0]:
                return 1
            if x[1].name in y[0]:
                return -1
            return 0
        return tuple([elem[1] for elem in sorted(func_deps, key=cmp_to_key(sort_func))])

    def get_iterator_vars(self):
        iters = []
        for depth, var in enumerate(self._iterators):
            if depth == 0:
                iters.extend([('First', 0), ('Last', var.shape[0])])
            elif depth == 1:
                if self.kernel_dims == 2:
                    iters.append(('W', var.shape[0]))
                else:
                    iters.append(('H', var.shape[0]))
            elif depth == 2:
                iters.append(('W', var.shape[0]))
        return iters

    def create_kernel(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()
        execution_order = self.execution_order
        index_dependencies = {var_name: set([index
                                             for ivar in ivars
                                             for index in self._variable_indexes[ivar]])
                              for var_name, ivars in self._freevars.items()}
        for var_name, indexes in index_dependencies.items():
            indexes.update(set(self._variable_indexes[var_name]))
        index_dependencies = {var_name: max(indexes) if indexes else 0
                              for var_name, indexes in index_dependencies.items()}
        produced_idx = 0
        if self.kernel_dims:
            for depth in range(self.kernel_dims):
                if depth == 0:
                    code_block.write(
                        'for ({0}=First; {1}<Last; {1}++) {{',
                        self.iterators[depth].c_expr(declare=True),
                        self.iterators[depth].c_expr())
                else:
                    if depth == 1:
                        code_block.write(
                            'for ({0}=0; {1}<{2}; {1}++) {{',
                            self.iterators[depth].c_expr(declare=True),
                            self.iterators[depth].c_expr(),
                            "W" if self.kernel_dims == 2 else "H")
                    elif depth == 2:
                        code_block.write(
                            'for ({0}=0; {1}<W; {1}++) {{',
                            self.iterators[depth].c_expr(declare=True),
                            self.iterators[depth].c_expr())
                code_block.indent()
                produced_idx = self.produce_functions(
                    produced_idx, execution_order, index_dependencies, depth, code_block)
            for depth in reversed(range(self.kernel_dims)):
                produced_idx = self.produce_functions(
                    produced_idx, execution_order, index_dependencies, depth, code_block)

                code_block.deindent()
                code_block.write('}}')
        else:
            produced_idx = self.produce_functions(
                produced_idx, execution_order, index_dependencies, 0, code_block)

        code_block.write('')
        assert produced_idx >= len(execution_order)
        return code_block

    def produce_functions(self, produced_idx, execution_order, index_dependencies, depth, code_block):
        while (produced_idx < len(execution_order) and
               index_dependencies[execution_order[produced_idx].name] == depth):
            var = execution_order[produced_idx]
            declare = var.name in self.intermediate_names
            code_block.write("{} = {};", var.c_expr(declare=declare, dtype=var.dtype),
                             self._functions[var].c_expr())
            produced_idx += 1
        return produced_idx

    def quantize(self, quantizer: QuantizationHandlerBase, symbol_control, quantize_inputs=False, qtypes=None):
        funcs = []
        out_qrecs = {}
        in_qrecs = {}
        for var, func in self._functions.items():
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

            funcs.append((var, qfunc(substitute=True)))
            out_qrecs[var.name] = qrec
        in_qrecs.update(out_qrecs)
        return FunctionCollection(funcs, qrecs=in_qrecs)

    def __call__(self, *args: Any, dequantize_outputs=False, quantize_inputs=False, **kwargs: Any) -> Any:
        if quantize_inputs:
            quantize_inputs = self._inputs
        for sym_var in self.execution_order:
            kwargs[sym_var.name] = self._functions[sym_var](
                *args, quantize_inputs=quantize_inputs, **kwargs)
        if dequantize_outputs:
            if self._qrecs is None:
                raise ValueError('function collection is not quantized')
            return {name: self._qrecs[name].dequantize(kwargs[name])
                    for name in self._outputs}
        return {name: kwargs[name] for name in self._outputs}
