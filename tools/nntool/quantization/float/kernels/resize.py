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
from graph.types import (BilinearResizerParameters,
                         NearestNeighborResizerParameters)
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import AllFloatQRec, QRec


@params_type(BilinearResizerParameters)
@qrec_type('float')
class BilinearResizerFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dim, out_dim = params.in_dims[0], params.out_dims[0]
        in_tensor = in_tensor.transpose(
            in_dim.transpose_to_order(("h", "w", "c")))
        w_out = out_dim.w
        h_out = out_dim.h
        c_out = out_dim.c
        w_in = in_dim.w
        h_in = in_dim.h
        wstep = (w_in - 1) / w_out
        hstep = (h_in - 1) / h_out
        out_tensor = np.empty((h_out, w_out, c_out))
        out_dtype = qrec.out_qs[0].dtype if qrec.ktype.startswith(
            'float') else np.float32
        for i in range(h_out):
            y_l, y_h = math.floor(hstep * i), math.ceil(hstep * i)
            hc = out_dtype((hstep * i) - y_l)
            for j in range(w_out):
                x_l, x_h = math.floor(wstep * j), math.ceil(wstep * j)
                wc = out_dtype((wstep * j) - x_l)
                P1 = in_tensor[y_l, x_l, :]
                P2 = in_tensor[y_l, x_h, :]
                P3 = in_tensor[y_h, x_l, :]
                P4 = in_tensor[y_h, x_h, :]
                out_tensor[i, j, :] = P1 * (out_dtype(1) - wc) * (out_dtype(1) - hc) \
                    + P2 * wc * (out_dtype(1) - hc) \
                    + P3 * (out_dtype(1) - wc) * hc \
                    + P4 * wc * hc

        out_tensor = out_tensor.transpose(
            out_dim.transpose_from_order(("h", "w", "c")))
        return qrec.get_outputs(params, [out_tensor], ktype="float")


@params_type(NearestNeighborResizerParameters)
@qrec_type('float')
class NearestNeighborResizerFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        if qrec is None:
            qrec = AllFloatQRec()
        in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="float")[0]
        in_dim, out_dim = params.in_dims[0], params.out_dims[0]
        in_tensor = in_tensor.transpose(
            in_dim.transpose_to_order(("h", "w", "c")))
        w_out = out_dim.w
        h_out = out_dim.h
        c_out = out_dim.c
        w_in = in_dim.w
        h_in = in_dim.h
        wstep = (w_in - 1) / (w_out - 1)
        hstep = (h_in - 1) / (h_out - 1)
        out_tensor = np.empty((h_out, w_out, c_out))
        for i in range(h_out):
            h_rounded = int(round(hstep * i))
            for j in range(w_out):
                w_rounded = int(round(wstep * j))
                out_tensor[i, j, :] = in_tensor[h_rounded, w_rounded, :]

        out_tensor = out_tensor.transpose(
            out_dim.transpose_from_order(("h", "w", "c")))
        return qrec.get_outputs(params, [out_tensor], ktype="float")
