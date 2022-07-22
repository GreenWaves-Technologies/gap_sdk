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
from nntool.graph.types import (MatMulOpNode, MatrixAddNode,
                         MatrixDivNode, MatrixMulNode,
                         MatrixSubNode)
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import MatScaleFusionNode
from nntool.graph.types.tensor_arithmetic import BroadcastableNodeBase, MatMulTransposedNode
from nntool.execution.kernels.kernel_base import (KernelBase, params_type,
                                              qrec_type)
from nntool.quantization.qtype import QType
from nntool.quantization.new_qrec import QRec
from nntool.utils.at_norm import at_norm

PIECEWISE_OPS = {
    MatrixAddNode: {'op': lambda x, y, dtype: x + y, 'is_mult': False},
    MatrixMulNode: {'op': lambda x, y, dtype: np.multiply(x, y, dtype=dtype), 'is_mult': True},
    MatrixSubNode: {'op': lambda x, y, dtype: x - y, 'is_mult': False},
    MatrixDivNode: {'op': lambda x, y, dtype: x / y, 'is_mult': True},
}

LOG = logging.getLogger(__name__)


@params_type(MatrixAddNode, MatrixDivNode,
             MatrixMulNode, MatrixSubNode)
@qrec_type('scaled')
class PieceWiseSymmetricMult(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        if isinstance(params, BroadcastableNodeBase) and params.is_broadcasted:
            in_tensors = params.broadcast_inputs(in_tensors)
        func = PIECEWISE_OPS[params.__class__]
        op = func['op']

        if func['is_mult']:
            # compute_in_out_scale(qrec, in_idx=(0, 1), out_idx=0)
            scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
            i1 = in_tensors[0].astype(np.int32)
            i2 = in_tensors[1].astype(np.int32)
            out_tensor = scale_mul_biases_q.apply_scales(op(i1, i2, np.int32))
        else:
            # larger scale should be scaled
            # set_add_in_scale(qrec)
            scale_mul_biases_q = qrec.cache['scale_mul_biases_q']
            if qrec.cache['scaled_idx']:
                i1 = in_tensors[0].astype(np.int32)
                i2 = qrec.cache['scale_in_mul_biases_q'].apply_scales(in_tensors[1])
            else:
                i1 = qrec.cache['scale_in_mul_biases_q'].apply_scales(in_tensors[0])
                i2 = in_tensors[1].astype(np.int32)

            out_tensor = scale_mul_biases_q.apply_scales(op(i1, i2, None)) + qrec.cache['add_bias_offset']
        return qrec.get_outputs(params, [qrec.out_qs[0].clip(out_tensor)], ktype="symmetric")


@params_type(MatrixAddNode, MatrixDivNode,
             MatrixMulNode, MatrixSubNode)
@qrec_type('symmetric')
class PieceWiseSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        func = PIECEWISE_OPS[params.__class__]
        op = func['op']
        if func['is_mult']:
            i1 = in_tensors[0].astype(np.int32)
            i2 = in_tensors[1].astype(np.int32)
            res = op(i1, i2, np.int32)
            q_calc = QType.Pow2(
                bits=32, q=qrec.in_qs[0].q+qrec.in_qs[1].q, signed=True)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
        else:
            off_in = abs(qrec.in_qs[0].q - qrec.in_qs[1].q)
            if qrec.in_qs[0].q > qrec.in_qs[1].q:
                i1 = at_norm(in_tensors[0].astype(np.int32), off_in)
                i2 = in_tensors[1].astype(np.int32)
            else:
                i1 = in_tensors[0].astype(np.int32)
                i2 = at_norm(in_tensors[1].astype(np.int32), off_in)
            res = op(i1, i2, None)
            q_calc = QType.Pow2(bits=32, q=min(qrec.in_qs[0].q, qrec.in_qs[1].q), signed=True)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
        return qrec.get_outputs(params, [res], ktype="symmetric")


@params_type(MatScaleFusionNode)
@qrec_type('symmetric', 'scaled')
class MatScaleSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
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
            q_calc = QType.Pow2(bits=32, q=qrec.in_qs[0].q +
                                qrec.in_qs[1].q + qrec.in_qs[2].q, signed=True)
            res = np.multiply(np.multiply(in_tensors[0], in_tensors[1],
                                          dtype=np.int32),
                              in_tensors[2],
                              dtype=np.int32)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
        elif qrec.in_qs[0].bits == 16:
            q_calc = QType.Pow2(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
            res = np.multiply(in_tensors[0], in_tensors[1], dtype=np.int32)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
            q_calc = QType.Pow2(bits=32, q=qrec.in_qs[2].q + qrec.out_qs[0].q, signed=True)
            res = np.multiply(res, in_tensors[2], dtype=np.int32)
            res = qrec.out_qs[0].reduce_from(res, q_calc)
        else:
            raise ValueError("only 8 and 16 bits supported")
        return res

    @classmethod
    def matscale2(cls, in_tensors, qrec=None):
        assert qrec.in_qs[0].bits == qrec.in_qs[1].bits
        q_calc = QType.Pow2(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
        res = np.multiply(in_tensors[0], in_tensors[1], dtype=np.int32)
        res = qrec.out_qs[0].reduce_from(res, q_calc)
        return res


@params_type(ExpressionFusionNode)
@qrec_type('symmetric', 'scaled')
class ExpressionSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        details = kwargs.get('details')
        if details is not None:
            results = {}
        else:
            results = None

        in_vars = {params.input_symbols[i]: in_tensor.copy()
                   for i, in_tensor in enumerate(in_tensors)}
        out_vars = qrec.cache['qfunc_col'](**in_vars, track_results=results)
        out_tensors = [out_vars[out_sym_name]
                       for out_sym_name in params.output_symbols]
        if details is not None:
            details['results'] = results
        return qrec.get_outputs(params, out_tensors, ktype="symmetric")

@params_type(MatMulOpNode, MatMulTransposedNode)
@qrec_type('scaled')
class MatMulScaled(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        in_tensors = [in_tensor.astype(np.int32) for in_tensor in qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")]
        if isinstance(params, MatMulTransposedNode):
            mat1, mat2 = in_tensors[0], np.swapaxes(in_tensors[1], -2, -1)
        else:
            mat1, mat2 = in_tensors[0], in_tensors[1]

        mat2 = mat2.astype(np.int32) - qrec.in_qs[1].zero_point.astype(np.int32)

        if len(in_tensors) > 2:
            biases = in_tensors[2]
            if len(biases.shape) == 1:
                if biases.shape[0] == mat1.shape[0]:
                    biases = np.expand_dims(biases, -1)
        else:
            biases = 0

        out_tensor = np.matmul(mat1, mat2) + biases
        out_rank = len(out_tensor.shape)
        mul_biases_q = qrec.cache['mul_biases_q']
        scale_axis = None if len(mul_biases_q.scale) == 1 else \
            (out_rank-1 if isinstance(params, MatMulTransposedNode) else out_rank-2)
        out_tensor = mul_biases_q.apply_scales(out_tensor, scale_axis)

        return qrec.get_outputs(params, [out_tensor], ktype="symmetric")

@params_type(MatMulOpNode, MatMulTransposedNode)
@qrec_type('symmetric')
class MatMulSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        in_tensors = [in_tensor.astype(np.int32) for in_tensor in qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")]

        if isinstance(params, MatMulTransposedNode):
            mat1, mat2 = in_tensors[0], np.swapaxes(in_tensors[1], -2, -1)
        else:
            mat1, mat2 = in_tensors[0], in_tensors[1]

        if len(in_tensors) > 2:
            biases = in_tensors[2]
            if len(biases.shape) == 1:
                if biases.shape[0] == mat1.shape[0]:
                    biases = np.expand_dims(biases, -1)
        else:
            biases = 0

        # expect biases in in_q1 + in_q2
        q_calc = QType.Pow2(bits=32, q=qrec.in_qs[0].q + qrec.in_qs[1].q, signed=True)
        out_tensor = np.matmul(mat1, mat2) + biases
        out_tensor = qrec.out_qs[0].reduce_from(out_tensor, q_calc)

        return qrec.get_outputs(params, [out_tensor], ktype="symmetric")
