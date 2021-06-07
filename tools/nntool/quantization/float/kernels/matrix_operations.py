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

from graph.types.others import AbsOpParameters, CosOpParameters, LogOpParameters, MaxOpParameters, MinOpParameters, PowOpParameters, SinOpParameters, SqrtOpParameters, UnaryOpParameters
import numpy as np
from graph.types import (ExpOpParameters, ExpressionFusionParameters,
                         MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters,
                         MatScaleFusionParameters)
from graph.types.tensor_arithmetic import Broadcastable, MatMulOpParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import AllFloatQRec, QRec
from expressions.symbolic.symbol import Symbol, SymbolStats


class PieceWiseFloat32Mixin():
    @classmethod
    def execute_piecewise(cls, params,
                          in_tensors,
                          qrec: QRec,
                          op,
                          **kwargs):
        del kwargs
        if qrec is None:
            qrec = AllFloatQRec()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float")
        if isinstance(params, Broadcastable) and params.is_broadcasted:
            in_tensors = params.broadcast_inputs(in_tensors)

        out_tensor = op(in_tensors[0], in_tensors[1])
        return qrec.get_outputs(params, [out_tensor], ktype="float")


@params_type(MatrixAddParameters)
@qrec_type('float')
class MatrixAddFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        return super(MatrixAddFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: x + y,
            **kwargs
        )


@params_type(MatrixMulParameters)
@qrec_type('float')
class MatrixMulFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        return super(MatrixMulFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: np.multiply(x, y, dtype=np.float),
            **kwargs
        )


@params_type(MatrixSubParameters)
@qrec_type('float')
class MatrixSubFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        return super(MatrixSubFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: x - y,
            **kwargs
        )


@params_type(MatrixDivParameters)
@qrec_type('float')
class MatrixDivFloat32(PieceWiseFloat32Mixin, KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        return super(MatrixDivFloat32, cls).execute_piecewise(
            params,
            in_tensors,
            qrec,
            lambda x, y: x / y,
            **kwargs
        )


@params_type(MatMulOpParameters)
@qrec_type('float')
class MatMulFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float")
        if len(in_tensors) > 2:
            biases = in_tensors[2]
            if len(biases.shape) == 1:
                biases = np.expand_dims(biases, -1)
        else:
            biases = 0
        output_tensor = np.matmul(in_tensors[0], in_tensors[1]) + biases
        return qrec.get_outputs(params, [output_tensor], ktype="float")


@params_type(MatScaleFusionParameters)
@qrec_type('float')
class MatScaleFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float")
        if len(params.in_dims) == 3:
            return qrec.get_outputs(params, [in_tensors[0] * in_tensors[1] * in_tensors[2]], ktype="float")
        return qrec.get_outputs(params, [in_tensors[0] * in_tensors[1]], ktype="float")


@params_type(ExpressionFusionParameters)
@qrec_type('float')
class ExpressionFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        if qrec is None:
            qrec = AllFloatQRec()
        details = kwargs.get('details')
        if details is not None:
            current_control = SymbolStats()
            Symbol.set_default_control(current_control)
            results = {}
        else:
            results = None
            current_control = None
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float")
        in_vars = {params.input_symbols[i]: in_tensor
                   for i, in_tensor in enumerate(in_tensors)}
        out_vars = params.func_col(**in_vars,
                                   calculate_ranges=current_control is not None,
                                   track_results=results)
        out_tensors = [out_vars[out_sym_name]
                       for out_sym_name in params.output_symbols]
        if current_control:
            details.update(current_control.stats)
            details['results'] = results
        return qrec.get_outputs(params, out_tensors, ktype="float")

class BinaryOpFloat32(KernelBase):
    FUNC = lambda x: None
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float")
        output = cls.FUNC(*in_tensors)
        return qrec.get_outputs(params, [output], ktype="float")

@params_type(MinOpParameters)
@qrec_type('float')
class MinFloat32(BinaryOpFloat32):
    FUNC = np.minimum

@params_type(MaxOpParameters)
@qrec_type('float')
class MaxFloat32(BinaryOpFloat32):
    FUNC = np.maximum

@params_type(PowOpParameters)
@qrec_type('float')
class PowFloat32(BinaryOpFloat32):
    FUNC = np.power

class UnaryOpFloat32(KernelBase):
    FUNC = lambda x: None
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        output = cls.FUNC(in_tensor)
        return qrec.get_outputs(params, [output], ktype="float")

@params_type(ExpOpParameters)
@qrec_type('float')
class ExpFloat32(UnaryOpFloat32):
    FUNC = np.exp

@params_type(CosOpParameters)
@qrec_type('float')
class CosFloat32(UnaryOpFloat32):
    FUNC = np.cos

@params_type(SinOpParameters)
@qrec_type('float')
class SinFloat32(UnaryOpFloat32):
    FUNC = np.sin

@params_type(AbsOpParameters)
@qrec_type('float')
class AbsFloat32(UnaryOpFloat32):
    FUNC = np.abs

@params_type(LogOpParameters)
@qrec_type('float')
class LogFloat32(UnaryOpFloat32):
    FUNC = np.log

@params_type(SqrtOpParameters)
@qrec_type('float')
class SqrtFloat32(UnaryOpFloat32):
    FUNC = np.sqrt
