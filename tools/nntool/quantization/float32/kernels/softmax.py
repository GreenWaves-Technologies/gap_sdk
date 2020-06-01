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
import scipy.special

from quantization.quantization_record_base import QuantizationRecordBase
from quantization.float32.float32_quantization import Float32QuantizationRecord

def softmax_func(v):
    return scipy.special.softmax(v)

def softmax(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    np.seterr(over='raise')
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float32")[0]
    return qrec.get_outputs(params, [softmax_func(in_tensor)], ktype="float32")
