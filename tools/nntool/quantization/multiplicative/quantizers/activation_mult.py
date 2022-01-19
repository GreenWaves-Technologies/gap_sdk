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
                                                       out_qs_constraint,
                                                       params_type, priority)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


class ActivationMultSWBase(MultQuantizionHandler):
    @classmethod
    def _quantize_sw(cls, params, in_qs, stats, in_dtype, out_dtype, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        fusion = kwargs.get('fusion', None)
        in_q = in_qs[0]
        if not fusion and in_dtype == np.int32:
            return None

        if isinstance(params, (HSwishActivationParameters, HSigmoidActivationParameters)):
            if in_q.max < params.upper_bound:
                # TODO - could do something clever for asymmetric here
                in_q = QType.from_min_max_sq(-params.upper_bound, params.upper_bound,
                                             dtype=in_dtype, forced=True)
        elif isinstance(params, (TanHActivationParameters, SigmoidActivationParameters)):
            in_q = QType.from_min_max_sq(-8, 8, dtype=in_dtype, forced=True)

        if force_out_q:
            if force_out_q.signed != in_q.signed:
                return None
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
            if (isinstance(params, ReluActivationParameters) and params.lower_bound == 0 and
                    in_q.dtype == np.int8):
                max_val = params.upper_bound if params.upper_bound else stats['range_out'][0]['max']
                o_q = QType.from_min_max_sq(0,
                                            max_val,
                                            dtype=out_dtype,
                                            asymmetric=(in_q.zero_point != 0))
                in_q = deepcopy(o_q)
            elif isinstance(params, (TanHActivationParameters, SigmoidActivationParameters)):
                if out_dtype == np.int8:
                    o_q = QType(q=7, dtype=np.int8)
                elif out_dtype == np.int16:
                    o_q = QType(q=15, dtype=np.int16)
                else:
                    raise NotImplementedError(
                        'int8 and int16 are implemented as output only')
            elif isinstance(params, LeakyActivationParameters):
                o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                            stats['range_out'][0]['max'],
                                            dtype=out_dtype)
                # force the preceeding filter to clip the negative range
                in_q = deepcopy(o_q)
            else:
                o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                            stats['range_out'][0]['max'],
                                            dtype=out_dtype)

        qrec = QRec.scaled(in_qs=[in_q], out_qs=[o_q])
        qrec = cls.compute_cache(params, qrec)
        return qrec

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        return [np.int8]

    @classmethod
    def compute_cache(cls, params, qrec):
        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        qrec.cache['scale_mul_biases_q'] = scale_mul_biases_q

        if isinstance(params, (SigmoidActivationParameters, TanHActivationParameters)):
            scale_mul_biases_q.scale = (math.pow(
                2, -7 if qrec.out_qs[0].dtype == np.int8 else -15)/qrec.out_qs[0].scale)
        elif isinstance(params, (LeakyActivationParameters)):
            scale_mul_biases_q.scale = (
                qrec.in_qs[0].scale/qrec.out_qs[0].scale)
            qrec.cache['leak_factor'] = np.int8(
                params.leak_factor*math.pow(2, 7) + 0.5)
        elif isinstance(params, HSwishActivationParameters):
            scale_mul_biases_q.scale = (
                (qrec.in_qs[0].scale*qrec.in_qs[0].scale * params.mult)/qrec.out_qs[0].scale)
            qrec.cache['offset'] = qrec.in_qs[0].quantize(params.offset)
            qrec.cache['mult'] = np.int8(1)
            qrec.cache['upper_bound'] = qrec.in_qs[0].quantize(
                params.upper_bound)
        elif isinstance(params, HSigmoidActivationParameters):
            scale_mul_biases_q.scale = (
                (qrec.in_qs[0].scale*params.mult)/qrec.out_qs[0].scale)
            qrec.cache['offset'] = qrec.in_qs[0].quantize(params.offset)
            qrec.cache['mult'] = np.int8(1)
            qrec.cache['upper_bound'] = qrec.in_qs[0].quantize(
                params.upper_bound)
        else:
            scale_mul_biases_q.scale = (
                qrec.in_qs[0].scale/qrec.out_qs[0].scale)
        return qrec


@params_type(ActivationParameters)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
class ActivationMultSW8x8(ActivationMultSWBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int8, np.int8, **kwargs)


@params_type(ActivationParameters)
@in_qs_constraint({'dtype': np.int32})
@out_qs_constraint({'dtype': np.int8})
@priority(2)
class ActivationMultSW32x8(ActivationMultSWBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int32, np.int8, **kwargs)


@params_type(ActivationParameters)
@in_qs_constraint({'dtype': np.int16})
@out_qs_constraint({'dtype': np.int16})
@priority(2)
class ActivationMultSW16x16(ActivationMultSWBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int16, np.int16, **kwargs)


@params_type(ActivationParameters)
@in_qs_constraint({'dtype': np.int32})
@out_qs_constraint({'dtype': np.int16})
@priority(2)
class ActivationMultSW32x16(ActivationMultSWBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int32, np.int16, **kwargs)


@params_type(ReluActivationParameters)
@in_qs_constraint({'dtype': {np.uint8, np.uint16, np.int8, np.int16}, 'attr': {'ne16': True}})
@out_qs_constraint({'dtype': {np.uint8, np.uint16, np.int8, np.int16}})
@priority(3)
class ActivationMultNe16(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = in_qs[0]

        if force_out_q:
            # TODO - should uint8 be accepted here if it does not scale to the relu?
            o_q = deepcopy(force_out_q)
            in_q = deepcopy(o_q)
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            upper = (stats['range_out'][0]['max'] if params.upper_bound is None
                     else params.upper_bound)
            in_q = QType.from_min_max_sq(
                params.lower_bound, upper, dtype=in_q.dtype, asymmetric=True,
                ne16=True, dont_copy_attr=['ne16'])
            o_q = deepcopy(in_q)

        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        scale_mul_biases_q.scale = in_q.scale/o_q.scale
        qrec = QRec.scaled(in_qs=[in_q], out_qs=[o_q],
                           ne16=True, scale_mul_biases_q=scale_mul_biases_q)
        return qrec


@params_type(HSigmoidActivationParameters, HSwishActivationParameters)
@in_qs_constraint({'dtype': {np.uint16, np.int32}}) #, 'attr': {'ne16': True}})
@out_qs_constraint({'dtype': {np.uint16}})
@priority(3)
class HSigmoidSwishActivationMultNe16USQ16(MultQuantizionHandler):
    DEFAULT_DTYPE = np.uint16

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]

        # input ranged to values that count and upper bound must be representable
        # there is an assumption here that params.offset is always less than params.upper_bound
        assert params.offset <= params.upper_bound
        in_q = in_qs[0]
        max_repr = np.maximum(in_q.max, params.upper_bound)
        in_q = QType.from_min_max_sq(-max_repr, max_repr, dtype=np.int32, forced=True)

        if force_out_q:
            # sigmoid and hswish has to output asymmetric with zero point at zero
            if not force_out_q.zero_point_asymmetric_zero:
                return None
            # if the output has been forced then propagate it
            out_q = deepcopy(force_out_q)
        elif isinstance(params, HSigmoidActivationParameters):
            # hsigmoid prefer to output Q16 zeropoint 0 to represent 0 - 1 range
            out_q = QType(dtype=np.uint16, scale=pow(2, -16), zero_point=0,
                          ne16=True, dont_copy_attr=['ne16'])
        else:
            # hswish multiplies 0-upper bound range by input so take the upper
            # bound from stats
            upper = stats['range_out'][0]['max']
            max_repr = np.maximum(in_q.max, upper)
            in_q = QType.from_min_max_sq(0, upper, dtype=np.int32,
                                         ne16=True, dont_copy_attr=['ne16'])

        qrec = QRec.scaled(in_qs=[in_q], out_qs=[out_q], ne16=True)
        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)

        if isinstance(params, HSigmoidActivationParameters):
            scale_mul_biases_q.scale = (
                (qrec.in_qs[0].scale*params.mult)/qrec.out_qs[0].scale)
        elif isinstance(params, HSwishActivationParameters):
            scale_mul_biases_q.scale = (
                (qrec.in_qs[0].scale*qrec.in_qs[0].scale*params.mult)/qrec.out_qs[0].scale)
        else:
            raise ValueError(f"Unexpacted params type {params}")

        qrec.cache['offset'] = qrec.in_qs[0].quantize(params.offset)
        qrec.cache['mult'] = np.int16(1)
        qrec.cache['upper_bound'] = qrec.in_qs[0].quantize(
            params.upper_bound)

        qrec.cache['scale_mul_biases_q'] = scale_mul_biases_q
        return qrec


@params_type(SigmoidActivationParameters)
@in_qs_constraint({'dtype': {np.uint16, np.int32}, 'attr': {'ne16': True}})
@out_qs_constraint({'dtype': {np.uint16}})
@priority(3)
class SigmoidActivationMultNe16USQ16(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = QType(dtype=np.int32, scale=pow(2, -12))
        if force_out_q:
            # sigmoid has to output asymmetric with zero point at zero
            if not force_out_q.zero_point_asymmetric_zero:
                return None
            # if the output has been forced then propagate it
            out_q = deepcopy(force_out_q)
        else:
            # 0 to 1 range so prefer Q16 zeropoint 0
            out_q = QType(dtype=np.uint16, scale=pow(2, -16), zero_point=0,
                          ne16=True, dont_copy_attr=['ne16'], min_val=0, max_val=1)

        scale_mul_biases_q = MultMulBiasScaleQType(
            dtype=np.uint8, scale=pow(2, -16)/out_q.scale)
        qrec = QRec.scaled(in_qs=[in_q], out_qs=[out_q],
                           ne16=True, scale_mul_biases_q=scale_mul_biases_q)
        return qrec


@params_type(TanHActivationParameters)
@in_qs_constraint({'dtype': {np.uint16, np.int32}, 'attr': {'ne16': True}})
@out_qs_constraint({'dtype': {np.uint16}})
@priority(3)
class TanHActivationMultNe16USQ16(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = QType(dtype=np.int32, scale=pow(2, -12))
        if force_out_q:
            # tanh has to output symmetric with zero point at 32768
            if not np.all(np.atleast_1d(force_out_q.zero_point) == 32768):
                return None
            # if the output has been forced then propagate it
            out_q = deepcopy(force_out_q)
        else:
            # -1 to 1 range so prefer Q15
            out_q = QType(dtype=np.uint16, scale=pow(2, -15), zero_point=np.array([32768], dtype=np.uint16),
                          ne16=True, dont_copy_attr=['ne16'], min_val=-1, max_val=1)

        scale_mul_biases_q = MultMulBiasScaleQType(
            dtype=np.uint8, scale=pow(2, -15)/out_q.scale)
        qrec = QRec.scaled(in_qs=[in_q], out_qs=[out_q],
                           ne16=True, scale_mul_biases_q=scale_mul_biases_q)
        return qrec
