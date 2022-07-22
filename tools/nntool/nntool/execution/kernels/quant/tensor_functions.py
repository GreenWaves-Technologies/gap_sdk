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

from nntool.graph.types import (ConstantInputNode, InputNode,
                         OutputNode)
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import QRec


@params_type(InputNode)
@qrec_type('symmetric', 'scaled')
class InputSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        # all graph inputs are passed all of the inputs of the graph
        # params.index indicates the index of the input that this node should output
        in_tensor = in_tensors[params.index]
        if in_tensor.size == params.dims.size():
            if len(in_tensor.shape) == len(params.dims.shape):
                in_shape = tuple(dim for dim in in_tensor.shape if dim > 1)
                expected_shape = tuple(
                    dim for dim in params.dims.shape if dim > 1)
                if in_shape != expected_shape:
                    raise ValueError(
                        f'{params.name} received input of shape {in_tensor.shape} but expecting {params.dims.shape}')
            in_tensor = in_tensor.reshape(params.dims.shape)
        else:
            raise ValueError(
                f'{params.name} received input of shape {in_tensor.shape} but expecting {params.dims.shape}')
        # output_tensors = qrec.get_outputs(params, [in_tensor], ktype="symmetric")
        if in_tensor.dtype != qrec.out_qs[0].dtype:
            in_tensor = qrec.out_qs[0].quantize(in_tensor)
        return [in_tensor]


@params_type(OutputNode)
@qrec_type('symmetric', 'scaled')
class OutputSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        del qrec
        in_tensor = in_tensors[0]
        return [in_tensor]


@params_type(ConstantInputNode)
@qrec_type('symmetric', 'scaled')
class ConstantInputSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        del in_tensors
        return [params.value_as(qrec.out_qs[0])]
