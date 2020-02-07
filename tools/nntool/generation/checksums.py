# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
