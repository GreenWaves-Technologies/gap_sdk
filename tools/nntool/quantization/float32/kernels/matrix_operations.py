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
from graph.types import (ExpOpParameters, ExpressionFusionParameters,
                         MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters,
                         MatScaleFusionParameters)
from graph.types.tensor_arithmetic import Broadcastable, MatMulOpParameters
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              quantization)
from quantization.quantization_record_base import QuantizationRecordBase
from utils.symbolic.symbol import Symbol, SymbolStats


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
        if params.transpose_in:
            in_tensors = [(np.transpose(in_tensor, params.transpose_in[idx]) if params.transpose_in[idx] else in_tensor)
                          for idx, in_tensor in enumerate(in_tensors)]
        if isinstance(params, Broadcastable) and params.is_broadcasted:
            in_tensors = params.broadcast_inputs(in_tensors)

        out_tensor = op(in_tensors[0], in_tensors[1])
        if params.transpose_out:
            out_tensor = np.transpose(out_tensor, params.transpose_out[0])
        return qrec.get_outputs(params, [out_tensor], ktype="float32")


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


@params_type(MatMulOpParameters)
@quantization('float32')
class MatMulFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        if qrec is None:
            qrec = Float32QuantizationRecord()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
        if len(in_tensors) > 2:
            biases = in_tensors[2]
        else:
            biases = 0

        return qrec.get_outputs(params, [np.matmul(in_tensors[0], in_tensors[1]) + biases], ktype="float32")


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
        details = kwargs.get('details')
        if details is not None:
            current_control = SymbolStats()
            Symbol.set_default_control(current_control)
            results = {}
        else:
            results = None
            current_control = None
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
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
        return qrec.get_outputs(params, out_tensors, ktype="float32")


@params_type(ExpOpParameters)
@quantization('float32')
class ExpFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        if qrec is None:
            qrec = Float32QuantizationRecord()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
        output = np.exp(in_tensor)
        return qrec.get_outputs(params, [output], ktype="float32")
