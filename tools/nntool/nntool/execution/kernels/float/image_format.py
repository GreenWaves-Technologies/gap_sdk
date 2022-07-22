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
from nntool.graph.types import ImageFormatNode
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import QRec
from nntool.utils.formatters import FORMAT_CHANGES, NORMALIZATIONS


@params_type(ImageFormatNode)
@qrec_type('float')
class ImageFormatFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        in_dim, out_dim = params.in_dims[0], params.out_dims[0]
        res = in_tensors[0]
        res = FORMAT_CHANGES[params.format_change](res, in_dim, out_dim)
        res = NORMALIZATIONS[params.norm_func](res)
        if qrec.ktype.startswith('float'):
            iinfo = np.iinfo(res.dtype)
            if res.dtype == np.int8 or res.dtype == np.int16:
                res = res.astype(qrec.out_qs[0].dtype) / (iinfo.min * -1)
            else:
                raise ValueError(
                    "unsure how to dequantize this output from imageformatter")
            return [qrec.out_qs[0].quantize(res)]
        return [qrec.out_qs[0].dequantize(res) if qrec.out_qs else res]
