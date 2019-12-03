# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#
# author: martin.croome@greenwaves-technologies.com

#pylint: disable=attribute-defined-outside-init

from collections.abc import Iterable
from functools import reduce
from math import ceil, floor

import numpy as np


class DimError(Exception):
    pass

class NoSizeError(DimError):
    pass

class DimUnknownError(DimError):
    pass

class DimHasNoOrderError(DimError):
    pass

class DimHasNoNamesError(DimError):
    pass

class DimMissingKeyError(DimError):
    pass

class DimIncorrectKeyError(DimError):
    pass

class MissMatchedInputsError(DimError):
    pass

class MoreThanOneInputError(DimError):
    pass

class Dim():
    def __init__(self, shape=None, names=None, is_ordered=False, is_unknown=False):
        set_shape = shape if shape is not None else [] if names is None else [None] * len(names)
        super().__setattr__('_shape', set_shape)
        super().__setattr__('_names', names)
        super().__setattr__('_is_ordered', is_ordered)
        super().__setattr__('_is_unknown', is_unknown or\
            (shape is None) or\
            any(elem is None for elem in set_shape))
        super().__setattr__('_is_named', names is not None)

    @classmethod
    def unknown(cls) -> 'Dim':
        '''creates an unknown dimension'''
        return cls(is_unknown=True)

    @classmethod
    def unnamed(cls, shape: list, is_ordered=True) -> 'Dim':
        '''creates an ordered but unnamed dimension'''
        return cls(shape=list(shape), is_ordered=is_ordered)

    @classmethod
    def noshape(cls, order: list) -> 'Dim':
        '''creates an ordered dimension with unknown shape'''
        return cls(names=list(order), is_ordered=True, is_unknown=True)

    @classmethod
    def named(cls, order=False, **kwargs) -> 'Dim':
        '''creates a named and maybe ordered dimension'''

        if isinstance(order, bool):
            is_ordered = order
            order = kwargs.keys()
        elif isinstance(order, Iterable):
            is_ordered = True
        else:
            raise TypeError("order should be boolean or iterable")

        names = []
        shape = []
        for k in order:
            names.append(k)
            shape.append(kwargs[k])
        return cls(shape, names, is_ordered=is_ordered)

    @classmethod
    def named_ordered(cls, **kwargs) -> 'Dim':
        '''creates a named and ordered dimension'''
        return cls.named(order=kwargs.keys(), **kwargs)

    def srange(self, use_order=None, **kwargs):
        self._verify_is_ordered()
        self._verify_is_named()
        order = use_order or self.order
        slice_ = []
        for k in order:
            if k in kwargs:
                v = kwargs[k]
                if isinstance(v, Iterable):
                    slice_.append(slice(*v))
                elif isinstance(v, int):
                    slice_.append(slice(v, v+1, 1))
            else:
                slice_.append(slice(None))
        return tuple(slice_)

    def _verify_is_ordered(self):
        if not self._is_ordered:
            raise DimHasNoOrderError()

    def _verify_has_keys(self, keys):
        first_missing = next((k for k in keys if k not in self._names), None)
        if first_missing:
            raise DimMissingKeyError("{} not in dim".format(first_missing))

    def _verify_is_named(self, ):
        if not self._is_named:
            raise DimHasNoNamesError()

    def _verify_is_known(self, ):
        if self._is_unknown:
            raise DimUnknownError()

    def _update_unknown(self):
        object.__setattr__(self, '_is_unknown', any(elem is None for elem in self._shape))

    @property
    def shape(self) -> list:
        '''returns the shape of the dim'''
        self._verify_is_known()
        return self._shape

    @property
    def is_ordered(self) -> bool:
        return self._is_ordered

    @property
    def is_unknown(self) -> bool:
        return self._is_unknown

    @property
    def is_known(self) -> bool:
        return not self._is_unknown

    @is_known.setter
    def is_known(self, val: bool):
        self._is_unknown = not bool(val)

    @property
    def is_named(self) -> bool:
        return self._is_named

    @property
    def keys(self) -> list:
        self._verify_is_named()
        return self._names

    @property
    def order(self) -> list:
        self._verify_is_named()
        self._verify_is_ordered()
        return self._names

    def transpose(self, order):
        '''transpose dimension in order which is a list of indexes or list of names'''
        self._verify_is_ordered()
        if len(order) != len(self.shape):
            raise MissMatchedInputsError()
        # if the order is names then convert to indices
        if isinstance(order[0], str):
            self._verify_is_named()
            order = [self.keys.index(k) for k in order]
        object.__setattr__(self, '_shape', [self._shape[i] for i in order])
        if self.is_named:
            object.__setattr__(self, '_names', [self._names[i] for i in order])

    def apply_naming(self, other):
        self._verify_is_ordered()
        # pylint: disable=protected-access
        other._verify_is_named()

        # pylint: disable=protected-access
        if len(self._shape) != len(other._names):
            raise MissMatchedInputsError()
        # pylint: disable=protected-access
        object.__setattr__(self, '_names', other._names.copy())
        object.__setattr__(self, '_is_named', True)

    def impose_order(self, order) -> 'Dim':
        if not object.__getattribute__(self, '_is_unknown'):
            object.__setattr__(self, '_shape', [getattr(self, k) for k in order])
        if self.is_named:
            assert len(self._names) == len(order), "can't change length"
        object.__setattr__(self, '_names', list(order))
        object.__setattr__(self, '_is_ordered', True)
        return self

    def transpose_from_order(self, current) -> list:
        '''returns a transform order from current -> dim'''
        self._verify_is_ordered()
        self._verify_is_named()
        current_order = {k: v for v, k in enumerate(current)}
        return [current_order[k] for k in self._names]

    def transpose_to_order(self, desired) -> list:
        '''returns a transform order from dim -> desired'''
        self._verify_is_ordered()
        self._verify_is_named()
        current_order = {k: v for v, k in enumerate(self._names)}
        return [current_order[k] for k in desired]

    def get_order_idx(self, key) -> int:
        '''Ceturns the position of key in dim'''
        self._verify_is_ordered()
        self._verify_is_named()
        return self._names.index(key)

    def has_key(self, key) -> bool:
        '''Checks if dim has a key'''
        self._verify_is_named()
        return key in self._names

    def has_keys(self, keys: list) -> bool:
        '''Checks if dim has the keys in keys'''
        self._verify_is_named()
        return all(k in self._names for k in keys)

    def size(self) -> int:
        '''Get size of dim'''
        if self.is_unknown:
            return 0
        return reduce(lambda x, y: x*y, self._shape)

    def makeshape(self, order):
        self._verify_is_ordered()
        self._verify_is_named()
        newshape = []
        for elem in order:
            if isinstance(elem, str):
                newshape.append(getattr(self, elem))
            elif isinstance(elem, Iterable):
                newshape.append(reduce(lambda x, y: x * getattr(self, y), elem, 1))
            else:
                raise TypeError("elements of order should be k or list of keys")
        return newshape

    @staticmethod
    def combine(dims: Iterable, axis) -> 'Dim':
        '''Combines dim down an axis. all of the dimensions not on the combine
        axis should be the same
        '''
        if not dims:
            raise ValueError("nothing to combine")

        if any(dim.is_unknown for dim in dims):
            raise DimUnknownError("at least one dimension is unknown")

        base = dims[0].clone()
        if len(dims) == 1:
            return base

        if isinstance(axis, str):
            keys = set(base.keys)
            for i in range(1, len(dims)):
                keys &= set(dims[i].keys)

            if not keys:
                raise ValueError("nothing to combine")

            if axis not in keys:
                raise ValueError("axis not present in all keys")

            keys.remove(axis)
            cnt = getattr(base, axis)
            for i in range(1, len(dims)):
                dim = dims[i]
                if not all(getattr(dim, j) == getattr(base, j) for j in keys):
                    raise MissMatchedInputsError()
                cnt += getattr(dim, axis)
            setattr(base, axis, cnt)

            return base
        if isinstance(axis, int):
            cnt = base.shape[axis]
            for i in range(1, len(dims)):
                dim = dims[i]
                if len(dim.shape) != len(base.shape) or\
                    not all(dim.shape[j] == base.shape[j]\
                        for j in range(len(base.shape)) if j != axis):
                    raise MissMatchedInputsError()
                cnt += dim.shape[axis]
            base[axis] = cnt
            return base
        raise ValueError("bad axis")

    def clone(self, keys=None) -> 'Dim':
        '''clones the dim so modifications to the result will not affect this dim'''
        if not keys:
            return Dim(
                self._shape.copy(),
                self._names.copy() if self._names is not None else None,
                self._is_ordered,
                self._is_unknown
            )
        self._verify_is_named()
        names = [name for name in self._names if name in keys]
        shape = None if self._is_unknown else [getattr(self, k) for k in names]
        return Dim(shape, names, self._is_ordered, self._is_unknown)

    def set(self, *args, **kwargs):
        if args and isinstance(args[0], Iterable):
            args = args[0]
        if args:
            if self._is_named and self._names and len(self._names) != len(args):
                raise ValueError("incorrect length of settings for dim")
            self._shape = args.copy()
        elif kwargs:
            self._verify_is_named()
            for k, v in kwargs.items():
                setattr(self, k, v)
        else:
            raise ValueError("nothing to set")
        self._update_unknown()

    # SPECIAL METHODS

    def __getattr__(self, name):
        self._verify_is_named()
        try:
            idx = self._names.index(name)
            return self._shape[idx]
        except ValueError:
            raise AttributeError("{} not found".format(name))

    def __setattr__(self, name, val):
        self._verify_is_named()
        try:
            idx = self._names.index(name)
            self._shape[idx] = val
            self._update_unknown()
        except ValueError:
            raise AttributeError("{} not found".format(name))

    def _do_operation(self, other, op) -> 'Dim':
        if self._is_unknown:
            return Dim.unknown()

        if isinstance(other, int):
            res = self.clone()
            for k in self.keys:
                setattr(res, k, op(getattr(res, k), other))
            return res

        if isinstance(other, Dim):
            if other.is_unknown:
                return Dim.unknown()

            res = self.clone()

            for k in self.keys:
                if k in other.keys:
                    if getattr(self, k) is None:
                        setattr(res, k, getattr(other, k))
                    elif getattr(other, k) is not None:
                        setattr(res, k, op(getattr(self, k), getattr(other, k)))
            return res

        raise TypeError("Inapropriate types for operation")

    def __add__(self, other):
        return self._do_operation(other, lambda x, y: x+y)

    def __sub__(self, other):
        return self._do_operation(other, lambda x, y: x-y)

    def __mul__(self, other):
        return self._do_operation(other, lambda x, y: x*y)

    def __floordiv__(self, other):
        return self._do_operation(other, lambda x, y: x//y)

    # pylint: disable=too-many-return-statements
    def __eq__(self, other):
        if self.is_unknown:
            return False

        if isinstance(other, int):
            return all(getattr(self, k) == other for k in self.keys)

        if isinstance(other, Dim):
            if other.is_unknown or\
               self.is_ordered != other.is_ordered or\
               self.is_named != other.is_named:
                return False
            if self.is_named:
                if self.is_ordered:
                    return self.keys == other.keys and self.shape == other.shape
                all_keys = set.union(set(self.keys), set(other.keys))
                try:
                    return all(getattr(self, k) == getattr(other, k) for k in all_keys)
                except AttributeError:
                    return False
            else:
                return self.shape == other.shape

        raise TypeError("values cannot be compared")

    # collection methods
    def __len__(self):
        if not (self._is_named or self._is_ordered):
            raise DimUnknownError()
        return len(self._shape)

    def __getitem__(self, key):
        self._verify_is_ordered()
        return self._shape[key]

    def __setitem__(self, key, val):
        self._verify_is_ordered()
        self._shape[key] = val
        self._update_unknown()

    def __iter__(self):
        self._verify_is_ordered()
        return (v for v in self._shape)

    # string transformation
    def __str__(self):
        if self.is_named:
            if self.is_unknown:
                if self.is_ordered:
                    return 'x'.join(self._names)
                return "unknown"
            return 'x'.join(map(lambda k: str(getattr(self, k)), self._names))
        if self.is_unknown:
            return "unknown"
        return 'x'.join([str(v) for v in self._shape])

PAD_DIMS = ['t', 'b', 'l', 'r']
PAD_VERT_DIMS = ['t', 'b']
PAD_HORIZ_DIMS = ['l', 'r']

class PadDim(Dim):

    def __init__(self, *args, is_same=False):
        if not args:
            super().__init__(names=PAD_DIMS.copy(), is_ordered=True, is_unknown=True)
            object.__setattr__(self, '_same', is_same)
        else:
            if not all(isinstance(i, int) for i in args):
                raise TypeError("incorrect type for PadDim")

            if len(args) == 1:
                val = args[0]
                super().__init__([val] * 4, PAD_DIMS, is_ordered=True)
            elif len(args) == 2:
                super().__init__([args[0], args[0], args[1], args[1]], PAD_DIMS, is_ordered=True)
            elif len(args) == 4:
                super().__init__([args[0], args[1], args[2], args[3]], PAD_DIMS, is_ordered=True)
            else:
                raise ValueError("incorrect pad argument length")
            object.__setattr__(self, '_same', False)

    def height_width(self) -> Dim:
        '''return a dim representing the width and height'''
        return Dim.named_ordered(h=self.h, w=self.w)

    @classmethod
    def compute_pad_compatibility(cls, l, r):
        # left, right, balanced_left, balanced_right
        if l > 0:
            if r == 0:
                # all left
                if l > 1:
                    return [True, False, False, False]
                # balanced left is ok
                return [True, False, True, False]
            total = r + l
            half_pad = floor(total/2)
            if r > l:
                # should match balanced right
                if l != half_pad or r != total - half_pad:
                    raise ValueError("padding incompatible with autotiler - not balanced r>l")
                return [False, False, False, True]
            # should match balanced left
            if r != half_pad or l != total - half_pad:
                raise ValueError("padding incompatible with autotiler - not balanced l>r")
            if r == l:
                return [False, False, True, True]
            return [False, False, True, False]
        if r > 0:
            if r == 1:
                # balanced right is ok
                return [False, True, False, True]
            # all right
            return [False, True, False, False]
        # no pad. compatible with anything
        return [True, True, True, True]

    @classmethod
    def pad_compatibility_reduce(cls, *pad_compatibilities, err_msg=None):
        reduction = [all(compat) for compat in zip(*pad_compatibilities)]
        if not any(reduction):
            if err_msg:
                raise ValueError("padding is incompatible with autotiler - " + err_msg)
            return None
        return reduction


    @property
    def pad_compatibility(self):
        return self.pad_compatibility_reduce(
            self.compute_pad_compatibility(self.l, self.r),
            self.compute_pad_compatibility(self.t, self.b),
            err_msg="lr & tb need different settings"
        )

    @property
    def w(self) -> int:
        '''return total pad width'''
        self._verify_is_known()
        return self.l + self.r

    @property
    def h(self) -> int:
        '''return total pad height'''
        self._verify_is_known()
        return self.t + self.b

    def clone(self, keys=None):
        '''clone the paddim'''
        assert not keys
        if self.is_unknown:
            return PadDim(is_same=self.is_same)
        return PadDim(self.t, self.b, self.l, self.r, is_same=self.is_same)

    @classmethod
    def same(cls) -> 'PadDim':
        '''return a same padding'''
        return cls(is_same=True)

    @classmethod
    def valid(cls) -> 'PadDim':
        '''return a valid padding'''
        return cls(0)

    def numpy_pad_shape(self, in_dim: Dim) -> list:
        '''returns a list compatible with the numpy pad operation'''
        shape = []
        for axis in in_dim.keys:
            if axis == "c":
                shape.append((0, 0))
            elif axis == "h":
                shape.append((self.t, self.b))
            elif axis == "w":
                shape.append((self.l, self.r))
            else:
                raise DimIncorrectKeyError()
        return shape

    @property
    def is_same(self) -> bool:
        '''checks if PadDim is set same'''
        return self._same

    def calculate_same(self, in_dim, filt, stride) -> Dim:
        '''calculates the actual padding from the input dimension'''
        out_height = ceil(float(in_dim.h) / float(stride.h))
        out_width = ceil(float(in_dim.w) / float(stride.w))

        pad_along_height = max(
            (out_height - 1) * stride.h + filt.h - in_dim.h,
            0)
        pad_along_width = max(
            (out_width - 1) * stride.w + filt.w - in_dim.w,
            0)
        pad_top = pad_along_height // 2
        pad_left = pad_along_width // 2
        self.set(
            t=pad_top,
            b=pad_along_height - pad_top,
            l=pad_left,
            r=pad_along_width - pad_left
        )
        return self

    @property
    def has_end_h_pad(self) -> bool:
        '''checks if padded horizontally'''
        return self.b > 0 and self.t <= self.b

    @property
    def has_end_w_pad(self) -> bool:
        '''checks if padded vertically'''
        return self.r > 0 and self.l <= self.r

    def has_at_pad(self) -> bool:
        '''checks if padding is compatible with autotiler'''
        if self.t == 0 and self.b == 0 and self.l == 0 and self.r == 0:
            return False
        if self.has_end_h_pad and self.has_end_w_pad and self.t == self.l:
            return True
        raise AttributeError("Padding is probably not compatible with AutoTiler")

DEFAULT_CONVFILTER_DIMS = ['out_c', 'in_c', 'h', 'w']

class Conv2DFilterDim(Dim):

    def __init__(self, h, w, out_c, in_c=None, order=None):
        super().__init__([out_c, in_c, h, w], DEFAULT_CONVFILTER_DIMS.copy(), is_ordered=True)
        if order is not None and order != DEFAULT_CONVFILTER_DIMS:
            self.impose_order(order)

    @property
    def actual_shape(self):
        return self.shape

    def clone(self, keys=None) -> 'Conv2DFilterDim':
        assert not keys
        return Conv2DFilterDim(self.h, self.w, self.out_c, self.in_c, order=self.keys)

DEFAULT_FCFILTER_DIMS = ['out_c', 'in_c', 'h', 'w']

class FcFilterDim(Dim):

    def __init__(self, h, w, out_c, in_c=None, order=None):
        super().__init__([out_c, in_c, h, w], DEFAULT_FCFILTER_DIMS.copy(), is_ordered=True)
        if order is not None and order != DEFAULT_FCFILTER_DIMS:
            self.impose_order(order)

    @property
    def sz(self):
        return self.in_c * self.h * self.w

    @property
    def actual_shape(self):
        return [self.out_c, self.sz]

    @property
    def actual_order(self):
        # TODO - Fixme
        return ['out_c', 'sz']

    def srange(self, use_order=None, **kwargs):
        if use_order is None:
            use_order = self.actual_order
        return super().srange(use_order=use_order, **kwargs)

    def get_filter_dims(self):
        self._verify_is_ordered()
        self._verify_is_named()
        out_c_idx = self.get_order_idx('out_c')
        assert out_c_idx == 0 or out_c_idx == 3, "very strange shape for Linear filter"
        if out_c_idx == 0:
            return Dim.named_ordered(out_c=self.out_c, sz=self.h*self.w*self.in_c)
        return Dim.named_ordered(sz=self.h*self.w*self.in_c, out_c=self.out_c)

    def clone(self, keys=None) -> 'FcFilterDim':
        assert not keys
        return FcFilterDim(self.h, self.w, self.out_c, self.in_c, order=self.keys)

DEFAULT_2DDIMS = ['h', 'w']

class Dim2D(Dim):

    def __init__(self, *args, order=None):
        if len(args) == 1:
            super().__init__([args[0], args[0]], DEFAULT_2DDIMS.copy(), is_ordered=True)
        elif len(args) == 2:
            super().__init__([args[0], args[1]], DEFAULT_2DDIMS.copy(), is_ordered=True)
        else:
            raise ValueError()
        if order is not None and order != DEFAULT_2DDIMS:
            self.impose_order(order)

    def clone(self, keys=None) -> 'Dim2D':
        assert not keys
        return self.__class__(self.h, self.w, order=self.keys)

class StrideDim(Dim2D):
    pass

class PoolFilterDim(Dim2D):
    pass

class ScaleDim(Dim2D):
    pass

class DilationDim(Dim2D):
    pass
