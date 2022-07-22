# Copyright (C) 2020, 2022  GreenWaves Technologies, SAS

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

from copy import deepcopy

import numpy as np
from nntool.graph.types import (MatrixDivNode, MinOpNode, MaxOpNode, PowOpNode,
                                MatrixMulNode, MatScaleFusionNode,
                                UnaryOpNodeBase)
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                              out_qs_constraint,
                                                              params_type)

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(MatrixDivNode, MatrixMulNode,
             MinOpNode, MaxOpNode, PowOpNode, UnaryOpNodeBase,
             MatScaleFusionNode)
@in_qs_constraint(MatchAll({'dtype': np.int8}))
@out_qs_constraint(MatchAll({'dtype': np.int8}))
class FromStatsMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_qs = cls.force_symmetric_and_dtype(in_qs, dtype=np.int8)
        if in_qs is None:
            return None
        if force_out_q:
            o_q = deepcopy(force_out_q)
            if o_q.asymmetric:
                return None
        else:
            o_q = QType.from_min_max_sq(*stats.get_range_out(0, dtype=out_dtype),
                                        dtype=out_dtype)
        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        scale_mul_biases_q.scale = (
            np.prod([qtype.scale for qtype in in_qs]) / o_q.scale)

        return QRec.scaled(in_qs=in_qs, out_qs=[o_q], scale_mul_biases_q=scale_mul_biases_q)
