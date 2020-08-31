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
from skimage.transform import resize

from quantization.quantization_record_base import QuantizationRecordBase
from quantization.quantized_ndarray import QuantizedNdarray


def graph_input(params,
                in_tensors,
                qrec: QuantizationRecordBase,
                details=None):
    del details
    # all graph inputs are passed all of the inputs of the graph
    # params.index indicates the index of the input that this node should output
    in_tensor = in_tensors[params.index]
    if in_tensor.size == params.dims.size():
        in_tensor = in_tensor.reshape(params.dims.shape)
    else:
        in_tensor = resize(in_tensor, params.dims.shape)
    if params.transpose_out:
        in_tensor = np.transpose(in_tensor, params.transpose_out)
    # output_tensors = qrec.get_outputs(params, [in_tensor], ktype="symmetric")
    return [qrec.out_qs[0].quantize(in_tensor)]


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
    # returns an array with a property that contains the quantization
    output_tensor = QuantizedNdarray(
        qrec.out_qs[0].get_quantized(
            params.value,
            container_is_quantized=qrec.constants_are_quantized),
        qtype=qrec.out_qs[0])
    return [output_tensor]


def concat(params,
           in_tensors,
           qrec: QuantizationRecordBase,
           details=None):
    del details
    in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
    assert all(qrec.in_qs[0] == qrec.in_qs[idx]
               for idx in range(1, len(qrec.in_qs))), "input is incorrectly quantized"
    if params.transpose_in:
        in_tensors = [(np.transpose(in_tensor, params.transpose_in[idx])
                       if params.transpose_in[idx] else in_tensor)
                      for idx, in_tensor in enumerate(in_tensors)]
    out_tensor = np.concatenate(in_tensors, params.axis)
    if params.transpose_out:
        out_tensor = np.transpose(out_tensor, params.transpose_out[0])
    return qrec.get_outputs(params, [out_tensor], ktype="symmetric")


def reshape(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    in_tensor = np.reshape(in_tensor, params.shape)
    if params.transpose_out:
        in_tensor = np.transpose(in_tensor, params.transpose_out[0])
    return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


def transpose(params,
              in_tensors,
              qrec: QuantizationRecordBase,
              details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


def strided_slice(params,
                  in_tensors,
                  qrec: QuantizationRecordBase,
                  details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    out_tensors = [params.numpy_slice(in_tensor)]
    return qrec.get_outputs(params, out_tensors, ktype="symmetric")


def cast(params,
         in_tensors,
         qrec: QuantizationRecordBase,
         details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    out_tensors = [in_tensor]
    return qrec.get_outputs(params, out_tensors, ktype="symmetric")


def split(params,
          in_tensors,
          qrec: QuantizationRecordBase,
          details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    if params.transpose_in:
        in_tensor = np.transpose(in_tensor, params.transpose_in[0])
    out_tensors = params.numpy_split(in_tensor)
    if params.transpose_out:
        out_tensors = [(np.transpose(out_tensor, params.transpose_in[idx])
                        if params.transpose_in[idx] else out_tensor)
                       for idx, out_tensor in enumerate(out_tensors)]
    return qrec.get_outputs(params, out_tensors, ktype="symmetric")


def copy(params,
         in_tensors,
         qrec: QuantizationRecordBase,
         details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    return qrec.get_outputs(params, [in_tensor], ktype="symmetric")
