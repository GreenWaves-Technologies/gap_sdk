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
from graph.types import (ConcatParameters, ConstantInputParameters,
                         CopyParameters, InputParameters, OutputParameters,
                         ReshapeParameters, ReverseParameters, SplitParameters,
                         StridedSliceParameters, TransposeParameters)
from graph.types.others import (GatherParameters, NoOPParameters,
                                QuantizeParameters)
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import AllFloatQRec, QRec
from skimage.transform import resize


@params_type(InputParameters)
@qrec_type('float')
class InputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
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
        return qrec.get_outputs(params, [in_tensor], ktype="float")


@params_type(OutputParameters)
@qrec_type('float')
class OutputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = in_tensors[0]
        return [in_tensor]


@params_type(ConstantInputParameters)
@qrec_type('float')
class ConstantInputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        del in_tensors
        if qrec is None:
            qrec = AllFloatQRec()

        # if value_quantization is set then dequantize
        # if mutated then make a copy otherwise numpy may modify it

        if params.qtype is None:
            value = params.value if not params.is_mutated else params.value.copy()
        else:
            value = params.dqvalue
        value = qrec.out_qs[0].quantize(value)
        return qrec.get_outputs(params, [value], ktype="float")

@params_type(QuantizeParameters)
@qrec_type('any')
class QuantizeAny(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype=qname)
        if qrec:
            out_tensor = qrec.out_qs[0].quantize_from(in_tensors[0], qrec.in_qs[0])
        else:
            out_tensor = in_tensors[0]
        
        return qrec.get_outputs(params, [out_tensor], ktype=qname)

@params_type(ConcatParameters)
@qrec_type('any')
class ConcatFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype=qname)
        out_tensor = np.concatenate(in_tensors, params.axis)
        return qrec.get_outputs(params, [out_tensor], ktype=qname)


@params_type(ReshapeParameters)
@qrec_type('any')
class ReshapeFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        in_tensor = np.reshape(in_tensor, params.shape)
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(TransposeParameters)
@qrec_type('any')
class TransposeFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(StridedSliceParameters)
@qrec_type('any')
class StridedSliceFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        params = typing_cast(StridedSliceParameters, params)
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensors = [params.numpy_slice(in_tensor)]
        return qrec.get_outputs(params, out_tensors, ktype=qname)


@params_type(SplitParameters)
@qrec_type('any')
class SplitFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        params = typing_cast(SplitParameters, params)
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensors = params.numpy_split(in_tensor)
        return qrec.get_outputs(params, out_tensors, ktype=qname)

@params_type(GatherParameters)
@qrec_type('any')
class GatherAny(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensor = np.take(in_tensor, params.indices, axis=params.axis)
        return qrec.get_outputs(params, [out_tensor], ktype=qname)

@params_type(CopyParameters)
@qrec_type('any')
class CopyFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(ReverseParameters)
@qrec_type('any')
class ReverseFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(params, [np.flip(in_tensor, axis=params.axis)], ktype=qname)

@params_type(NoOPParameters)
@qrec_type('any')
class NoOPFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype=qname)
        return qrec.get_outputs(params, in_tensors, ktype=qname)
