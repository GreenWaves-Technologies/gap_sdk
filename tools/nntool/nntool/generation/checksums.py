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

import numpy as np

CHECKSUM_FUNCS = {
    8: "verify_crc_int8",
    16: "verify_crc_int16",
    32: "verify_crc_int32",
}

DTYPES = {
    8: np.uint8,
    16: np.uint16,
    32: np.uint32,
}

def checksum_func(G, name):
    node = G[name]
    return CHECKSUM_FUNCS[node.out_q.bits]

def calc_value_checksum(G, name):
    node = G[name]
    return np.sum(node.value, dtype=DTYPES[node.out_q.bits])
