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
from nntool.execution.kernels.kernel_base import (KernelBase, params_type,
                                                  qrec_type)
from nntool.graph.types import (BatchToSpaceNode, ConcatNode,
                                ConstantInputNode, CopyNode, ExpandNode,
                                GatherNode, InputNode, OutputNode, RepeatNode,
                                ReshapeNode, ReverseNode, SpaceToBatchNode,
                                SplitNode, StridedSliceNode, TransposeNode)
from nntool.graph.types.misc import NoOPNode, QuantizeNode
from nntool.quantization.new_qrec import AllFloatQRec, QRec
from nntool.utils.at_norm import at_norm
from nntool.utils.batch_to_space import batch_to_space_nd, space_to_batch_nd


@params_type(InputNode)
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
            raise ValueError(
                f'{params.name} received input of shape {in_tensor.shape} but expecting {params.dims.shape}')
        out_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else (params.imported_dtype if params.imported_dtype else np.float32)
        in_tensor = in_tensor.astype(out_dtype)
        return qrec.get_outputs(params, [in_tensor], ktype="float")


@params_type(OutputNode)
@qrec_type('float')
class OutputFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = in_tensors[0]
        return [in_tensor]


@params_type(ConstantInputNode)
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

@params_type(QuantizeNode)
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
            in_q = qrec.in_qs[0]
            out_q = qrec.out_qs[0]
            float_conversion = in_q.is_floating or out_q.is_floating
            bit_conversion = in_q.bits != out_q.bits
            if not float_conversion:
                same_sign = in_q.signed == out_q.signed
                if in_q.bits > out_q.bits:
                    bit_diff = in_q.bits - out_q.bits
                    same_scale = np.allclose(in_q.scale * np.power(2, bit_diff), out_q.scale, atol=0.0001)
                    same_zeropoint = np.all(in_q.zero_point >> bit_diff == out_q.zero_point)
                elif out_q.bits > in_q.bits:
                    bit_diff = out_q.bits - in_q.bits
                    same_scale = np.allclose(out_q.scale * np.power(2, bit_diff), in_q.scale, atol=0.0001)
                    same_zeropoint = np.all(in_q.zero_point == out_q.zero_point >> bit_diff)
                else:
                    same_scale = np.allclose(out_q.scale, in_q.scale, atol=0.0001)
                    same_zeropoint = np.all(in_q.zero_point == out_q.zero_point)

                if same_scale and same_sign and bit_conversion and same_zeropoint:
                    if in_q.bits > out_q.bits:
                        if in_q.signed:
                            out_tensor = out_q.clip(at_norm(in_tensors[0].astype(np.int32), in_q.bits - out_q.bits))
                        else:
                            out_tensor = out_q.clip(at_norm(in_tensors[0].astype(np.uint32), in_q.bits - out_q.bits))
                    else:
                        out_tensor = in_tensors[0].astype(out_q.dtype) << (out_q.bits - in_q.bits)
                    return qrec.get_outputs(params, [out_tensor], ktype=qname)
            # in all other conversions should be numerically equivalent to this (within 1 bit)
            out_tensor = qrec.out_qs[0].quantize_from(in_tensors[0], qrec.in_qs[0])
        else:
            out_tensor = in_tensors[0]
        
        return qrec.get_outputs(params, [out_tensor], ktype=qname)

@params_type(ConcatNode)
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

@params_type(RepeatNode)
@qrec_type('any')
class Repeat(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        in_tensor = np.repeat(in_tensor, params.n_repeats, axis=params.repeat_axis)
        return qrec.get_outputs(params, [in_tensor], ktype=qname)

@params_type(ReshapeNode)
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


@params_type(TransposeNode)
@qrec_type('any')
class TransposeFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        if params.transpose:
            in_tensor = np.transpose(in_tensor, params.transpose) 
        return qrec.get_outputs(params, [in_tensor], ktype=qname)


@params_type(StridedSliceNode)
@qrec_type('any')
class StridedSliceFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        params = typing_cast(StridedSliceNode, params)
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensors = [params.numpy_slice(in_tensor)]
        return qrec.get_outputs(params, out_tensors, ktype=qname)


@params_type(SplitNode)
@qrec_type('any')
class SplitFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        params = typing_cast(SplitNode, params)
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        out_tensors = params.numpy_split(in_tensor)
        return qrec.get_outputs(params, out_tensors, ktype=qname)

@params_type(GatherNode)
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

@params_type(CopyNode)
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


@params_type(ReverseNode)
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

@params_type(ExpandNode)
@qrec_type('any')
class ExpandFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(params, [in_tensor * np.ones(params.shape, dtype=in_tensor.dtype)], ktype=qname)

@params_type(NoOPNode)
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

@params_type(BatchToSpaceNode)
@qrec_type('any')
class BatchToSpaceAny(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        # TODO - Cope with non nhwc order
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(
            params,
            [batch_to_space_nd(in_tensor, params.block_shape, params.crops)],
            ktype=qname)

@params_type(SpaceToBatchNode)
@qrec_type('any')
class SpaceToBatchAny(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        qname = kwargs['qname']
        # TODO - Cope with non nhwc order
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype=qname)[0]
        return qrec.get_outputs(
            params,
            [space_to_batch_nd(in_tensor, params.block_shape, params.pads)],
            ktype=qname)
