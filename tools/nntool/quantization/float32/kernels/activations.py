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

from quantization.quantization_record_base import QuantizationRecordBase
from quantization.float32.float32_quantization import Float32QuantizationRecord
from graph.types import ReluActivationParameters


def hswish(params,
           in_tensors,
           qrec: QuantizationRecordBase,
           details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    return qrec.get_outputs(params, [in_tensor * np.minimum(np.maximum(in_tensor + 3, 0), 6) / 6], ktype="float32")


def hsigmoid(params,
             in_tensors,
             qrec: QuantizationRecordBase,
             details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    return qrec.get_outputs(params, [np.minimum(np.maximum(in_tensor + params.offset, 0), 6) / 6], ktype="float32")


def sigmoid(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    return qrec.get_outputs(params, [1/(1 + np.exp(-in_tensor))], ktype="float32")


def relu(params: ReluActivationParameters,
         in_tensors,
         qrec: QuantizationRecordBase,
         details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    if params.upper_bound == None:
        return qrec.get_outputs(params,
                                [np.maximum(in_tensor,
                                            params.lower_bound)],
                                ktype="float32")
    return qrec.get_outputs(params,
                            [np.minimum(np.maximum(in_tensor,
                                                   params.lower_bound),
                                        params.upper_bound)],
                            ktype="float32")


def leaky(params,
          in_tensors,
          qrec: QuantizationRecordBase,
          details=None):
    raise NotImplementedError()
