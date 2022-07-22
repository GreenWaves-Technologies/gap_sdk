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
from nntool.graph.types import (GlobalPoolingNodeBase, MatrixAddNode,
                                MatrixBroadcastedLinearOpNodeBase,
                                MatrixSubNode, MatScaleFusionNode, MaxOpNode,
                                MinOpNode, PowOpNode, UnaryOpNodeBase)
from nntool.graph.types.fusions import FusionInputNode
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.unified_quantization_handler import (
    in_qs_constraint, out_qs_constraint, params_type)

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


@params_type(MatrixAddNode, MatrixSubNode,
             MatrixBroadcastedLinearOpNodeBase,
             MatScaleFusionNode, GlobalPoolingNodeBase, MinOpNode,
             MaxOpNode, PowOpNode, UnaryOpNodeBase,
             FusionInputNode)
@in_qs_constraint(MatchAll({'dtype': set([np.int8, np.int16])}))
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
class FromStatsMultiInputPow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_pow2_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        o_q = QType.from_min_max_pow2(*stats.get_range_out(0, dtype=out_dtype),
                                      dtype=out_dtype)
        if force_out_q:
            if not force_out_q.is_pow2:
                return None
            if force_out_q.bits - force_out_q.q < o_q.bits - o_q.q:
                LOG.warning('%s is being forced to output in Q%s and may clip',
                            params.name, force_out_q.q)
            o_q = force_out_q

        return QRec.symmetric(in_qs=in_qs, out_qs=[o_q])
