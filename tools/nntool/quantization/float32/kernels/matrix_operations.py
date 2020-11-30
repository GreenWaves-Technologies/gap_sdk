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
from graph.types import (ExpressionFusionParameters, MatrixAddParameters,
                         MatrixDivParameters, MatrixMulParameters,
                         MatrixSubParameters, MatScaleFusionParameters)
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              quantization)
from quantization.quantization_record_base import QuantizationRecordBase


class PieceWiseFloat32Mixin():
    @classmethod
    def execute_piecewise(cls, params,
                          in_tensors,
                          qrec: QuantizationRecordBase,
                          op,
                          **kwargs):
        del kwargs
        if qrec is None:
            qrec = Float32QuantizationRecord()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
        return qrec.get_outputs(params, [op(in_tensors[0], in_tensors[1])], ktype="float32")


@params_type(MatrixAddParameters)
@quantization('float32')
class MatrixAddFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        return super(MatrixAddFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: x + y,
            **kwargs
        )


@params_type(MatrixMulParameters)
@quantization('float32')
class MatrixMulFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        return super(MatrixMulFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: np.multiply(x, y, dtype=np.float),
            **kwargs
        )


@params_type(MatrixSubParameters)
@quantization('float32')
class MatrixSubFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        return super(MatrixSubFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: x - y,
            **kwargs
        )


@params_type(MatrixDivParameters)
@quantization('float32')
class MatrixDivFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        return super(MatrixDivFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: x / y,
            **kwargs
        )


@params_type(MatScaleFusionParameters)
@quantization('float32')
class MatScaleFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        if qrec is None:
            qrec = Float32QuantizationRecord()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
        if len(params.in_dims) == 3:
            return qrec.get_outputs(params, [in_tensors[0] * in_tensors[1] * in_tensors[2]], ktype="float32")
        return qrec.get_outputs(params, [in_tensors[0] * in_tensors[1]], ktype="float32")


@params_type(ExpressionFusionParameters)
@quantization('float32')
class ExpressionFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        if qrec is None:
            qrec = Float32QuantizationRecord()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
        return qrec.get_outputs(params, params.execute(in_tensors), ktype="float32")
