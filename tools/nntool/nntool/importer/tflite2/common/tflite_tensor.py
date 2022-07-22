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

from nntool.quantization.qtype import QType
import numpy as np

from ..tflite_schema_head import TensorType


def expand_dims_to(value, other_value):
    if len(value) <= 1:
        return value
    return np.reshape(value, list(value.shape) + [1] * (other_value.ndim - value.ndim))


class TensorBase():
    def __init__(self, name):
        self._name = name
        self._used = False
        self._inputs = []
        self._output = None
        self._constant_node = None
        self._dont_link = False
        self._input_node = None
        self._output_node = None

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, val):
        self._name = val

    @property
    def input_node(self):
        return self._input_node

    @input_node.setter
    def input_node(self, val):
        self._input_node = val

    @property
    def output_node(self):
        return self._output_node

    @output_node.setter
    def output_node(self, val):
        self._output_node = val

    @property
    def used(self):
        return self._used

    @used.setter
    def used(self, val):
        self._used = val

    @property
    def inputs(self):
        return self._inputs

    @property
    def output(self):
        return self._output

    @output.setter
    def output(self, val):
        self._output = val

    @property
    def constant_node(self):
        return self._constant_node

    @constant_node.setter
    def constant_node(self, val):
        self._constant_node = val

    @property
    def dont_link(self):
        return self._dont_link

    @dont_link.setter
    def dont_link(self, val):
        self._dont_link = val

NP_TYPES = {
    'FLOAT16': np.float16,
    'FLOAT32': np.float32,
    'FLOAT64': np.float64,
    'INT16': np.int16,
    'INT32': np.int32,
    'INT64': np.int64,
    'INT8': np.int8,
    'UINT8': np.uint8,
    'COMPLEX64': np.complex64 
}

class TFLiteTensorWrapper(TensorBase):
    TF_TO_NUMPY_TYPE = {
        getattr(TensorType.TensorType, key): NP_TYPES[key] for key in dir(TensorType.TensorType) if key in NP_TYPES
    }

    def __init__(self, tensor, model):
        super(TFLiteTensorWrapper, self).__init__(tensor.Name().decode('ascii'))
        self._tensor = tensor
        self._model = model

    @property
    def buffer_idx(self):
        return self._tensor.Buffer()

    @property
    def tf_tensor(self):
        return self._tensor

    @property
    def is_variable(self):
        return self._tensor.IsVariable()

    @property
    def shape(self):
        val = self._tensor.ShapeAsNumpy()
        if isinstance(val, int) and val == 0:
            return tuple()
        shape = tuple(val)
        return shape

    @property
    def dtype(self):
        return self.TF_TO_NUMPY_TYPE.get(self._tensor.Type(), None)

    @property
    def scale(self):
        return self._tensor.Quantization().ScaleAsNumpy()

    @property
    def zero_point(self):
        return self._tensor.Quantization().ZeroPointAsNumpy()

    @property
    def min_val(self):
        return self._tensor.Quantization().MinAsNumpy()

    @property
    def max_val(self):
        return self._tensor.Quantization().MaxAsNumpy()

    @property
    def is_uint_symmetric(self):
        quant = self._tensor.Quantization()
        if quant is not None:
            return (self.dtype == np.uint8 or self.dtype == np.uint16 or self.dtype == np.uint32) and \
                np.all(quant.ZeroPointAsNumpy() == 128)
        return False

    @staticmethod
    def asymmetric_mult_qtype_from_tflite(tf_qps, dtype):
        zero_point = tf_qps.ZeroPointAsNumpy() if tf_qps.ZeroPointLength() > 0 else None
        scale = tf_qps.ScaleAsNumpy() if tf_qps.ScaleLength() > 0 else None
        if tf_qps.MinLength() == 0 or tf_qps.MaxLength() == 0:
            # Some tflite graphs seem to have records with scale and zero point but no min and max
            # values. This causes problems to determine symmetric scalling
            # This recreates min and max from the scale and zeropoint values
            assert zero_point is not None, "can't compute min/max statistics - zero point is not present"
            assert scale is not None, "can't compute min/max statistics - scale is not present"
            iinfo = np.iinfo(dtype)
            min_val = (iinfo.min - zero_point) * scale
            max_val = (iinfo.max - zero_point) * scale
        else:
            min_val = tf_qps.MinAsNumpy()
            max_val = tf_qps.MaxAsNumpy()
        quantized_dimension = tf_qps.QuantizedDimension()
        return QType(min_val=min_val, max_val=max_val, scale=scale, zero_point=zero_point,
                     quantized_dimension=quantized_dimension, dtype=dtype)

    @staticmethod
    def symmetric_mult_qtype_from_tflite(tf_qps, dtype):
        min_val = tf_qps.MinAsNumpy() if tf_qps.MinLength() > 0 else None
        max_val = tf_qps.MaxAsNumpy() if tf_qps.MaxLength() > 0 else None
        if min_val is not None and max_val is not None \
                and np.all(np.abs(min_val) == max_val):
            narrow_range = True
        else:
            narrow_range = False
        scale = tf_qps.ScaleAsNumpy() if tf_qps.ScaleLength() > 0 else None
        quantized_dimension = tf_qps.QuantizedDimension()
        return QType(min_val=min_val, max_val=max_val, dtype=dtype,
                     scale=scale, narrow_range=narrow_range,
                     quantized_dimension=quantized_dimension)

    @property
    def qtype(self):
        if self.dtype is None:
            return None
        if issubclass(self.dtype, np.floating):
            if self.is_constant:
                min_val = np.atleast_1d(self.value.min())
                max_val = np.atleast_1d(self.value.max())
            else:
                min_val = max_val = None
            return QType(dtype=self.dtype, min_val=min_val, max_val=max_val)
        quant = self._tensor.Quantization()
        if quant is not None:
            if quant.ScaleLength() == 0 and quant.MinLength() == 0 and\
                    quant.MaxLength() == 0 and quant.ZeroPointLength() == 0:
                return None
            if self.dtype == np.uint8 or self.dtype == np.uint16 or self.dtype == np.uint32 or self.dtype == np.uint64:
                return self.asymmetric_mult_qtype_from_tflite(quant, self.dtype)
            elif self.dtype == np.int8 or self.dtype == np.int16 or self.dtype == np.int32 or self.dtype == np.int64:
                if np.all(quant.ZeroPointAsNumpy() == 0):
                    return self.symmetric_mult_qtype_from_tflite(quant, self.dtype)
                return self.asymmetric_mult_qtype_from_tflite(quant, self.dtype)
            return None
        return None

    @property
    def is_constant(self):
        return self.buffer_idx != 0

    @property
    def dqvalue(self):
        if self.qtype:
            return self.qtype.dequantize(self.value)
        return self.value

    @property
    def value(self):
        if self.buffer_idx == 0 or self._model.Buffers(self.buffer_idx).DataLength() == 0:
            if self.dtype is None:
                return None
            return np.zeros(self.shape, dtype=self.dtype().newbyteorder('L'))
        tf_buffer = self._model.Buffers(self.buffer_idx)
        np_buffer = np.frombuffer(tf_buffer.DataAsNumpy(), dtype=self.dtype().newbyteorder('L'))
        if not (isinstance(self.shape, int) or len(self.shape) == 0):
            np_buffer = np.resize(np_buffer, self.shape)
        else:
            np_buffer = np.reshape(np_buffer, self.shape)
        return np_buffer

    def shape_as(self, order):
        assert len(order) == len(self.shape), "tensor does not have correct number of dimensions"
        return {k: v for k, v in zip(order, self.shape)}
