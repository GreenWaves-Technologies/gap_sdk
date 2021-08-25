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
from graph.types import SoftMaxParameters
from graph.types.activations import HTanHActivationParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       out_qs_constraint,
                                                       params_type, options)

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(SoftMaxParameters)
@in_qs_constraint({'dtype': set([np.int8, np.int16])})
@out_qs_constraint({'dtype': set([np.int8, np.int16])})
@options(
    {
        'name': 'softmax_out_8bits',
        'type': bool,
        'help': 'make the output scale8 8 bits',
        'default': False
    }
)
class SoftmaxMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        opts = kwargs['opts']
        if force_out_q:
            if force_out_q.forced_scale or force_out_q.forced_zero_point:
                return None
            if in_qs[0].dtype == np.int8:
                dtypes = [np.int8, np.int16]
            else:
                dtypes = [np.int16]
            if force_out_q.forced_dtype and force_out_q.dtype not in dtypes:
                return None

        in_qs = cls.force_symmetric_and_dtype(in_qs)
        if in_qs is None:
            return None
        # force the input to be POW2 scaled
        pow2_scale = np.power(2, np.ceil(np.log2(in_qs[0].scale)))
        in_q = QType(min_val=in_qs[0].min_val, max_val=in_qs[0].max_val,
                     dtype=in_qs[0].dtype, scale=pow2_scale, forced=True)
        if in_q.dtype == np.int8 and (opts.get('softmax_out_8bits', None) or
                                      (force_out_q and force_out_q.dtype == np.int8)):
            # params.at_options.softmax_out_8bits = 1
            o_q = QType(min_val=-1, max_val=1, dtype=np.int8,
                        scale=2**(-7))
        else:
            o_q = QType(min_val=-1, max_val=1, dtype=np.int16,
                        scale=2**(-15))
            if in_q.dtype == np.int16 and o_q.dtype == np.int16:
                return QRec.symmetric(in_qs=[in_q], out_qs=[o_q])

        return QRec.scaled(in_qs=[in_q], out_qs=[o_q])

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        if in_qs[0].dtype == np.uint16:
            dtype = np.int16
        else:
            dtype = np.int8
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=dtype)
                if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]

@params_type(HTanHActivationParameters)
@in_qs_constraint({'dtype': np.int8})
@out_qs_constraint({'dtype': np.int16})
class TanHMult(MultQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        opts = kwargs['opts']
        if force_out_q:
            if force_out_q.forced_scale or force_out_q.forced_zero_point:
                return None

        in_qs = cls.force_symmetric_and_dtype(in_qs, dtype=np.int8)
        if in_qs is None:
            return None
        # force the input to be POW2 scaled
        pow2_scale = np.power(2, np.ceil(np.log2(in_qs[0].scale)))
        in_q = QType(min_val=in_qs[0].min_val, max_val=in_qs[0].max_val,
                     dtype=np.int8, scale=pow2_scale, forced=True)
        o_q = QType(min_val=-1, max_val=1, dtype=np.int16,
                    scale=2**(-15))
        return QRec.scaled(in_qs=[in_q], out_qs=[o_q])
