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

from graph.types import (MatrixAddParameters, MatrixDivParameters,
                         MatrixMulParameters, MatrixSubParameters)
from quantization.quantization_record_base import QuantizationRecordBase
from quantization.float32.float32_quantization import Float32QuantizationRecord

PIECEWISE_OPS = {
    MatrixAddParameters: {'op': lambda x, y: x + y},
    MatrixMulParameters: {'op': lambda x, y: np.multiply(x, y, dtype=np.float)},
    MatrixSubParameters: {'op': lambda x, y: x - y},
    MatrixDivParameters: {'op': lambda x, y: x / y},
}


def piecewise(params,
              in_tensors,
              qrec: QuantizationRecordBase,
              details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    func = PIECEWISE_OPS[params.__class__]
    op = func['op']
    in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
    return qrec.get_outputs(params, [op(in_tensors[0], in_tensors[1])], ktype="float32")


def matscale(params,
             in_tensors,
             qrec: QuantizationRecordBase,
             details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
    if len(params.in_dims) == 3:
        return qrec.get_outputs(params, [in_tensors[0] * in_tensors[1] * in_tensors[2]], ktype="float32")
    return qrec.get_outputs(params, [in_tensors[0] * in_tensors[1]], ktype="float32")
