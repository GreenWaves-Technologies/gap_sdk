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

from nntool.quantization.new_qrec import QRec
from nntool.graph.types import FusionInputNode, FusionOutputNode
from .kernel_base import KernelBase, qrec_type, params_type

@params_type(FusionInputNode, FusionOutputNode)
@qrec_type('any')
class FusionNoopKernel(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = in_tensors[params.idx]
        in_tensor = in_tensor.reshape(params.dims.shape)
        return [in_tensor]
