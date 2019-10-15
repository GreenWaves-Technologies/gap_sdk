# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
