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

from nntool.graph.types.misc import PadNode
import numpy as np

from nntool.quantization.new_qrec import QRec

from nntool.execution.kernels.kernel_base import (KernelBase, params_type,
                                              qrec_type)
@params_type(PadNode)
@qrec_type('symmetric', 'scaled')
class PadSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        pad_vals = [(qrec.in_qs[0].quantize(val[0])[0], qrec.in_qs[0].quantize(val[1])[0])
                    for val in params.pad_vals]
        return [np.pad(in_tensors[0], params.padding, 'constant', constant_values=pad_vals)]
