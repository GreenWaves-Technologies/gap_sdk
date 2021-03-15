from math import pi, floor, sin, cos
import numpy as np


class CordicFp():
    K1 = 0.6072529350088812561694

    def __init__(self, bits):
        self._bits = bits
        self._q = bits - 1
        self._mul = (1 << self._q)
        self._one_div_k = floor(self._mul * self.K1 + 0.5)
        self._half_pi = floor(self._mul * (pi/2) + 0.5)
        self._tab = np.round((np.arctan(np.power(2.0, np.negative(
            np.arange(start=0, stop=bits))))) * self._mul, 0).astype(np.int32)

    def quantize(self, val):
        return floor(self._mul * val + 0.5)

    def dequantize(self, val):
        return val / self._mul

    def cordic(self, theta, n=None):
        if n is None:
            n = self._bits
        theta = np.array(theta, dtype=np.int32).reshape((-1,))
        cordic_cos = np.full([len(theta)], self._one_div_k, dtype=np.int32)
        cordic_sin = np.zeros([len(theta)], dtype=np.int32)
        n = len(self._tab) if n > len(self._tab) else n
        for k in range(n):
            delta = np.where(theta >= 0, 0, -1).astype(np.int32)
            cordic_cos, cordic_sin = (cordic_cos - (((cordic_sin >> k) ^ delta) - delta),
                                      cordic_sin + (((cordic_cos >> k) ^ delta) - delta))
            theta = theta - ((self._tab[k] ^ delta) - delta)
        return cordic_cos, cordic_sin


x = CordicFp(16)
for i in range(50):
    p = (i/50.0)*pi/2
    c, s = x.cordic(x.quantize(p))
    print("%s : %s" % (cos(p), x.dequantize(c)))
x = 1
