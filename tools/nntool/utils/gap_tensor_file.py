# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
import os
import struct

import numpy as np

LOG = logging.getLogger("nntool." + __name__)

TYPES = {
    8: np.int8,
    16: np.int16,
    32: np.int32
}


class Header():
    __FMT = '5i4i'

    def __init__(self, step, tensor_type, size, elem_size, num_dims, *dims):
        self.step = step
        self.tensor_type = tensor_type
        self.size = size
        self.elem_size = elem_size
        self.dims = [dims[i] for i in range(num_dims)]

    @classmethod
    def from_file(cls, fp):
        buf = fp.read(struct.calcsize(cls.__FMT))
        if buf:
            return cls(*struct.unpack(cls.__FMT, buf))
        return None

def read_gap_tensors(filename):
    tensors = {}
    size = os.path.getsize(filename)
    max_step = -1
    with open(filename, 'rb') as fp:
        while fp.tell() < size:
            header = Header.from_file(fp)
            if not header:
                break
            if header.elem_size not in TYPES:
                LOG.error("Bit width %s not found in types list. File may be corrupted.",
                          header.elem_size)
                raise ValueError()
            tensor = np.fromfile(fp, dtype=TYPES[header.elem_size], count=header.size)
            step_rec = tensors.get(header.step)
            if not step_rec:
                step_rec = [None]*3
                tensors[header.step] = step_rec
            elem = 0 if header.tensor_type < 2 else header.tensor_type - 1
            step_rec[elem] = tensor.reshape(header.dims)
            if header.step > max_step:
                max_step = header.step
    return [tensors.get(i) for i in range(max_step + 1)]
