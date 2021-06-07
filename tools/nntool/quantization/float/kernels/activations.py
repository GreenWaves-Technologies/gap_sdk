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

from graph.types.activations import HTanHActivationParameters, TanHActivationParameters
import numpy as np
from graph.types import (HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         ReluActivationParameters, SigmoidActivationParameters)
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import AllFloatQRec, QRec


@params_type(HSwishActivationParameters)
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
        return qrec.get_outputs(params, [in_tensor * np.minimum(np.maximum(in_tensor + 3, 0), 6) / 6], ktype="float")


@params_type(HSigmoidActivationParameters)
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
        return qrec.get_outputs(params, [np.minimum(np.maximum(in_tensor + params.offset, 0), 6) / 6], ktype="float")


@params_type(SigmoidActivationParameters)
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
        return qrec.get_outputs(params, [1 / (1 + np.exp(-in_tensor))], ktype="float")

@params_type(TanHActivationParameters)
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
        return qrec.get_outputs(params, [np.tanh(in_tensor)], ktype="float")

@params_type(HTanHActivationParameters)
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
        return qrec.get_outputs(params, [np.minimum(np.maximum(in_tensor, -1.0), 1.0)], ktype="float")

# Not used currently - need a way to select this


# def sigmoid(params,
#             in_tensors,
#             qrec: QRec,
#             **kwargs):
#     del details
#     if qrec is None:
#         qrec = AllFloatQRec()
#     in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
#     return qrec.get_outputs(params, [1/(1 + np.exp(-in_tensor))], ktype="float")


@params_type(ReluActivationParameters)
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
        if params.upper_bound is None:
            return qrec.get_outputs(params,
                                    [np.maximum(in_tensor,
                                                params.lower_bound)],
                                    ktype="float")
        return qrec.get_outputs(params,
                                [np.minimum(np.maximum(in_tensor,
                                                       params.lower_bound),
                                            params.upper_bound)],
                                ktype="float")


@params_type(LeakyActivationParameters)
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
        output = in_tensor * (in_tensor > 0) + in_tensor * \
            params.leak_factor * (in_tensor < 0)
        return qrec.get_outputs(params, [output], ktype="float")
