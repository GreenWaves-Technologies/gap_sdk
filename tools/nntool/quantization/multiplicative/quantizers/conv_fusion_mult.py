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
from graph.types import ConvFusionParameters
from quantization.multiplicative.quantizers.filter_mult import (can_ne16,
                                                                check_option)
from quantization.new_qrec import QRec
from quantization.unified_quantization_handler import (fusion_handler,
                                                       in_qs_constraint,
                                                       option_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler

# TODO - Handle more NE16 types

def check_filter_options(is_ne16, input_size, output_size):
    def check_options(opts, params, **kwargs):
        if not check_option(input_size, opts.get('force_input_size')):
            return False
        if not check_option(output_size, opts.get('force_output_size')):
            return False
        if opts.get('force_ne16'):
            return is_ne16
        if not opts.get('use_ne16'):
            return not is_ne16
        return is_ne16 and can_ne16(True, params)
    return check_options


@params_type(ConvFusionParameters)
@in_qs_constraint({'dtype': set([np.int8, np.uint8])})
@out_qs_constraint({'dtype': set([np.int8, np.uint8])})
# @option_constraint(check_filter_options(False, input_size=set([8, None]), output_size=set([8, None])))
@fusion_handler
class ConvFusionMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        out_qs = kwargs['out_qs']
        return QRec.scaled(in_qs=in_qs, out_qs=out_qs)

# @params_type(ConvFusionParameters)
# @in_qs_constraint({'dtype': {np.uint8, np.uint16}})
# @out_qs_constraint({'dtype': {np.int8, np.uint8, np.int16, np.uint16}})
# @option_constraint(check_filter_options(True, input_size=set([16]), output_size=set([8, 16, None])))
# @fusion_handler
# class Ne16ConvFusionMult(MultQuantizionHandler):
#     @classmethod
#     def _quantize(cls, params, in_qs, stats, **kwargs):
#         out_qs = kwargs['out_qs']
#         return QRec.scaled(in_qs=in_qs, out_qs=out_qs)
