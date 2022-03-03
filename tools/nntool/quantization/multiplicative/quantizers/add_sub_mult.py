# Copyright (C) 2020, 2021, 2022  GreenWaves Technologies, SAS

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
from graph.types import MatrixAddParameters, MatrixSubParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type)

from ..mult_quantization_handler import MultQuantizionHandler


class AddSubMultBase(MultQuantizionHandler):
    @classmethod
    def _quantize_sw(cls, params, in_qs, stats, inout_dtype, asym=False, **kwargs):
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        # NOTE: The autotiler kernel scales and clips after the operation and before the
        # activation so there is no change if this is in a fusion or not
        scaled_idx = params.force_quantized_index if isinstance(
            params, MatrixAddParameters) else None
        if not asym:
            in_qs = cls.force_symmetric_and_dtype(in_qs)

        if in_qs is None:
            return None

        if force_out_q:
            o_q = deepcopy(force_out_q)
            if (o_q.asymmetric and not asym) or o_q.dtype != inout_dtype:
                return None
            # important to set ne16 here so the o_q matches the force_out_q since
            # this attribute is not copied by deepcopy
            if force_out_q.attr.ne16:
                o_q.attr.ne16 = True
        else:
            cls.check_valid_ranges(params, stats, idx=0, dirs='out')
            o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                        stats['range_out'][0]['max'],
                                        dtype=inout_dtype,
                                        asymmetric=asym,
                                        dont_copy_attr=['ne16'],
                                        ne16=asym)
        if asym:
            o_q.set_forced(flags=['dtype'])
            in_qs = [in_q.set_forced(flags=['dtype']) for in_q in in_qs]
        else:
            o_q.set_forced(flags=['dtype', 'zero_point'])
            in_qs = [in_q.set_forced(flags=['dtype', 'zero_point'])
                     for in_q in in_qs]
        qrec = QRec.scaled(in_qs=in_qs, out_qs=[o_q], ne16=asym)

        if scaled_idx is None:
            scaled_idx = (1 if qrec.in_qs[1].scale >
                          qrec.in_qs[0].scale else 0)
        qrec.cache['scaled_idx'] = scaled_idx

        not_scaled_idx = 0 if scaled_idx else 1
        scale_mul_biases_q = qrec.cache['scale_mul_biases_q'] = MultMulBiasScaleQType(
            dtype=np.uint8)
        scale_mul_biases_q.scale = qrec.in_qs[not_scaled_idx].scale / \
            qrec.out_qs[0].scale

        scale_in_mul_biases_q = qrec.cache['scale_in_mul_biases_q'] = MultMulBiasScaleQType(
            dtype=np.uint8)
        scale_in_mul_biases_q.scale = qrec.in_qs[scaled_idx].scale / \
            qrec.in_qs[not_scaled_idx].scale

        if qrec.in_qs[0].zero_point or qrec.in_qs[1].zero_point or qrec.out_qs[0].zero_point:
            # (C - Zc)*Sc = (A - Za)*Sa + (B - Zb)*Sb =
            # C = Sa/Sc*(A + B*Sb/Sa - Za - Zb*Sb/Sa) + Zc =
            #   = Sa/Sc*(A + B*Sb/Sa) + (Zc - Sa/Sc*(Za + Zb*Sb/Sa))
            #                           |---------- bias ----------|
            add_bias = (
                qrec.out_qs[0].zero_point - qrec.cache['scale_mul_biases_q'].scale *
                (qrec.in_qs[not_scaled_idx].zero_point +
                 scale_in_mul_biases_q.scale * qrec.in_qs[scaled_idx].zero_point)
            )
        else:
            add_bias = 0

        qrec.cache['add_bias_offset'] = np.round(add_bias).astype(np.int16)
        return qrec


@params_type(MatrixAddParameters, MatrixSubParameters)
@in_qs_constraint({'dtype': np.int8},
                  {'dtype': np.int8})
@out_qs_constraint({'dtype': np.int8})
class AddSubMult8x8(AddSubMultBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int8, **kwargs)


@params_type(MatrixAddParameters, MatrixSubParameters)
@in_qs_constraint({'dtype': np.int16},
                  {'dtype': np.int16})
@out_qs_constraint({'dtype': np.int16})
class AddSubMult16x16(AddSubMultBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.int16, **kwargs)


@params_type(MatrixAddParameters, MatrixSubParameters)
@in_qs_constraint({'dtype': np.uint8},
                  {'dtype': np.uint8})
@out_qs_constraint({'dtype': np.uint8})
class AddSubMultu8xu8(AddSubMultBase):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize_sw(params, in_qs, stats, np.uint8, asym=True, **kwargs)
