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

# table QuantizationParameters {
#   // These four parameters are the asymmetric linear quantization parameters.
#   // Given a quantized value q, the corresponding float value f should be:
#   //   f = scale * (q - zero_point)
#   // For other quantization types, the QuantizationDetails below is used.
#   min:[float];  // For importing back into tensorflow.
#   max:[float];  // For importing back into tensorflow.
#   scale:[float];  // For dequantizing the tensor's values.
#   zero_point:[long];

#   // If this is not none, the other quantization parameters (i.e. min, max,
#   // scale, zero_point fields above) are ignored and the value of the
#   // QuantizationDetails union should be used.
#   details:QuantizationDetails;

#   // Specifies the dimension of the Tensor's shape that the scales and
#   // zero_points correspond to. For example, a tensor t, with dims=[4, 3, 2, 1]
#   // with quantization params:
#   //   scale=[1.0, 2.0, 3.0], zero_point=[1, 2, 3], quantization_dimension=1
#   // will be quantized across the second dimension of t.
#   //   t[:, 0, :, :] will have scale[0]=1.0, zero_point[0]=1
#   //   t[:, 1, :, :] will have scale[1]=2.0, zero_point[0]=2
#   //   t[:, 2, :, :] will have scale[2]=3.0, zero_point[0]=3
#   quantized_dimension:int;
# }

import numpy as np
from utils.json_serializable import JsonSerializable
from quantization.qtype_base import QTypeBase

def range_to_str(val):
    if val == 0:
        return "-no quant-"
    
    if len(val) == 1:
        return "{}".format(val[0])
    elif len(val) == 2:
        return "[{},{}]".format(val[0], val[1])
    else:
        return "[{},..,{}]".format(val[0], val[-1])

class TfliteQType(QTypeBase, JsonSerializable):
    def __init__(self, quantization_parameters, dtype, init=None):
        if init:
            self._min = np.array(init['min'])
            self._max = np.array(init['max'])
            self._scale = np.array(init['scale'])
            self._zero_point = np.array(init['zero'])
            self._quantized_dimension = init['dim']
            self._dtype = getattr(np, init['dtype'])
            return
        self._min = quantization_parameters.MinAsNumpy()
        self._max = quantization_parameters.MaxAsNumpy()
        self._scale = quantization_parameters.ScaleAsNumpy()
        self._zero_point = quantization_parameters.ZeroPointAsNumpy()
        self._quantized_dimension = quantization_parameters.QuantizedDimension()
        self._dtype = dtype

    def _encapsulate(self):
        return {
            "min": self._min.tolist(),
            "max": self._max.tolist(),
            "scale": self._scale.tolist(),
            "zero": self._zero_point.tolist(),
            "dim": self._quantized_dimension,
            "dtype": self._dtype.__name__
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(None, None, init=val)

    @classmethod
    def report_columns(cls):
        return [
            "Min",
            "Max",
            "Scale",
            "Zero",
            "Q Dim",
            "Type"
        ]

    def quantize(self, arr: np.array) -> np.array:
        raise NotImplementedError()

    def dequantize(self, arr: np.array) -> np.array:
        raise NotImplementedError()

    def expand_from(self, arr: np.array, from_qtype: QTypeBase) -> np.array:
        raise NotImplementedError()

    def reduce_from(self, arr: np.array, from_qtype: QTypeBase) -> np.array:
        raise NotImplementedError()

    def to_report(self):
        return [
            "{}".format(range_to_str(self._min)),
            "{}".format(range_to_str(self._max)),
            "{}".format(range_to_str(self._scale)),
            "{}".format(range_to_str(self._zero_point)),
            "{}".format(self._quantized_dimension),
            self._dtype
        ]