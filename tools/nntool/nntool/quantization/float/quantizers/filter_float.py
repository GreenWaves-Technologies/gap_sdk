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
from bfloat16 import bfloat16
from nntool.graph.types import Conv2DNode, LinearNode
from nntool.quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.quantizer_options import HWC_OPTION
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)

AT_CHW_KER_IN_ORDER = [['c', 'h', 'w'], ['out_c', 'in_c', 'h', 'w'], ['out_c']]
AT_CHW_KER_OUT_ORDER = [['c', 'h', 'w']]
AT_HWC_KER_IN_ORDER = [['h', 'w', 'c'], ['out_c', 'h', 'w', 'in_c'], ['out_c']]
AT_HWC_KER_OUT_ORDER = [['h', 'w', 'c']]


@options(
    HWC_OPTION
)
@params_type(Conv2DNode, LinearNode)
@in_qs_constraint({'dtype': set([np.float32, np.float16, bfloat16])})
@out_qs_constraint({'dtype': set([np.float32, np.float16, bfloat16])})
class FilterFloat(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, dtype = cls.get_float_opts(**kwargs)
        if force_out_qs and any(qtype.dtype != dtype for qtype in force_out_qs if qtype is not None):
            return None
        # all inputs and outputs are set to the required float type
        opts = kwargs['opts']
        if opts['hwc']:
            cls.check_order(params, AT_HWC_KER_IN_ORDER, AT_HWC_KER_OUT_ORDER)
        else:
            cls.check_order(params, AT_CHW_KER_IN_ORDER, AT_CHW_KER_OUT_ORDER)
        return QRec.float(in_qs=[QType(dtype=dtype)
                                 for _ in range(3)],
                          out_qs=[QType(dtype=dtype)],
                          float_dtype=dtype)
