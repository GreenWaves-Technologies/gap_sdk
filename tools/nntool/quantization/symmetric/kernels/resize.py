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

import math
import numpy as np

from quantization.quantization_record_base import QuantizationRecordBase

def resize_bilinear(params,
                    in_tensors,
                    qrec: QuantizationRecordBase,
                    details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    in_tensor = in_tensor.transpose(params.in_dims[0].transpose_to_order(("h", "w", "c")))
    w_out = params.out_dims[0].w
    h_out = params.out_dims[0].h
    c_out = params.out_dims[0].c
    w_in = params.in_dims[0].w
    h_in = params.in_dims[0].h
    wstep = ((w_in - 1) << 16) // w_out
    hstep = ((h_in - 1) << 16) // h_out
    hcoeff = wcoeff = 0
    out_tensor = np.empty((h_out, w_out, c_out))
    for i in range(h_out):
        offsetY = hcoeff >> 16
        hc2 = (hcoeff >> 9) & 127
        hc1 = 128 - hc2
        wcoeff = 0
        for j in range(w_out):
            offsetX = wcoeff >> 16
            wc2 = (wcoeff >> 9) & 127
            wc1 = 128 - wc2
            P1 = in_tensor[offsetY, offsetX, :].astype(np.int32)
            P2 = in_tensor[offsetY+1, offsetX, :].astype(np.int32)
            P3 = in_tensor[offsetY, offsetX+1, :].astype(np.int32)
            P4 = in_tensor[offsetY+1, offsetX+1, :].astype(np.int32)
            out_tensor[i, j, :] = (P1 * wc1 * hc1 \
                                   + P2 * wc1 * hc2 \
                                   + P3 * wc2 * hc1 \
                                   + P4 * wc2 * hc2).astype(np.int32) >> 14
            wcoeff += wstep
        hcoeff += hstep

    out_tensor = out_tensor.transpose(params.out_dims[0].transpose_from_order(("h", "w", "c")))
    return qrec.get_outputs(params, [out_tensor], ktype="symmetric")

def resize_nearest_neighbor(params,
                            in_tensors,
                            qrec: QuantizationRecordBase,
                            details=None):
    del details
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    in_tensor = in_tensor.transpose(params.in_dims[0].transpose_to_order(("h", "w", "c")))
    w_out = params.out_dims[0].w
    h_out = params.out_dims[0].h
    c_out = params.out_dims[0].c
    w_in = params.in_dims[0].w
    h_in = params.in_dims[0].h
    wstep = ((w_in - 1) << 16) // (w_out - 1)
    hstep = ((h_in - 1) << 16) // (h_out - 1)
    out_tensor = np.empty((h_out, w_out, c_out))
    for i in range(h_out):
        h_rounded = ((hstep * i) + (1 << (16 - 1))) >> 16
        for j in range(w_out):
            w_rounded = ((wstep * j) + (1 << (16 - 1))) >> 16
            out_tensor[i, j, :] = in_tensor[h_rounded, w_rounded, :]

    out_tensor = out_tensor.transpose(params.out_dims[0].transpose_from_order(("h", "w", "c")))
    return qrec.get_outputs(params, [out_tensor], ktype="symmetric")
