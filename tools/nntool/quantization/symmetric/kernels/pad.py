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

from quantization.quantization_record_base import QuantizationRecordBase


def pad(params,
        in_tensors,
        qrec: QuantizationRecordBase,
        details=None):
    del qrec, details
    if params.pad_type == "zero":
        return [np.pad(in_tensors[0], params.padding.numpy_pad_shape(params.in_dims[0]),
                       'constant', constant_values=0)]
    raise NotImplementedError()
