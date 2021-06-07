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
from copy import deepcopy

import numpy as np
from graph.types.activations import (ActivationParameters,
                                     HSigmoidActivationParameters,
                                     HSwishActivationParameters,
                                     HTanHActivationParameters,
                                     ReluActivationParameters,
                                     SigmoidActivationParameters,
                                     TanHActivationParameters)
from quantization.multiplicative.mulbias import compute_in_out_scale
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.symmetric.kernels.activations import \
    SigmoidScaledSymmetricMult
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type, priority)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

@params_type(ActivationParameters)
@in_qs_constraint({'dtype': set([np.int8, np.int32])})
@out_qs_constraint({'dtype': np.int8})
class ActivationMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        fusion = kwargs.get('fusion', None)
        in_q = in_qs[0]
        if not fusion and in_q.dtype == np.int32:
            return None

        if isinstance(params, (HSwishActivationParameters, HSigmoidActivationParameters)):
            max_val = in_q.scale * pow(2, in_q.bits - 1)
            if max_val < 6:
                in_q = QType.from_min_max_sq(-6, 6, dtype=in_q.dtype, forced=True)
        elif isinstance(params, SigmoidActivationParameters):
            in_q = QType.from_min_max_sq(-8, 8, dtype=in_q.dtype, forced=True)

        if force_out_q:
            if force_out_q.signed != in_q.signed:
                return None
            if fusion and fusion.fusion_type in ['conv_active_pool', 'conv_active']:
                if not isinstance(params, (SigmoidActivationParameters, HTanHActivationParameters,
                                           HSwishActivationParameters, HSigmoidActivationParameters)):
                    in_q = deepcopy(force_out_q)
            o_q = deepcopy(force_out_q)
            # activation cannot move zeropoint unless it is a reduction step
            if o_q.zero_point != in_q.zero_point and in_q.dtype != np.int32:
                return None
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            zero_point = in_q.zero_point if in_q.zero_point != 0 else None
            o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                        stats['range_out'][0]['max'],
                                        dtype=in_q.dtype,
                                        zero_point=zero_point)

        qrec = QRec.scaled(in_qs=[in_q], out_qs=[o_q])
        if isinstance(params, (SigmoidScaledSymmetricMult, TanHActivationParameters)):
            compute_in_out_scale(qrec, extra_scale=QType.Pow2(bits=32, q=7, signed=True).scale/qrec.in_qs[0].scale)
        elif isinstance(params, HSwishActivationParameters):
            compute_in_out_scale(qrec, extra_scale=qrec.in_qs[0].scale * 1/6)
        else:
            compute_in_out_scale(qrec)
        return qrec


    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        return [np.int8]

@params_type(ReluActivationParameters)
@in_qs_constraint({'dtype': np.uint8})
@out_qs_constraint({'dtype': np.uint8})
@priority(2)
class ActivationMultNe16(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = in_qs[0]

        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        if force_out_q:
            # if the output has been forced then propagate it
            in_q = force_out_q
        else:
            upper = params.upper_bound if params.upper_bound is not None else stats['range_out'][0]['max']
            in_q = QType.from_min_max_sq(0, upper, dtype=np.uint8, asymmetric=True)

        return QRec.scaled(in_qs=[in_q], out_qs=[in_q], ne16=True)
