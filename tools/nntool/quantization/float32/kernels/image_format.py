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

from utils.formatters import FORMAT_CHANGES, NORMALIZATIONS


def image_format(params, in_tensors, qrec, details):
    del details
    in_dim = params.in_dims[0]
    out_dim = params.out_dims[0]
    res = in_tensors[0]
    res = FORMAT_CHANGES[params.format_change](res, in_dim, out_dim)
    res = NORMALIZATIONS[params.norm_func](res)
    if not qrec:
        iinfo = np.iinfo(res.dtype)
        if res.dtype == np.int8:
            res = res.astype(np.float32) / -iinfo.min
        elif res.dtype == np.int16:
            res = res.astype(np.float32) / -iinfo.min
        else:
            raise ValueError("unsure how to dequantize this output from imageformatter")
        return [res]
    return [qrec.out_qs[0].dequantize(res)]
