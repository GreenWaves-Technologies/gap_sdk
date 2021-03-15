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

from graph.types.others import GatherParameters, NoOPParameters, QuantizeParameters
from typing import cast as typing_cast

import numpy as np
from skimage.transform import resize
from graph.types import (CastParameters, ConcatParameters,
                         ConstantInputParameters, CopyParameters,
                         InputParameters, OutputParameters, ReshapeParameters,
                         ReverseParameters, SplitParameters,
                         StridedSliceParameters, TransposeParameters)
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              quantization)
from quantization.quantization_record_base import QuantizationRecordBase


@params_type(InputParameters)
@quantization('float32')
class InputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        if qrec is None:
            qrec = Float32QuantizationRecord()
        in_tensor = in_tensors[params.index]
        if in_tensor.size == params.dims.size():
            if len(in_tensor.shape) == len(params.dims.shape):
                in_shape = tuple(dim for dim in in_tensor.shape if dim > 1)
                expected_shape = tuple(dim for dim in params.dims.shape if dim > 1)
                if in_shape != expected_shape:
                    raise ValueError(f'{params.name} received input of shape {in_tensor.shape} but expecting {params.dims.shape}')
            in_tensor = in_tensor.reshape(params.dims.shape)
        else:
            in_tensor = resize(in_tensor, params.dims.shape)
        if params.transpose_out:
            in_tensor = np.transpose(in_tensor, params.transpose_out)
        return qrec.get_outputs(params, [in_tensor], ktype="float32")


@params_type(OutputParameters)
@quantization('float32')
class OutputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensor = in_tensors[0]
        if params.transpose_in:
            in_tensor = np.transpose(in_tensor, params.transpose_in[0])
        return [in_tensor]


@params_type(ConstantInputParameters)
@quantization('float32')
class ConstantInputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        del in_tensors
        if qrec is None:
            qrec = Float32QuantizationRecord()

        # if value_quantization is set then dequantize
        # if mutated then make a copy otherwise numpy may modify it

        if params.qtype is None:
            value = params.value if not params.is_mutated else params.value.copy()
        else:
            value = params.dqvalue
        return qrec.get_outputs(params, [value], ktype="float32")

@params_type(QuantizeParameters)
@quantization('any')
class QuantizeAny(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        # qname = kwargs['qname']
        # in_tensors = qrec.prepare_inputs(params, in_tensors, ktype=qname)
        # return qrec.get_outputs(params, in_tensors, ktype=qname)
        return in_tensors

@params_type(ConcatParameters)
@quantization('any')
class ConcatFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype=qname)
        if params.transpose_in:
            in_tensors = [(np.transpose(in_tensor, params.transpose_in[idx]) if params.transpose_in[idx] else in_tensor)
                          for idx, in_tensor in enumerate(in_tensors)]
        out_tensor = np.concatenate(in_tensors, params.axis)
        if params.transpose_out:
            out_tensor = np.transpose(out_tensor, params.transpose_out[0])
        return qrec.get_outputs(params, [out_tensor], ktype=qname)


@params_type(ReshapeParameters)
@quantization('any')
class ReshapeFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        if params.transpose_in:
            in_tensor = np.transpose(in_tensor, params.transpose_in[0])
        in_tensor = np.reshape(in_tensor, params.shape)
        if params.transpose_out:
            in_tensor = np.transpose(in_tensor, params.transpose_out[0])
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(TransposeParameters)
@quantization('any')
class TransposeFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        if params.transpose_in:
            in_tensor = np.transpose(in_tensor, params.transpose_in[0])
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(StridedSliceParameters)
@quantization('any')
class StridedSliceFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        params = typing_cast(StridedSliceParameters, params)
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensors = [params.numpy_slice(in_tensor)]
        return qrec.get_outputs(params, out_tensors, ktype=qname)


@params_type(CastParameters)
@quantization('any')
class CastFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensors = [in_tensor]
        return qrec.get_outputs(params, out_tensors, ktype=qname)


@params_type(SplitParameters)
@quantization('any')
class SplitFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        params = typing_cast(SplitParameters, params)
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        if params.transpose_in:
            in_tensor = np.transpose(in_tensor, params.transpose_in[0])
        out_tensors = params.numpy_split(in_tensor)
        if params.transpose_out:
            out_tensors = [(np.transpose(out_tensor, params.transpose_in[idx])
                            if params.transpose_in[idx] else out_tensor)
                           for idx, out_tensor in enumerate(out_tensors)]
        return qrec.get_outputs(params, out_tensors, ktype=qname)

@params_type(GatherParameters)
@quantization('any')
class GatherAny(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensor = np.take(in_tensor, params.indices, axis=params.axis)
        return qrec.get_outputs(params, [out_tensor], ktype=qname)

@params_type(CopyParameters)
@quantization('any')
class CopyFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(ReverseParameters)
@quantization('any')
class ReverseFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(params, [np.flip(in_tensor, axis=params.axis)], ktype=qname)

@params_type(NoOPParameters)
@quantization('any')
class NoOPFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        qname = kwargs['qname']
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype=qname)
        return qrec.get_outputs(params, in_tensors, ktype=qname)
