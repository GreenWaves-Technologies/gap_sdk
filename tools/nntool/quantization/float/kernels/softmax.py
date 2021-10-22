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
import scipy.special
from graph.types import SoftMaxParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import AllFloatQRec, QRec


def softmax_func(v, axis=None):
    return scipy.special.softmax(v, axis=axis)


@params_type(SoftMaxParameters)
@qrec_type('float')
class SoftMaxFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        if qrec is None:
            qrec = AllFloatQRec()
        old_err = np.seterr(over='raise')
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        out_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        in_tensor = softmax_func(in_tensor, axis=params.axis).astype(out_dtype)
        np.seterr(**old_err)
        return qrec.get_outputs(params, [in_tensor], ktype="float")
