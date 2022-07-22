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

from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.graph.types import BatchNormalizationNode
from nntool.quantization.new_qrec import QRec

@params_type(BatchNormalizationNode)
@qrec_type('float')
class BatchNormF32(KernelBase):
    @classmethod
    def execute(cls, params: BatchNormalizationNode,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = in_tensors[0]
        in_dim, out_dim = params.in_dims[0], params.out_dims[0]
        in_tensor = in_tensor.transpose(in_dim.transpose_to_order(['c', 'h', 'w']))

        running_mean = params.running_mean.reshape(-1, 1, 1)
        running_var = params.running_variance.reshape(-1, 1, 1)
        scale = params.scale.reshape(-1, 1, 1)
        bias = params.bias.reshape(-1, 1, 1)
        res = (in_tensor - running_mean) / np.sqrt(running_var + params.epsilon) * scale + bias

        res = res.transpose(out_dim.transpose_from_order(['c', 'h', 'w']))
        return [res]
