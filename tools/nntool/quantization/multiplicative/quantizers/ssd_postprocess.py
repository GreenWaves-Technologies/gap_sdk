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
from graph.types import SSDDetectorParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type, in_qs_constraint

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(SSDDetectorParameters)
@in_qs_constraint({'dtype': np.int8})
class SSDDetectorParametersMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params: SSDDetectorParameters, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            return None

        in_qs = cls.force_symmetric_and_dtype(in_qs, dtype=np.int8)
        if in_qs is None:
            return None
        o_boxes_qtype = QType(min_val=-2, max_val=2,
                              dtype=np.int16, scale=2**(-14))
        o_scores_qtype = in_qs[1]
        o_class_qtype = QType(scale=1, dtype=np.int8)
        outputs = [o_boxes_qtype, o_class_qtype, o_scores_qtype]
        if params.output_detection_count:
            outputs.append(QType(scale=1, dtype=np.int32))
        return QRec.scaled(in_qs=in_qs, out_qs=outputs)
