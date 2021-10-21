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
from pytest import param
from graph.types.activations import (ActivationParameters,
                                     HSigmoidActivationParameters,
                                     HSwishActivationParameters,
                                     HTanHActivationParameters, LeakyActivationParameters,
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
            max_val = in_q.scale * pow(2, in_q.bits - 1)
            if max_val < 6:
                in_q = QType.from_min_max_sq(-6, 6,
                                             dtype=in_dtype, forced=True)
        elif isinstance(params, SigmoidActivationParameters):
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
        qrec = cls.compute_scales(params, qrec)
        return qrec

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        return [np.int8]

    @classmethod
    def compute_scales(cls, params, qrec):
        if isinstance(params, (SigmoidScaledSymmetricMult, TanHActivationParameters)):
            compute_in_out_scale(qrec, extra_scale=QType.Pow2(
                bits=32, q=7, signed=True).scale/qrec.in_qs[0].scale)
        elif isinstance(params, HSwishActivationParameters):
            compute_in_out_scale(qrec, extra_scale=qrec.in_qs[0].scale * 1/6)
        else:
            compute_in_out_scale(qrec)
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
        if params.lower_bound != 0:
            raise NotImplementedError(
                'relu with non zero lower bound is not implemented for NE16 quantizer')
        cls.check_valid_ranges(params, stats, idx=0, dirs='out')
        if force_out_q:
            # since the relu is done by setting 0 zero point and scaling to the upper bound
            # we cannot be forced to something that does not meet this requirement
            if not force_out_q.zero_point_asymmetric_zero:
                return None
            if params.upper_bound is not None and not np.isclose(force_out_q.max, params.upper_bound, atol=0.01):
                return None
            # if the output has been forced then propagate it
            in_q = force_out_q
        else:
            upper = params.upper_bound if params.upper_bound is not None else stats[
                'range_out'][0]['max']
            in_q = QType.from_min_max_sq(
                0, upper, dtype=in_q.dtype, asymmetric=True,
                ne16=True, dont_copy_attr=['ne16'])
        o_q = deepcopy(in_q)
        o_q.set_forced()
        qrec = QRec.scaled(in_qs=[in_q], out_qs=[o_q], ne16=True)
        compute_in_out_scale(qrec)
        return qrec
