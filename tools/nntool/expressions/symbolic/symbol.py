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

from collections.abc import Iterable
from functools import reduce

import numpy as np
from bfloat16 import bfloat16
from generation.code_block import CodeBlock
from quantization.qtype import DTYPE_GAP_CTYPE


class SymbolStats():
    def __init__(self, stats=None) -> None:
        self._stats = {} if stats is None else stats

    @property
    def stats(self):
        return self._stats

    def add_stat(self, sym, val):
        stats = self._stats.setdefault(sym.name, {})
        stats['max'] = max(np.max(val), stats.setdefault('max', float('-inf')))
        stats['min'] = min(np.min(val), stats.setdefault('min', float('inf')))

    def add_min_max(self, sym, min_val, max_val):
        stats = self._stats.setdefault(sym.name, {})
        stats['max'] = max_val
        stats['min'] = min_val

    def get_range(self, sym):
        stats = self._stats.get(sym.name)
        if not stats:
            return None
        min_val = stats.get('min')
        max_val = stats.get('max')
        if min_val is None or max_val is None:
            return None
        return (min_val, max_val)

    def get_max(self, sym):
        sym_range = self.get_range(sym)
        if sym_range:
            return max(abs(x) for x in sym_range)
        return None

    def reset_ranges(self):
        for stat in self._stats.values():
            for key in ['min', 'max']:
                if key in stat:
                    del stat[key]

class QRecBase():
    DTYPE_TO_CTYPE = {
        np.int8: 'int8_t',
        np.int16: 'int16_t',
        np.int32: 'int32_t',
        np.uint8: 'uint8_t',
        np.uint16: 'uint16_t',
        np.uint32: 'uint32_t',
        np.float32: 'float',
        bfloat16: 'F16',
        np.float16: 'F16'
    }
    def __init__(self, dtype=None) -> None:
        self._dtype = dtype

    @property
    def dtype(self):
        return self._dtype

    @dtype.setter
    def dtype(self, val):
        self._dtype = val

    @property
    def ctype(self):
        return self.DTYPE_TO_CTYPE[self.dtype]

class Symbol():
    NARGS = None
    CURRENT_CONTROL = SymbolStats()
    ENVIRONMENT = None
    COUNTS = {}
    C_HEADERS = []
    COPY_PROPS = tuple()

#pylint: disable=unused-argument
    def __init__(self, *args, name="", shape=None, dtype=np.float32, qrec: QRecBase = None, **kwargs):
        super(Symbol, self).__init__(**kwargs)
        if self.NARGS is not None and len(args) != self.NARGS:
            raise ValueError("wrong number of arguments to Symbol %s"%self.__class__.__name__)
        self._contents = [Symbol.symbolify(arg) for arg in args]
        self._name = name or self.get_name(self.__class__)
        self._dtype = dtype
        self._shape = shape
        self._qrec = qrec

    @classmethod
    def get_name(cls, cls_to_name):
        name = "%s%s" % (cls_to_name.__name__, cls.COUNTS.setdefault(cls_to_name, 0))
        cls.COUNTS[cls_to_name] += 1
        return name

    @property
    def qrec(self):
        return self._qrec

    @qrec.setter
    def qrec(self, val):
        self._qrec = val

    @property
    def contents(self):
        return self._contents

    @property
    def is_constant(self):
        return False

    def is_zero(self, qrec):
        return False

    @property
    def is_one(self, qrec):
        return False

    @property
    def shape(self):
        return self._shape

    @shape.setter
    def shape(self, val):
        if not isinstance(val, tuple):
            raise ValueError('shape should be a tuple')
        self._shape = val

    @property
    def dtype(self):
        if self._qrec is not None:
            return self._qrec.dtype
        return self._dtype

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, val):
        self._name = val

    @property
    def control(self):
        return self.CURRENT_CONTROL

    @property
    def value(self):
        return self._impl()

    @property
    def unbound_variables(self):
        return {}

    @property
    def ops(self):
        return 0

    @classmethod
    def set_default_control(cls, control):
        cls.CURRENT_CONTROL = control

    @staticmethod
    def extend_shapes(*shapes):
        if len(shapes) == 1:
            return list(shapes)
        max_length = max(len(x) for x in shapes)
        return [tuple([1] * (max_length - len(x)) + list(x)) for x in shapes]

    @staticmethod
    def broadcast(*shapes):
        if len(shapes) == 1:
            return shapes[0]
        if any(shape is None for shape in shapes):
            return None
        extended_shapes = Symbol.extend_shapes(*shapes)
        base = tuple([max([dim for dim in dims]) for dims in zip(*extended_shapes)])
        if any(any(dim != 1 and base[idx] != dim for idx, dim in enumerate(shape)) for shape in extended_shapes):
            raise ValueError('shapes cannot be broadcast')
        return base

#pylint: disable=unused-argument
    def _eval(self, *args, **kwargs):
        return self

    def _impl(self, *args, **kwargs):
        raise NotImplementedError('_impl has not been overriden')

    def _resolve(self, **kwargs):
        """Can be overridden to implement custom resolver"""
        return self

    def _calculate(self, calculate_ranges=False, **kwargs):
        """Can be overridden to implement custom calculator"""
        raise NotImplementedError()

    def _collect_globals(self) -> dict:
        if self.ENVIRONMENT is not None:
            return self.ENVIRONMENT
        return {}

    def resolve(self, **kwargs):
        """Given a set of substitions for variable in kwargs resolve all variables"""
        return self._resolve(**kwargs)

    def calculate(self, calculate_ranges=False, **kwargs):
        """Given a set of substitions for variable in kwargs calculate a result"""
        return self._calculate(calculate_ranges=calculate_ranges, **kwargs)

    def collect_globals(self) -> dict:
        """Returns a dict of globals necessary to execute a lambda of this symbol. Globals
        can be attached to a subclass of symbol using the @environment decorator."""
        return self._collect_globals()

    def clone_with(self, *args):
        return self.__class__(*args,
                              dtype=self.dtype,
                              name=self.name, 
                              qrec=self.qrec,
                              shape=self._shape, # access private property to pick up None value
                              **{k: getattr(self, k) for k in self.COPY_PROPS})

    def find(self, name):
        if name == self.name:
            return self
        for elem in self.contents:
            res = elem.find(name)
            if res:
                return res
        return None

    @staticmethod
    def environment(globals_dict):
        return Symbol.property_register("ENVIRONMENT", globals_dict)

    @staticmethod
    def nargs(op):
        return Symbol.property_register("NARGS", op)

    @staticmethod
    def c_headers(*headers):
        return Symbol.property_register("C_HEADERS", headers)

    @property
    def sym_c_headers(self):
        return self.C_HEADERS

    @property
    def c_header_set(self):
        return set(self.sym_c_headers) | set([header for sym in self.contents for header in sym.c_header_set])

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

    @staticmethod
    def symbolify(sym):
        if isinstance(sym, Symbol):
            return sym
        if isinstance(sym, str):
            return Variable(sym)
        if isinstance(sym, str):
            return Variable(sym)
        if isinstance(sym, (np.ndarray, float, int)):
            return Constant(sym)
        return sym

    @staticmethod
    def handles(special_function, **kwargs):
        def decorator(cls):
            def handler(*args):
                args = [Symbol.symbolify(arg) for arg in args]
                return cls(*args, **kwargs)
            setattr(Symbol, special_function, handler)
            return cls
        return decorator

    @staticmethod
    def handlesr(special_function, **kwargs):
        def decorator(cls):
            def handler(*args):
                args = [Symbol.symbolify(arg) for arg in args]
                return cls(*tuple(reversed(args)), **kwargs)
            setattr(Symbol, special_function, handler)
            return cls
        return decorator

    @staticmethod
    def copy_props(*props):
        def decorator(cls):
            setattr(cls, 'COPY_PROPS', tuple(list(cls.COPY_PROPS) + list(props)))
            return cls
        return decorator

    def _py_expr(self, *args, **kwargs):
        raise ValueError('no python representation is available')

    def _c_expr(self, *args, **kwargs):
        return self._py_expr(*args, **kwargs)

    def py_expr(self, *args, **kwargs):
        return self._py_expr(*args, **kwargs)

    def c_expr(self, *args, **kwargs):
        return self._c_expr([], **kwargs)

    def c_block(self, code_block=None, **kwargs):
        if code_block is None:
            code_block = CodeBlock
        code_block.write(self.c_expr)

    def _equivalent(self, other) -> bool:
        pass

    def equivalent(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False
        if not all(getattr(self, prop) == getattr(other, prop) for prop in self.COPY_PROPS):
            return False
        return self._equivalent(other)

    def __call__(self, *args, **kwargs):
        if self.NARGS is not None and len(args) != self.NARGS:
            raise ValueError("incorrect number of arguments")
        return self._impl(self, *args, **kwargs)

    def __repr__(self) -> str:
        if self.NARGS is None:
            return "nargs on %s is None but __str__ is not overriden"%self.__class__.__name__
        return "%s(%s)" % (self.__class__.__name__, ", ".join(self._contents[i].__repr__()
                                                              for i in range(self.NARGS)))

    def __str__(self) -> str:
        return self.__repr__()


#pylint: disable=invalid-name
nargs = Symbol.nargs
handles = Symbol.handles
handlesr = Symbol.handlesr
environment = Symbol.environment
c_headers = Symbol.c_headers
copy_props = Symbol.copy_props

def print_float_constant(val):
    val = float(val)
    if int(val) == val:
        return f"{val:.1f}f"
    return f"{val:g}f"

@environment({
    'float32': np.float32,
    'int32': np.int32,
    'int16': np.int16,
    'int8': np.int8,
    'array': np.array
})
@nargs(None)
class Constant(Symbol):
    #pylint: disable=arguments-differ
    def __new__(cls, value, **kwargs):
        del kwargs
        if isinstance(value, float) and value in Rational.RATIONALS:
            return object.__new__(Rational)

        return object.__new__(cls)

# pylint: disable=super-init-not-called
    def __init__(self, value, dtype=np.float32, is_rational=False, **kwargs):
        if hasattr(value, "dtype"):
            dtype = value.dtype
        super(Constant, self).__init__(dtype=dtype)
        if value is None:
            if not is_rational:
                raise ValueError('attempt to create None Constant')
        else:
            self._value = np.atleast_1d(value).astype(dtype)

    @property
    def sym_c_headers(self):
        if self.qrec and self.qrec.ctype == "F16":
            return [
                '"CNN_FloatType.h"'
            ]
        return self.C_HEADERS

    @property
    def is_constant(self):
        return True

    def is_zero(self, qrec):
        if qrec is None:
            return np.all(self.value == 0)
        return np.all(self.value == qrec.zero_point)

    def is_one(self, qrec):
        if qrec is None:
            return np.all(self.value == 1)
        raise NotImplementedError()

    @property
    def is_scalar(self):
        return self._value.shape in [(), (1,)]

    @property
    def shape(self):
        if self._shape is None:
            return self._value.shape
        return self._shape

    def _equivalent(self, other) -> bool:
        return np.all(self.value == other.value)

    @property
    def _c_expr_func(self):
        return self._c_expr

    def _calculate(self, calculate_ranges=False, **kwargs):
        if calculate_ranges:
            self.control.add_stat(self, self.value)
        return self

    def _impl(self, *args, **kwargs):
        if self.qrec:
            return self._value.astype(self.qrec.dtype)
        return self._value

    def _py_expr(self, *args, **kwargs):
        return self._value.__repr__()

    def _c_expr(self, *args, **kwargs):
        if len(self.value.shape) == 0:
            val = self._value
        elif len(self.value) == 1:
            val = self._value[0]
        else:
            raise ValueError('cannot produce c code for functions with array constants')

        if self.dtype in [bfloat16, np.float16]:
            return f"(F16){print_float_constant(val)}"
        elif self.dtype == np.float32:
            return print_float_constant(val)
        return val

    def __repr__(self) -> str:
        return str(self._value)

class Rational(Constant):
    RATIONALS = {
        0.5: (1, 2),
        0.25: (1, 4),
        0.1: (1, 10),
    }

# pylint: disable=arguments-differ,unused-argument
    def __new__(cls, *args, **kwargs):
        return object.__new__(cls)

    def __init__(self, *args, dtype=np.int32, **kwargs):
        super(Rational, self).__init__(None, is_rational=True, **kwargs)
        if len(args) == 1:
            args = Rational.RATIONALS[args[0]]
        if len(args) != 2:
            raise ValueError('Ration expects 2 arguments - the numerator and denominator')
        self._value = tuple(np.atleast_1d(x).astype(dtype)
                            for x in args)
        if not np.all(np.floor(x) == x for x in self._value):
            raise ValueError('attempt to create rational with fractional elements')

    @property
    def shape(self):
        if self._shape is None:
            return self.broadcast(*self._value)
        return self._shape

    def _impl(self, *args, **kwargs):
        return np.divide(*self._value)

    def _py_expr(self, *args, **kwargs):
        return "np.divide(%s, %s)" % self._value

    def _c_expr(self, *args, **kwargs):
        return "%s/%s" % tuple(int(x) for x in self._value)

    def __repr__(self) -> str:
        return "%s/%s" % tuple(str(x) for x in self._value)


@nargs(None)
class Variable(Symbol):

    # pylint: disable=super-init-not-called
    def __init__(self, var_name, shape=None, symbol_binding=None, name="", **kwargs):
        if not name:
            name = var_name
        super(Variable, self).__init__(name=name, **kwargs)
        self._symbol_binding = symbol_binding
        self._shape = shape
        self._index_vars = None
        self._ispointer = False

    @property
    def shape(self):
        if self._symbol_binding:
            return self._symbol_binding.shape
        return super().shape

    @shape.setter
    def shape(self, val):
        if self._symbol_binding:
            self._symbol_binding.shape = val
        else:
            self._shape = val

    @property
    def dtype(self):
        if self._symbol_binding:
            return self._symbol_binding.dtype
        return super().dtype

    @property
    def sym_c_headers(self):
        if self.qrec and self.qrec.ctype == "F16":
            return [
                '"CNN_FloatType.h"'
            ]
        return self.C_HEADERS

    @property
    def value(self):
        raise ValueError("Variable has no value")

    @property
    def unbound_variables(self):
        return {self._name: self}

    @property
    def index_vars(self):
        return self._index_vars

    @index_vars.setter
    def index_vars(self, val):
        self._index_vars = val

    @property
    def ispointer(self):
        return self._ispointer

    @ispointer.setter
    def ispointer(self, val):
        self._ispointer = val

    @property
    def variable_quantization(self):
        return {self.name: self._qrec}

    def _impl(self, *args, **kwargs):
        if self.name in kwargs:
            calculate_ranges = kwargs.get('calculate_ranges', False)
            val = np.array(kwargs[self.name])
            if self.shape is not None:
                val = np.reshape(val, self.shape)
            quantize_inputs = kwargs.get('quantize_inputs', False)
            if quantize_inputs is True or isinstance(quantize_inputs, Iterable) and self.name in quantize_inputs:
                if self.qrec is None:
                    raise ValueError("can't quantize %s. no quantization record is set."%self.name)
                val = self.qrec.quantize_and_clip(val)
            if calculate_ranges:
                self.control.add_stat(self, val)
            return val
        return self

    def _resolve(self, **kwargs):
        """Given a set of substitions for variable in kwargs resolve all variables"""
        if self.name in kwargs:
            # if substituting for another variable just return that variable
            if isinstance(kwargs[self.name], Variable):
                return kwargs[self.name]
            # if substituting for a symbol then try to resolve that
            if isinstance(kwargs[self.name], Symbol):
                return kwargs[self.name].resolve(**kwargs)
            # try to resolve to a constant
            return Constant(self._impl(**kwargs), shape=self.shape)
        return self

    def _calculate(self, calculate_ranges=False, **kwargs):
        res = self._resolve(**kwargs)
        if not isinstance(res, Constant):
            raise ValueError(
                "unable to calculate this function. there is no value for %s" % self.name)
        if calculate_ranges:
            self.control.add_stat(self, res.value)
        return res

    def _py_expr(self, *args, **kwargs):
        if self.shape:
            return "np.reshape(np.array(%s), %s)" % (self.name, self.shape)
        return "np.array(%s)" % self.name

    def _equivalent(self, other) -> bool:
        return self.shape == other.shape and self.dtype == other.dtype

    def __eq__(self, o: object) -> bool:
        if isinstance(o, Variable):
            return o.name == self.name
        return False

    def __hash__(self) -> int:
        return self.name.__hash__()

    @staticmethod
    def gen_index(index_vars):
        # generates d0*D1*D2+d1*D2+d2 from d0, d1, d2 and so on
        names = sorted(x[0].name for x in index_vars)
        return "+".join(
            "*".join(elems)
            for elems in reduce(
                lambda s, x: [[x]+[s[0][0].upper()]+s[0][1::]] + s, reversed(names[0:-1:]), [[names[-1]]]))

#pylint: disable=arguments-differ
    def _c_expr(self, *args, declare=False, dtype=None, pointer=None, iteration_space=None, **kwargs):
        if iteration_space:
            return iteration_space.c_indexed_var(self.name)
        if pointer is None:
            pointer = self._ispointer
        if declare:
            if dtype:
                return "%s %s%s"%(DTYPE_GAP_CTYPE[dtype], "*" if pointer else "", self.name)
            else:
                return "%s %s%s"%(DTYPE_GAP_CTYPE[self.dtype], "*" if pointer else "", self.name)
        if self._index_vars is not None:
            if self._index_vars:
                return f"{self.name}[{self.gen_index(self._index_vars)}]"
                # return "%s[%s]"%(self.name, "+".join("%s*%s"%(v[0].name, v[1]) for v in self._index_vars))
            else:
                if pointer:
                    return "(*%s)"%self.name
                return self.name
        return self.name

    def __repr__(self) -> str:
        return f'{self.name}'
