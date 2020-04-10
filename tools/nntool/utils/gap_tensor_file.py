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

import logging
import os
import struct
from io import BufferedWriter, BufferedReader

import numpy as np

LOG = logging.getLogger("nntool." + __name__)

TYPES = {
    8: np.int8,
    16: np.int16,
    32: np.int32
}

CTYPES = {
    np.int8: "signed char",
    np.int16: "short int",
    np.int32: "int"
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
    def read(cls, fp: BufferedReader):
        buf = fp.read(struct.calcsize(cls.__FMT))
        if buf:
            return cls(*struct.unpack(cls.__FMT, buf))
        return None

    def write(self, fp: BufferedWriter):
        assert len(self.dims) <= 4, "up to 4 dimensions are supported"
        dims = self.dims.copy() + [0] * (4 - len(self.dims))
        buf = struct.pack(self.__FMT, self.step, self.tensor_type, self.size, self.elem_size, len(self.dims), *dims)
        fp.write(buf)

def write_dims(prefix, fp, dims, elem_size):
    assert len(dims) == 3
    dim_names = ["CHANNELS", "HEIGHT", "WIDTH"]
    for i, v in enumerate(dims):
        fp.write('{}_{}?={}\n'.format(prefix, dim_names[i], v))
    fp.write('{}_BITS?={}\n'.format(prefix, elem_size))
    fp.write('{}_TYPE?={}\n'.format(prefix, CTYPES[TYPES[elem_size]]))

def write_make_include_file(filename, header, output_shape, output_elem_size):
    with open(filename, 'w') as fp:
        write_dims("INPUT", fp, header.dims, header.elem_size)
        write_dims("OUTPUT", fp, output_shape, output_elem_size)        

def get_bits(tensor):
    bit_size = -1
    for k, v in TYPES.items():
        if tensor.dtype == v:
            bit_size = k
            break
    if bit_size < 0:
        raise ValueError("only int8, int16 and int32 are supported")
    return bit_size

def write_gap_tensor(filename, tensor, step=0, output_tensor=None, make_file=None):
    bit_size = get_bits(tensor)
    with open(filename, 'wb') as fp:
        header = Header(step, 0, tensor.size * bit_size // 8, bit_size, len(tensor.shape), *tensor.shape)
        header.write(fp)
        tensor.tofile(fp)
    if output_tensor is not None and make_file is not None:
        output_bit_size = get_bits(output_tensor)
        write_make_include_file(make_file, header, output_tensor.shape, output_bit_size)

def read_gap_tensors(filename):
    tensors = {}
    size = os.path.getsize(filename)
    max_step = -1
    with open(filename, 'rb') as fp:
        while fp.tell() < size:
            header = Header.read(fp)
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
