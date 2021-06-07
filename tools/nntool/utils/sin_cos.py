# Derived from https://www.nullhardware.com/blog/fixed-point-sine-and-cosine-for-embedded-systems/

import numpy as np


def fpsin(arr):
    # arr is a Q15 in radians from -2pi -> 2pi
    # result is Q3.12

    arr = arr << 1
    c = (arr<0)
    arr = np.where(arr == (arr|0x4000), np.array(1<<15, dtype=np.int16) - arr, arr)
    arr = (arr & 0x7fff) >> 1

    A1 = np.array(3370945099, dtype=np.uint32)
    B1 = np.array(2746362156, dtype=np.uint32)
    C1 = np.array(292421, dtype=np.uint32)

    n = 13
    p = 32
    q = 31

    r = 3
    a = 12

    arr = arr.astype(np.uint32)
    y = (C1*(arr))>>n
    y = B1 - ((arr*y)>>r)
    y = arr * (y>>n)
    y = arr * (y>>n)
    y = A1 - (y>>(p-q))
    y = arr * (y>>n)
    y = (y+(np.array(1, dtype=np.uint32)<<(q-a-1)))>>(q-a) # Rounding
    y = y.astype(np.int16)
    y[c] = -y[c]
    return y

def fpcos(arr):
    return fpsin(((arr.astype(np.int16)) + np.array(8192, dtype=np.uint16)).astype(np.int16))
