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

from typing import cast as typing_cast

import numpy as np
from skimage.transform import resize

from graph.types import StridedSliceParameters, SplitParameters
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.quantization_record_base import QuantizationRecordBase


def graph_input(params,
                in_tensors,
                qrec: QuantizationRecordBase,
                details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = in_tensors[params.index]
    if in_tensor.size == params.dims.size():
        in_tensor = in_tensor.reshape(params.dims.shape)
    else:
        in_tensor = resize(in_tensor, params.dims.shape)
    if params.transpose_out:
        in_tensor = np.transpose(in_tensor, params.transpose_out)
    return qrec.get_outputs(params, [in_tensor], ktype="float32")


def graph_output(params,
                 in_tensors,
                 qrec: QuantizationRecordBase,
                 details=None):
    del details, qrec
    in_tensor = in_tensors[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    return [in_tensor]


def constant_input(params,
                   in_tensors,
                   qrec: QuantizationRecordBase,
                   details=None):
    del in_tensors, details
    if qrec is None:
        qrec = Float32QuantizationRecord()

    # if value_quantization is set then dequantize
    # if mutated then make a copy otherwise numpy may modify it
    if params.value_quantization is None:
        value = params.value if not params.is_mutated else params.value.copy()
    else:
        value = params.value_quantization.get_dequantized(params.value)
    return qrec.get_outputs(params, [value], ktype="float32")


def concat(params,
           in_tensors,
           qrec: QuantizationRecordBase,
           details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
    if params.transpose_in:
        in_tensors = [(np.transpose(in_tensor, params.transpose_in[idx])
                       if params.transpose_in[idx] else in_tensor)
                      for idx, in_tensor in enumerate(in_tensors)]
    out_tensor = np.concatenate(in_tensors, params.axis)
    if params.transpose_out:
        out_tensor = np.transpose(out_tensor, params.transpose_out[0])
    return qrec.get_outputs(params, [out_tensor], ktype="float32")


def reshape(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    in_tensor = np.reshape(in_tensor, params.shape)
    if params.transpose_out:
        in_tensor = np.transpose(in_tensor, params.transpose_out[0])
    return qrec.get_outputs(params, [in_tensor], ktype="float32")


def transpose(params,
              in_tensors,
              qrec: QuantizationRecordBase,
              details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    return qrec.get_outputs(params, [in_tensor], ktype="float32")


def strided_slice(params,
                  in_tensors,
                  qrec: QuantizationRecordBase,
                  details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    params = typing_cast(StridedSliceParameters, params)
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    out_tensors = [params.numpy_slice(in_tensor)]
    return qrec.get_outputs(params, out_tensors, ktype="float32")


def cast(params,
         in_tensors,
         qrec: QuantizationRecordBase,
         details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    out_tensors = [in_tensor]
    return qrec.get_outputs(params, out_tensors, ktype="float32")


def split(params,
          in_tensors,
          qrec: QuantizationRecordBase,
          details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    params = typing_cast(SplitParameters, params)
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    out_tensors = params.numpy_split(in_tensor)
    if params.transpose_out:
        out_tensors = [(np.transpose(out_tensor, params.transpose_in[idx])
                        if params.transpose_in[idx] else out_tensor)
                       for idx, out_tensor in enumerate(out_tensors)]
    return qrec.get_outputs(params, out_tensors, ktype="float32")


def copy(params,
         in_tensors,
         qrec: QuantizationRecordBase,
         details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    return qrec.get_outputs(params, [in_tensor], ktype="float32")
