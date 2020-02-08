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

from .tflite_schema_head.BuiltinOperator import BuiltinOperator

TFLITE_TYPE_NAMES = [
    "FLOAT32",
    "FLOAT16",
    "INT32",
    "UINT8",
    "INT64",
    "STRING",
    "BOOL",
    "INT16",
    "COMPLEX64",
    "INT8"
]

TFLITE_TYPE_SIZES = [
    32,
    16,
    32,
    8,
    64,
    -1,
    1,
    16,
    64,
    8
]

TFLITE_TYPE_DECODERS = [
    np.float32,
    np.float16,
    np.int32,
    np.uint8,
    np.int64,
    None,
    np.bool,
    np.int16,
    np.complex64,
    np.int8
]

TFLITE_BUILTIN_OPERATOR_NAMES = {
    getattr(BuiltinOperator, attr):attr\
        for attr in dir(BuiltinOperator) if not attr.startswith("__")
}

def calculate_size_bits(tensor):
    size = TFLITE_TYPE_SIZES[tensor.Type()]
    assert size > 0, "Strings not handled"
    return np.prod(tensor.ShapeAsNumpy()) * size

def calculate_size_bytes(tensor):
    return calculate_size_bits(tensor) // 8

def tensor_type_name(tensor):
    return TFLITE_TYPE_NAMES[tensor.Type()]

def get_vals(model, tensor, offset=0, count=-1):
    buffer = model.Buffers(tensor.Buffer())
    dtype = np.dtype(TFLITE_TYPE_DECODERS[tensor.Type()]).newbyteorder('>')
    t_len = TFLITE_TYPE_SIZES[tensor.Type()]//8
    offset = offset * t_len
    if count == -1:
        count = (buffer.DataLength() - (offset * t_len)) // t_len
    t_count = count * t_len
    return np.frombuffer(bytearray([buffer.Data(i + offset) for i in range(t_count)]),\
        dtype=dtype, count=count, offset=0)

def get_stats(model, tensor):
    vals = get_vals(model, tensor)
    return {"min": np.amin(vals), "max": np.amax(vals), "std": np.std(vals)}

def get_operator_index(model, op_name):
    op_index = getattr(BuiltinOperator, op_name,\
        BuiltinOperator.CUSTOM)
    for j in range(model.OperatorCodesLength()):
        op_code = model.OperatorCodes(j)
        if op_code.BuiltinCode() == op_index and\
            (op_index != BuiltinOperator.CUSTOM or\
                op_code.CustomCode() == op_name):
            return j
    raise IndexError()

def get_operator_name(model, index):
    op_code = model.OperatorCodes(index)
    op_name = TFLITE_BUILTIN_OPERATOR_NAMES[op_code.BuiltinCode()]
    if op_name == "CUSTOM":
        return str(op_code.CustomCode(), 'utf-8'), True

    return op_name, False
