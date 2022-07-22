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
from nntool.graph.types import (BilinearResizerNode,
                         NearestNeighborResizerNode)
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import QRec


@params_type(BilinearResizerNode)
@qrec_type('symmetric', 'scaled')
class BilinearResizerSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        in_dim, out_dim = params.in_dims[0], params.out_dims[0]
        in_tensor = in_tensor.transpose(
            in_dim.transpose_to_order(("h", "w", "c")))
        w_out = out_dim.w
        h_out = out_dim.h
        c_out = out_dim.c
        w_in = in_dim.w
        h_in = in_dim.h
        wstep = ((w_in - 1) << 16) // w_out
        hstep = ((h_in - 1) << 16) // h_out
        hcoeff = wcoeff = 0
        out_tensor = np.zeros((h_out, w_out, c_out), dtype=np.int32)
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
                out_tensor[i, j, :] = (P1 * wc1 * hc1
                                       + P2 * wc1 * hc2
                                       + P3 * wc2 * hc1
                                       + P4 * wc2 * hc2).astype(np.int32) >> 14
                wcoeff += wstep
            hcoeff += hstep

        out_tensor = out_tensor.transpose(
            out_dim.transpose_from_order(("h", "w", "c")))
        return qrec.get_outputs(params, [out_tensor], ktype="symmetric")


@params_type(NearestNeighborResizerNode)
@qrec_type('symmetric', 'scaled')
class NearestNeighbourResizerSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensor = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")[0]
        in_dim, out_dim = params.in_dims[0], params.out_dims[0]
        in_tensor = in_tensor.transpose(
            in_dim.transpose_to_order(("h", "w", "c")))
        w_out = out_dim.w
        h_out = out_dim.h
        c_out = out_dim.c
        w_in = in_dim.w
        h_in = in_dim.h
        wstep = ((w_in - 1) << 16) // (w_out - 1)
        hstep = ((h_in - 1) << 16) // (h_out - 1)
        out_tensor = np.zeros((h_out, w_out, c_out), dtype=np.int32)
        for i in range(h_out):
            h_rounded = ((hstep * i) + (1 << (16 - 1))) >> 16
            for j in range(w_out):
                w_rounded = ((wstep * j) + (1 << (16 - 1))) >> 16
                out_tensor[i, j, :] = in_tensor[h_rounded, w_rounded, :]

        out_tensor = out_tensor.transpose(
            out_dim.transpose_from_order(("h", "w", "c")))
        return qrec.get_outputs(params, [out_tensor], ktype="symmetric")
