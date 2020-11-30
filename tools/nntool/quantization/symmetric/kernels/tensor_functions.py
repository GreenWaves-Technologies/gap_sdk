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
from skimage.transform import resize
from graph.types import (ConstantInputParameters, InputParameters,
                         OutputParameters)
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              quantization)
from quantization.quantization_record_base import QuantizationRecordBase


@params_type(InputParameters)
@quantization('symmetric')
class InputSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        # all graph inputs are passed all of the inputs of the graph
        # params.index indicates the index of the input that this node should output
        in_tensor = in_tensors[params.index]
        if in_tensor.size == params.dims.size():
            in_tensor = in_tensor.reshape(params.dims.shape)
        else:
            in_tensor = resize(in_tensor, params.dims.shape)
        if params.transpose_out:
            in_tensor = np.transpose(in_tensor, params.transpose_out)
        # output_tensors = qrec.get_outputs(params, [in_tensor], ktype="symmetric")
        return [qrec.out_qs[0].quantize(in_tensor)]


@params_type(OutputParameters)
@quantization('symmetric')
class OutputSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        del qrec
        in_tensor = in_tensors[0]
        if params.transpose_in:
            in_tensor = np.transpose(in_tensor, params.transpose_in[0])
        return [in_tensor]


@params_type(ConstantInputParameters)
@quantization('symmetric')
class ConstantInputSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        del in_tensors

        return [qrec.out_qs[0].get_quantized(params.value,
                                             container_is_quantized=qrec.constants_are_quantized)]
