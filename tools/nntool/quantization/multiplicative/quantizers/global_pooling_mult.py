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
from graph.types import GlobalPoolingParameters
from graph.types.global_pooling import (GlobalAveragePoolParameters,
                                        GlobalSumPoolParameters)
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type, priority)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

@options(
    {
        'name': 'hwc',
        'type': bool,
        'help': 'Use HWC kernel',
        'default': False
    }
)
@params_type(GlobalPoolingParameters)
@in_qs_constraint({'dtype': set([np.int8])})
@out_qs_constraint({'dtype': set([np.int8, np.int32])})
class GlobalPoolingMult(MultQuantizionHandler):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs.get('opts', {})
        fusion = kwargs.get('fusion', None)

        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        in_q = in_qs[0]

        if fusion:
            # Global pooling fused with activations need to have only the activation scale
            #o_q = QType(scale=in_q.scale, dtype=np.int32)
            o_q = deepcopy(in_q)
        elif force_out_q:
            if force_out_q.zero_point != in_q.zero_point:
                return None
            o_q = force_out_q
            LOG.warning('node %s output forced to range %s/%s  %s',
                        params.name, o_q.min, o_q.max, "asymmetric" if o_q.asymmetric else "symmetric")
        elif isinstance(params, GlobalAveragePoolParameters) or isinstance(params, GlobalSumPoolParameters):
            cls.check_valid_ranges(params, stats, idx=0, dirs='in')
 
            # scaling needs to be based on stats and zero point
            o_q = QType.from_min_max_sq(stats['range_out'][0]['min'],
                                        stats['range_out'][0]['max'],
                                        dtype=out_dtype,
                                        asymmetric=(stats['range_out'][0]['min'] == 0 and in_q.zero_point == -128))
        else:
            o_q = deepcopy(in_q)

        if opts.get('hwc'):
            cls.check_order(params, [['h', 'w', 'c']], [['h', 'w', 'c']])
        elif params.in_dims_hint:
            cls.check_order(params, [['c', 'h', 'w']], [['c', 'h', 'w']])
        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        scale_mul_biases_q.scale = in_q.scale/o_q.scale
        return QRec.scaled(in_qs=[in_q],
                           out_qs=[o_q],
                           scale_mul_biases_q=scale_mul_biases_q
                           )

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=np.int8,
                                      asymmetric=in_qs[idx].asymmetric)
                if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]
