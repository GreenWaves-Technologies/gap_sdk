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
from nntool.graph.types import (HSigmoidNode,
                         HSwishNode, LeakyNode,
                         ReluNode, SigmoidNode)
from nntool.graph.types.activations import (HTanHNode,
                                     TanHNode)
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import AllFloatQRec, QRec
from nntool.utils.fast_float import np_fastsigmoid, np_fasttanh
from nntool.utils.sigmoid_tanh_lut import sigmoid_lut_float, tanh_lut_float


@params_type(HSwishNode)
@qrec_type('float')
class HSwishFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        return qrec.get_outputs(params, [in_tensor * np.minimum(np.maximum(in_tensor + in_dtype(params.offset), in_dtype(0)), in_dtype(params.upper_bound)) * in_dtype(params.mult)], ktype="float")


@params_type(HSigmoidNode)
@qrec_type('float')
class HSigmoidFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        return qrec.get_outputs(params, [np.minimum(np.maximum(in_tensor + params.offset, in_dtype(0)), in_dtype(params.upper_bound)) * in_dtype(params.mult)], ktype="float")


@params_type(SigmoidNode)
@qrec_type('float')
class SigmoidFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        if in_dtype == np.float32:
            return qrec.get_outputs(params, [in_dtype(1) / (in_dtype(1) + np.exp(-in_tensor).astype(in_dtype))], ktype="float")
        else:
            if qrec.cache.get('kernel_type') == "lut":
                return qrec.get_outputs(params, [sigmoid_lut_float(in_tensor, in_dtype)], ktype="float")
            return qrec.get_outputs(params, [np_fastsigmoid(in_tensor, dtype=in_dtype )], ktype="float")


@params_type(TanHNode)
@qrec_type('float')
class TanHFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        if in_dtype == np.float32:
            return qrec.get_outputs(params, [np.tanh(in_tensor).astype(in_dtype)], ktype="float")
        else:
            if qrec.cache.get('kernel_type') == "lut":
                return qrec.get_outputs(params, [tanh_lut_float(in_tensor, in_dtype)], ktype="float")
            return qrec.get_outputs(params, [np_fasttanh(in_tensor, dtype=in_dtype)], ktype="float")


@params_type(HTanHNode)
@qrec_type('float')
class HTanHFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        return qrec.get_outputs(params, [np.minimum(np.maximum(in_tensor, in_dtype(-1.0)), in_dtype(1.0))], ktype="float")


@params_type(ReluNode)
@qrec_type('float')
class ReluFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        if params.upper_bound is None:
            return qrec.get_outputs(params,
                                    [np.maximum(in_tensor, in_dtype(
                                        params.lower_bound))],
                                    ktype="float")
        return qrec.get_outputs(params,
                                [np.minimum(np.maximum(in_tensor, in_dtype(params.lower_bound)),
                                            in_dtype(params.upper_bound))],
                                ktype="float")


@params_type(LeakyNode)
@qrec_type('float')
class LeakyFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dtype = qrec.in_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        leak_factor = in_dtype(params.leak_factor)
        output = in_tensor * (in_tensor > 0) + in_tensor * \
            leak_factor * (in_tensor < 0)
        return qrec.get_outputs(params, [output], ktype="float")
