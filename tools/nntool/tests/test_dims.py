# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#
# author: martin.croome@greenwaves-technologies.com

from graph.dim import Dim, PadDim, Conv2DFilterDim, StrideDim, DimHasNoOrderError
from copy import deepcopy

def test_creation1():
    dim1 = Dim()
    assert dim1.is_unknown
    assert not dim1.is_named
    assert not dim1.is_ordered

def test_creation2():
    dim1 = Dim.named(a=1, b=2, c=3)
    assert not dim1.is_unknown
    assert dim1.is_named
    assert not dim1.is_ordered
    assert dim1.a == 1 and dim1.b == 2 and dim1.c == 3
    assert dim1.just_has_keys(['a', 'b', 'c'])
    assert not dim1.just_has_keys(['a', 'b', 'c', 'd'])
    assert not dim1.just_has_keys(['a', 'b', 'd'])
    assert not dim1.just_has_keys(['a', 'b'])

def test_creation3():
    dim1 = Dim.named(a=1, b=2, c=3, order=['c', 'b', 'a'])
    assert not dim1.is_unknown
    assert dim1.is_named
    assert dim1.is_ordered
    assert dim1.a == 1 and dim1.b == 2 and dim1.c == 3
    assert dim1.shape == [3, 2, 1]
    assert dim1.get_order_idx('b') == 1
    caught = False
    try:
        dim1.z = 5
    except AttributeError:
        caught = True
    assert caught

def test_transpose():
    dim1 = Dim.named(a=1, b=2, c=3, order=['c', 'b', 'a'])
    dim1.transpose([2, 1, 0])
    assert dim1.order == ['a', 'b', 'c']
    assert dim1.shape == [1, 2, 3]
    dim1.transpose(['b', 'a', 'c'])
    assert dim1.order == ['b', 'a', 'c']
    assert dim1.shape == [2, 1, 3]

def test_creation4():
    dim1 = Dim.named(a=1, c=3, b=2, order=True)
    assert not dim1.is_unknown
    assert dim1.is_named
    assert dim1.is_ordered
    assert dim1.a == 1 and dim1.b == 2 and dim1.c == 3
    assert dim1.shape == [1, 3, 2]

def test_creation5():
    dim1 = Dim.named_ordered(a=1, c=3, b=2)
    assert not dim1.is_unknown
    assert dim1.is_named
    assert dim1.is_ordered
    assert dim1.a == 1 and dim1.b == 2 and dim1.c == 3
    assert dim1.shape == [1, 3, 2]

def test_creation6():
    dim1 = Dim.unnamed((1, 3, 2))
    assert not dim1.is_unknown
    assert not dim1.is_named
    assert dim1.is_ordered
    assert dim1[0] == 1 and dim1[1] == 3 and dim1[2] == 2
    dim1[0] = 2
    assert dim1[0] == 2

def test_creation7():
    dim1 = Dim.unnamed((1, 3, 2), is_ordered=False)
    assert not dim1.is_unknown
    assert not dim1.is_named
    assert not dim1.is_ordered
    waserror = False
    # from a safety perspective unordered dims can't be indexed
    try:
        dim1[0] = 1
    except DimHasNoOrderError:
        waserror = True
    assert waserror

def test_creation8():
    dim1 = Dim.noshape(('a', 'b', 'c'))
    assert dim1.is_unknown
    assert dim1.is_named
    assert dim1.is_ordered
    waserror = False
    # from a safety perspective unordered dims can't be indexed
    try:
        dim1[0] = 1
    except DimHasNoOrderError:
        waserror = True
    assert not waserror
    assert dim1.is_unknown
    # set all dimensions and the dim becomes known
    try:
        dim1.b = 1
        dim1.c = 1
    except DimHasNoOrderError:
        waserror = True
    assert not waserror
    assert not dim1.is_unknown

def test_operation1():
    dim1 = Dim.named_ordered(a=1, c=3, b=2)
    dim2 = Dim.named_ordered(a=1, c=3, b=2)
    dim3 = dim1 + dim2
    assert dim3.is_named
    assert dim3.is_ordered
    assert dim3.a == 2 and dim3.b == 4 and dim3.c == 6
    assert dim3.shape == [2, 6, 4]
    dim3.a = 2
    assert dim1.a == 1 and dim2.a == 1

def test_operation2():
    dim1 = Dim.named_ordered(a=1, c=3, b=2)
    dim2 = Dim.named_ordered(a=1, c=3, b=2)
    dim3 = dim1 - dim2
    assert dim3.is_named
    assert dim3.is_ordered
    assert dim3.size() == 0

def test_combine1():
    dim1 = Dim.named_ordered(a=1, c=3, b=2)
    dim2 = Dim.named_ordered(a=1, c=3, b=2)
    dim3 = Dim.combine((dim1, dim2), 'c')
    assert dim3.shape == [1, 6, 2]
    dim3.c = 4
    assert dim1.c == 3 and dim2.c == 3

def test_paddim():
    dim1 = PadDim(1)
    assert not dim1.is_same
    assert dim1.h == 2 and dim1.w == 2
    assert dim1.l == 1 and dim1.r == 1 and dim1.t == 1 and dim1.b == 1
    assert dim1.numpy_pad_shape(Dim.named_ordered(w=10, h=10)) == [(1, 1), (1, 1)]
    stride_dim = StrideDim(1)
    filt_dim = Conv2DFilterDim(5, 5, 1, 1)
    in_dim = Dim.named_ordered(c=1, h=20, w=20)
    dim1 = PadDim.same()
    dim1.calculate_same(in_dim, filt_dim, stride_dim)
    assert dim1.shape == [2, 2, 2, 2]

def test_paddim_compat():
    red1 = PadDim.pad_compatibility_reduce([True, True, False, True], [True, True, False, True])
    assert red1 == [True, True, False, True]
    red1 = PadDim.pad_compatibility_reduce([True, True, False, True], [True, True, False, False])
    assert red1 == [True, True, False, False]
    dim1 = PadDim(1)
    dim2 = PadDim(1, 2, 1, 2)
    compat1 = dim1.pad_compatibility
    assert compat1 == [False, False, True, True]    
    compat2 = dim2.pad_compatibility
    assert compat2 == [False, False, False, True]    
    red2 = PadDim.pad_compatibility_reduce(compat1, compat2)
    assert red2 == [False, False, False, True]    

def test_combine2():
    dim1 = Dim.unnamed((1, 12800, 2))
    dim2 = Dim.unnamed((1, 3200, 2))
    dim3 = Dim.unnamed((1, 800, 2))
    dim4 = Dim.unnamed((1, 200, 2))
    res = Dim.combine((dim1, dim2, dim3, dim4), 1)
    assert res.shape == [1, 17000, 2]

def test_broadcast1():
    dim1 = Dim.unnamed((1, 12800, 2))
    dim2 = Dim.unnamed((1, 3200, 2))
    dim3 = Dim.unnamed((1, 800, 2))
    dim4 = Dim.unnamed((1, 200, 2))
    res = Dim.broadcast((dim1, dim2, dim3, dim4))
    assert res.shape == [1, 17000, 2]

def test_broadcast2():
    dim1 = Dim.unnamed([6])
    dim2 = Dim.unnamed((3, 2, 6))
    dim3 = Dim.unnamed((4, 2, 6))
    res = Dim.broadcast((dim1, dim2, dim3))
    assert res.shape == [8, 2, 6]

def test_broadcast3():
    dim1 = Dim.named(['c'], c=6)
    dim2 = Dim.named(['h', 'w', 'c'], h=3, w=2, c=6)
    dim3 = Dim.named(['h', 'w', 'c'], h=4, w=2, c=6)
    res = Dim.broadcast((dim1, dim2, dim3))
    assert res.shape == [8, 2, 6] and res.is_named

def test_candeepcopy():
    dim1 = Dim.named(a=1, b=2, c=3, order=['c', 'b', 'a'])
    dim2 = deepcopy(dim1)
    assert dim1 == dim2
