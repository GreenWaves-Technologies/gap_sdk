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

import logging

import numpy as np
from graph.types import (MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters)
from graph.types.expression_fusion import ExpressionFusionParameters
from graph.types.fusions import MatScaleFusionParameters
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              qrec_type, quantization)
from quantization.multiplicative.mult_quantization import (
    MultAddQuantizationRecord, MultQuantizationRecord)
from quantization.qtype import QType
from quantization.quantization_record_base import QuantizationRecordBase
from utils.at_norm import at_norm

PIECEWISE_OPS = {
    MatrixAddParameters: {'op': lambda x, y, dtype: x + y, 'is_mult': False},
    MatrixMulParameters: {'op': lambda x, y, dtype: np.multiply(x, y, dtype=dtype), 'is_mult': True},
    MatrixSubParameters: {'op': lambda x, y, dtype: x - y, 'is_mult': False},
    MatrixDivParameters: {'op': lambda x, y, dtype: x / y, 'is_mult': True},
}

LOG = logging.getLogger("nntool." + __name__)


@params_type(MatrixAddParameters, MatrixDivParameters,
             MatrixMulParameters, MatrixSubParameters)
@quantization('symmetric')
@qrec_type(MultQuantizationRecord, MultAddQuantizationRecord)
class PieceWiseSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        func = PIECEWISE_OPS[params.__class__]
        op = func['op']
        if func['is_mult']:
            qrec.set_scale(in_idx=(0, 1), out_idx=0)
            i1 = in_tensors[0].astype(np.int32)
            i2 = in_tensors[1].astype(np.int32)
            res = qrec.scale_mul_biases_q.apply_scales(op(i1, i2, np.int32))
        else:
            # larger scale should be scaled
            qrec.set_add_scale()
            if qrec.scaled_idx:
                i1 = in_tensors[0].astype(np.int32)
                i2 = qrec.scale_in_mul_biases_q.apply_scales(in_tensors[1])
            else:
                i1 = qrec.scale_in_mul_biases_q.apply_scales(in_tensors[0])
                i2 = in_tensors[1].astype(np.int32)

            res = qrec.scale_mul_biases_q.apply_scales(op(i1, i2, None))
        return qrec.get_outputs(params, [qrec.out_qs[0].clip(res)], ktype="symmetric")


@params_type(MatrixAddParameters, MatrixDivParameters,
             MatrixMulParameters, MatrixSubParameters)
@quantization('symmetric')
class PieceWiseSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):

        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        func = PIECEWISE_OPS[params.__class__]
        op = func['op']
        if func['is_mult']:
            i1 = in_tensors[0].astype(np.int32)
            i2 = in_tensors[1].astype(np.int32)
            res = op(i1, i2, np.int32)
        else:
            off_in = abs(qrec.in_qs[0].q - qrec.in_qs[1].q)
            if qrec.in_qs[0].q > qrec.in_qs[1].q:
                i1 = at_norm(in_tensors[0].astype(np.int32), off_in)
                i2 = in_tensors[1].astype(np.int32)
            else:
                i1 = in_tensors[0].astype(np.int32)
                i2 = at_norm(in_tensors[1].astype(np.int32), off_in)
            res = op(i1, i2, None)
        return qrec.get_outputs(params, [res], ktype="symmetric")


@params_type(MatScaleFusionParameters)
@quantization('symmetric')
class MatSCaleSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        LOG.debug("matscale input %s", ",".join([t.dtype.name for t in in_tensors]))
        if len(params.in_dims) == 3:
            output_tensor = cls.matscale3(in_tensors, qrec)
        else:
            output_tensor = cls.matscale2(in_tensors, qrec)
        return qrec.get_outputs(params, [output_tensor], ktype="symmetric")

    @classmethod
    def matscale3(cls, in_tensors, qrec):
        assert qrec.in_qs[0].bits == qrec.in_qs[1].bits
        assert qrec.in_qs[1].bits == qrec.in_qs[2].bits
        if qrec.in_qs[0].bits == 8:
            q_calc = QType(bits=32, q=qrec.in_qs[0].q +
                           qrec.in_qs[1].q + qrec.in_qs[2].q, signed=True)
            res = np.multiply(np.multiply(in_tensors[0], in_tensors[1],
                                          dtype=np.int32),
                              in_tensors[2],
                              dtype=np.int32)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
        elif qrec.in_qs[0].bits == 16:
            q_calc = QType(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
            res = np.multiply(in_tensors[0], in_tensors[1], dtype=np.int32)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
            q_calc = QType(bits=32, q=qrec.in_qs[2].q + qrec.out_qs[0].q, signed=True)
            res = np.multiply(res, in_tensors[2], dtype=np.int32)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
        else:
            raise ValueError("only 8 and 16 bits supported")
        return res

    @classmethod
    def matscale2(cls, in_tensors, qrec=None):
        assert qrec.in_qs[0].bits == qrec.in_qs[1].bits
        q_calc = QType(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
        res = np.multiply(in_tensors[0], in_tensors[1], dtype=np.int32)
        res = qrec.out_qs[0].reduce_from(res, q_calc)
        return res


@params_type(ExpressionFusionParameters)
@quantization('symmetric')
class ExpressionSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        in_tensors = [in_tensor.astype(np.int32) for in_tensor in qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")]
        out_tensors = params.execute(in_tensors,
                                     input_symbols=qrec.inputs,
                                     expr_inter=qrec.intermediate_exprs,
                                     expr_outputs=qrec.output_exprs,
                                     details=kwargs.get('details'))
        out_tensors = [out_tensor.astype(np.int8) for out_tensor in out_tensors]
        return qrec.get_outputs(params, out_tensors, ktype="symmetric")
