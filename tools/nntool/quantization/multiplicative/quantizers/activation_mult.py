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
import math
from copy import deepcopy

import numpy as np
from graph.types.activations import (ActivationParameters,
                                     HSigmoidActivationParameters,
                                     HSwishActivationParameters,
                                     HTanHActivationParameters,
                                     LeakyActivationParameters,
                                     ReluActivationParameters,
                                     SigmoidActivationParameters,
                                     TanHActivationParameters)
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,option_constraint,
                                                       params_type, options, priority)

from ..mult_quantization_handler import MultQuantizionHandler
from quantization.quantizer_options import *

LOG = logging.getLogger('nntool.' + __name__)


@options(
    FORCE_OUTPUT_SIZE_OPTION,
)
class ActivationMultSWBase(MultQuantizionHandler):
    @classmethod
    def _quantize_sw(cls, params, in_qs, stats, in_dtype, out_dtype, out_asym, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        fusion = kwargs.get('fusion', None)
        in_q = in_qs[0]
        if fusion:
            in_dtype = np.int32
        bits = 8 if out_dtype == np.int8 or out_dtype == np.uint8 else 16

        if isinstance(params, (HSwishActivationParameters, HSigmoidActivationParameters)):
            cls.check_valid_ranges(params, stats, idx=0, dirs='in')
            # we need to be able to represent offset and upper_bound in output dtype
            # input range should match stats since swish requires the full input range
            if fusion:
                # in a fusion the output container is smaller than the input container
                # The input scale may be too small to represent offset and upper_bound
                # in the output dtype
                params_qtype = QType.from_min_max_sq(
                    0,
                    np.maximum(
                        params.upper_bound,
                        params.offset),
                    bits=bits,
                    dtype=out_dtype)
                in_q = QType.from_min_max_sq(
                    stats['range_in'][0]['min'],
                    stats['range_in'][0]['max'],
                    dtype=in_dtype)
                # if params scale is larger then we must reduce precision
                if np.all(params_qtype.scale > in_q.scale):
                    in_q.scale = params_qtype.scale
            else:
                # outside a fusion our in and out dtype is the same
                # so we just need to check that offset and upper_bound can be represented
                if in_dtype == np.uint8:
                    in_dtype = np.int8
                elif in_dtype == np.uint16:
                    in_dtype = np.int16
                if isinstance(params, HSwishActivationParameters):
                    lower = stats['range_in'][0]['min']
                    upper = np.maximum(
                        np.maximum(
                            params.upper_bound,
                            params.offset),
                        stats['range_in'][0]['max'])
                else:
                    lower = -params.offset
                    upper = params.upper_bound

                in_q = QType.from_min_max_sq(
                    lower,
                    upper,
                    dtype=in_dtype)
        elif isinstance(params, (TanHActivationParameters, SigmoidActivationParameters)):
            if in_dtype == np.int8:
                in_q = QType.from_min_max_sq(
                    -8,
                    8,
                    dtype=in_dtype,
                    forced=True)
            elif in_dtype in [np.uint8, np.uint16]:
                in_q = QType(
                    dtype=in_dtype,
                    scale=pow(2, -12),
                    zero_point=1<<(8 if in_dtype == np.uint8 else 16))
            else:
                in_q = QType(
                    dtype=in_dtype,
                    scale=pow(2, -12))
        elif isinstance(params, (HTanHActivationParameters, )):
            scale = 2 / pow(2, bits)
            in_q = QType(scale=scale, dtype=in_dtype, forced=True)
        elif isinstance(params, (LeakyActivationParameters, )):
            max_out = max(abs(stats['range_out'][0]['max']), abs(stats['range_out'][0]['min']))
            scale = (2 * max_out) / pow(2, bits)
            in_q = QType(scale=scale, dtype=in_dtype, forced=True)

        if force_out_q:
            if fusion and fusion.fusion_type in ['conv_active_pool', 'conv_active']:
                if not isinstance(params, (SigmoidActivationParameters, HTanHActivationParameters,
                                           HSwishActivationParameters, HSigmoidActivationParameters)):
                    in_q = deepcopy(force_out_q)
            o_q = deepcopy(force_out_q)
            # activation cannot move zeropoint unless it is a reduction step
            if o_q.zero_point != in_q.zero_point and in_dtype != np.int32:
                return None

        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            if isinstance(params, ReluActivationParameters):
                max_val = params.upper_bound if params.upper_bound else stats['range_out'][0]['max']
                o_q = QType.from_min_max_sq(0,
                                            max_val,
                                            dtype=out_dtype,
                                            asymmetric=(in_q.zero_point != 0) or out_dtype in [np.uint8, np.uint16])
                in_q = deepcopy(o_q)
            elif isinstance(params, TanHActivationParameters):
                o_q = QType.from_min_max_sq(
                    min_val=-1, max_val=1, dtype=out_dtype, asymmetric=out_asym)
            elif isinstance(params, SigmoidActivationParameters):
                o_q = QType.from_min_max_sq(
                    min_val=0, max_val=1, dtype=out_dtype, asymmetric=out_asym)
            elif isinstance(params, LeakyActivationParameters):
                o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                            stats['range_out'][0]['max'],
                                            dtype=out_dtype,
                                            asymmetric=out_asym)
                in_q.scale = o_q.scale
            elif isinstance(params, HSigmoidActivationParameters):
                # hsigmoid prefer to output zeropoint 0 to represent 0 - 1 range
                o_q = QType.from_min_max_sq(
                    min_val=0, max_val=1, dtype=out_dtype, asymmetric=out_asym)
            elif isinstance(params, HSwishActivationParameters):
                # hswish multiplies 0-upper bound range by input so take the upper bound from stats
                o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                            stats['range_out'][0]['max'],
                                            dtype=out_dtype,
                                            asymmetric=out_asym)
            else:
                o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                            stats['range_out'][0]['max'],
                                            dtype=out_dtype,
                                            asymmetric=out_asym)

        qrec = QRec.scaled(in_qs=[in_q], out_qs=[o_q])
        qrec = cls.compute_cache(params, qrec, stats)
        return qrec

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        return [np.int8]

    @classmethod
    def compute_cache(cls, params, qrec, stats):
        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        qrec.cache['scale_mul_biases_q'] = scale_mul_biases_q
        if isinstance(params, (ReluActivationParameters)):
            if params.upper_bound:
                qrec.cache['upper_bound'] = qrec.in_qs[0].quantize(
                    params.upper_bound).astype(qrec.out_qs[0].dtype)
            if params.lower_bound:
                qrec.cache['lower_bound'] = qrec.in_qs[0].quantize(
                    params.lower_bound).astype(qrec.out_qs[0].dtype)
            scale_mul_biases_q.scale = (
                qrec.in_qs[0].scale/qrec.out_qs[0].scale)
        elif isinstance(params, (SigmoidActivationParameters, TanHActivationParameters)):
            scale_mul_biases_q.scale = math.pow(2, -15) / qrec.out_qs[0].scale
            qrec.cache["zero_point"] = qrec.out_qs[0].zero_point.astype(
                qrec.out_qs[0].dtype)
        elif isinstance(params, (LeakyActivationParameters)):
            scale_mul_biases_q.scale = (
                qrec.in_qs[0].scale/qrec.out_qs[0].scale)
            qrec.cache['leak_factor'] = np.int8(
                params.leak_factor*math.pow(2, 7) + 0.5)
            qrec.cache['zero_point'] = qrec.out_qs[0].zero_point.astype(qrec.out_qs[0].dtype)
        elif isinstance(params, (HSwishActivationParameters, HSigmoidActivationParameters)):
            scale = (qrec.in_qs[0].scale * params.mult)/qrec.out_qs[0].scale
            if isinstance(params, HSwishActivationParameters):
                # HSwish multiplies HSigmoid by input
                scale *= qrec.in_qs[0].scale
            scale_mul_biases_q.scale = scale
            qrec.cache['offset'] = qrec.in_qs[0].quantize(
                params.offset).astype(qrec.out_qs[0].dtype)
            qrec.cache['zero_point'] = qrec.out_qs[0].zero_point
            qrec.cache['upper_bound'] = qrec.in_qs[0].quantize(
                params.upper_bound).astype(qrec.out_qs[0].dtype)
        else:
            scale_mul_biases_q.scale = (
                qrec.in_qs[0].scale/qrec.out_qs[0].scale)
        return qrec


@params_type(ActivationParameters)
@in_qs_constraint({'dtype': {np.int8, np.int16, np.int32}})
@out_qs_constraint({'dtype': np.int8})
@option_constraint(force_output_size={8, None})
class ActivationMultSW_I_I8(ActivationMultSWBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, in_qs[0].dtype, np.int8, out_asym=False, **kwargs)


@params_type(HSwishActivationParameters, HSigmoidActivationParameters)
@in_qs_constraint({'dtype': {np.int8, np.int16, np.int32}})
@out_qs_constraint({'dtype': np.uint8})
@option_constraint(force_output_size={8, None})
@priority(2)
class ActivationMultSW_HSwish_I_U8(ActivationMultSWBase):
    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        dtype = in_qs and in_qs[0] and in_qs[0].dtype
        if dtype == np.uint16:
            dtype = np.int16
        else:
            dtype = np.int8
        return [QType.from_min_max_sq(
            stats['range_in'][0]['min'],
            stats['range_in'][0]['max'],
            dtype=dtype)]

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, in_qs[0].dtype, np.uint8, out_asym=False, **kwargs)


@params_type(HSwishActivationParameters, HSigmoidActivationParameters)
@in_qs_constraint({'dtype': {np.int8, np.int16, np.int32}})
@out_qs_constraint({'dtype': np.uint16})
@option_constraint(force_output_size=16)
class ActivationMultSW_HSwish_I_U16(ActivationMultSWBase):
    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        dtype = in_qs and in_qs[0] and in_qs[0].dtype
        if dtype == np.uint16:
            dtype = np.int16
        else:
            dtype = np.int8
        return [QType.from_min_max_sq(
            stats['range_in'][0]['min'],
            stats['range_in'][0]['max'],
            dtype=dtype)]

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, in_qs[0].dtype, np.uint16, out_asym=False, **kwargs)


@params_type(ActivationParameters)
@in_qs_constraint({'dtype': {np.int8, np.int16, np.int32}})
@out_qs_constraint({'dtype': np.int16})
@option_constraint(force_output_size=16)
class ActivationMultSW_I_I16(ActivationMultSWBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, in_qs[0].dtype, np.int16, out_asym=False, **kwargs)


@params_type(LeakyActivationParameters, TanHActivationParameters, SigmoidActivationParameters, ReluActivationParameters)
@in_qs_constraint({'dtype': {np.uint8, np.int32}})
@out_qs_constraint({'dtype': np.uint8})
@option_constraint(force_output_size={8, None})
class ActivationMultSW_U_U8(ActivationMultSWBase):
    # This handler should be called only for NE16 for the moment --> out is asym
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, in_qs[0].dtype, np.uint8, out_asym=True, **kwargs)


@params_type(LeakyActivationParameters, TanHActivationParameters, SigmoidActivationParameters, ReluActivationParameters)
@in_qs_constraint({'dtype': {np.uint16, np.int32}})
@out_qs_constraint({'dtype': np.uint16})
@option_constraint(force_output_size=16)
class ActivationMultSW_U_U16(ActivationMultSWBase):
    # This handler should be called only for NE16 for the moment --> out is asym
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, in_qs[0].dtype, np.uint16, out_asym=True, **kwargs)
