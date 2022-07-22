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
from nntool.graph.types.activations import SoftMaxNode
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import QRec
from nntool.utils.exp_17_15 import exp_fp_17_15
from nntool.utils.at_norm import at_norm


def softmax_func(arg, axis=None):
    if axis is None:
        axis = -1
    v = arg - np.max(arg, axis=axis, keepdims=True)
    exp_v = np.exp(v)
    v = exp_v/np.sum(exp_v, axis=axis, keepdims=True)
    if len(arg.shape) == 1:
        v = v.flatten()
    return v


@params_type(SoftMaxNode)
@qrec_type('symmetric')
class SoftMaxSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        old_err = np.seterr(over='raise')
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        # TODO - Implement properly quantized version
        in_tensor = qrec.in_qs[0].dequantize(in_tensor)
        in_tensor = qrec.out_qs[0].quantize(softmax_func(in_tensor, axis=params.axis))
        np.seterr(**old_err)
        return qrec.get_outputs(params, [in_tensor], ktype="symmetric")


@params_type(SoftMaxNode)
@qrec_type('scaled')
class SoftMaxSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        # in_tensor = in_tensors[0].flatten()
        in_tensor = in_tensors[0].astype(np.int32)
        max_val = np.max(in_tensor, axis=params.axis, keepdims=True)
        norm = 15 + np.ceil(np.log2(qrec.in_qs[0].scale)).astype(np.int32)
        exp = exp_fp_17_15((in_tensor.astype(np.int32) - max_val) << (norm))
        sum_exp = np.sum(exp, axis=params.axis, keepdims=True)
        inv_sum = (np.array([(1 << 15)-1], dtype=np.uint32) << 15)//sum_exp
        res = np.abs(at_norm(exp * inv_sum, 23 if qrec.out_qs[0].dtype_bits == 8 else 15))
        iinfo = np.iinfo(qrec.out_qs[0].dtype)
        res = np.clip(res, iinfo.min, iinfo.max).astype(
            qrec.out_qs[0].dtype).reshape(params.out_dims[0].shape)
        return qrec.get_outputs(params, [res], ktype="symmetric")
