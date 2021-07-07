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

# square root coefficients in 17.15 fixed point format

import numpy as np

# Q16 (uint16) LUT for sigmoid function
sigmoid_table = np.array(
    [32768, 33451, 34133, 34813, 35493, 36169, 36843, 37513, 38180, 38841, 39498,
     40149, 40794, 41432, 42064, 42688, 43304, 43912, 44511, 45102, 45683, 46255,
     46817, 47369, 47911, 48443, 48964, 49475, 49975, 50464, 50942, 51409, 51865,
     52311, 52745, 53169, 53581, 53983, 54374, 54755, 55125, 55485, 55834, 56174,
     56503, 56823, 57133, 57433, 57724, 58007, 58280, 58544, 58800, 59048, 59288,
     59519, 59743, 59959, 60168, 60370, 60565, 60753, 60935, 61110, 61279, 61441,
     61599, 61750, 61896, 62036, 62172, 62302, 62428, 62549, 62666, 62778, 62886,
     62990, 63090, 63186, 63279, 63368, 63454, 63536, 63615, 63691, 63765, 63835,
     63903, 63968, 64030, 64090, 64148, 64204, 64257, 64308, 64357, 64405, 64450,
     64494, 64536, 64576, 64614, 64652, 64687, 64721, 64754, 64786, 64816, 64845,
     64873, 64900, 64926, 64950, 64974, 64997, 65019, 65039, 65060, 65079, 65097,
     65115, 65132, 65149, 65164, 65179, 65194, 65208, 65221, 65234, 65246, 65258,
     65269, 65280, 65291, 65301, 65310, 65319, 65328, 65337, 65345, 65352, 65360,
     65367, 65374, 65381, 65387, 65393, 65399, 65404, 65410, 65415, 65420, 65425,
     65429, 65433, 65438, 65442, 65445, 65449, 65453, 65456, 65459, 65462, 65465,
     65468, 65471, 65474, 65476, 65479, 65481, 65483, 65485, 65488, 65489, 65491,
     65493, 65495, 65497, 65498, 65500, 65501, 65503, 65504, 65505, 65507, 65508,
     65509, 65510, 65511, 65512, 65513, 65514, 65515, 65516, 65517, 65517, 65518,
     65519, 65520, 65520, 65521, 65522, 65522, 65523, 65523, 65524, 65524, 65525,
     65525, 65526, 65526, 65526, 65527, 65527, 65528, 65528, 65528, 65529, 65529,
     65529, 65529, 65530, 65530, 65530, 65530, 65531, 65531, 65531, 65531, 65531,
     65532, 65532, 65532, 65532, 65532, 65532, 65533, 65533, 65533, 65533, 65533,
     65533, 65533, 65533, 65534, 65534, 65534, 65534, 65534, 65534, 65534, 65534,
     65534, 65534, 65535], dtype=np.int32)

NEAREST = True


def sigmoid_lut(x, qtype=None):
    """ Lookup table based sigmoid.
        Input is in Q12
        Output is in Q15"""
    del qtype
    # Scale [-8:8] into [-10.7:10.7] --> *3/4
    result = np.empty_like(x)
    if not NEAREST:
        abs_x = (np.abs(x) * 3) >> 9  # input in Q12
        abs_x_masked = np.where(abs_x >= 255, 0, abs_x)
        ua = sigmoid_table[abs_x_masked]
        ub = sigmoid_table[abs_x_masked+1]
        ut = abs_x & 0xFF
        result = np.where(abs_x >= 255,
                          0x7FFF << 10,
                          (ua << 9) + ut * (ub-ua))  # Q16*Q8 = Q24
        result = np.where(x > 0, result + (1 << 9),
                          (1 << (9+16))-result+(1 << 9)-1)
        return result >> 10
    else:
        abs_x = (np.abs(x) * 3) >> 9  # input in Q12
        abs_x_masked = np.where(abs_x >= 255, 0, abs_x)
        result = np.where(abs_x >= 255,
                          0xFFFF,
                          sigmoid_table[abs_x_masked])  # Q16*Q8 = Q24
        result = np.where(x > 0, result, (1 << 16)-result)
        return result >> 1


def tanh_lut(x, qtype=None):
    """ Lookup table based tanh.
        Input is in Q12
        Output is in Q15"""
    del qtype
    # Scale [-8:8] into [-10.7:10.7] --> *3/4
    result = np.empty_like(x)
    if not NEAREST:
        abs_x = (np.abs(x) * 3) >> 8  # 2*abs_x
        abs_x_masked = np.where(abs_x >= 255, 0, abs_x)
        ua = sigmoid_table[abs_x_masked]
        ub = sigmoid_table[abs_x_masked+1]
        ut = abs_x & 0xFF
        result = np.where(abs_x >= 255,
                          0xFFFF << 8,
                          (ua << 8) + ut * (ub-ua))  # Q16*Q8 = Q24
        result = np.where(x > 0,
                          result - (1 << (9+14)) + (1 << (9-2)),
                          -result + (1 << (9+14)) + (1 << (9-2)) - 1)
        return result >> 8
    else:
        abs_x = (np.abs(x) * 3) >> 8  # 2*abs_x
        abs_x_masked = np.where(abs_x >= 255, 0, abs_x)
        result = np.where(abs_x >= 255,
                          0xFFFF,
                          sigmoid_table[abs_x_masked])
        result = np.where(x > 0, result - (1 << 15), -result + (1 << 15))
        return result
