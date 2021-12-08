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
from graph.types import PoolingParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantizer_options import FORCE_NE16_OPTION, HWC_OPTION, USE_NE16_OPTION
from quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type, priority)

from ..mult_quantization_handler import MultQuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

@options(
    HWC_OPTION
)
@params_type(PoolingParameters)
@in_qs_constraint({'dtype': {np.int16, np.int8}})
@out_qs_constraint({'dtype': {np.int16, np.int8}})
class PoolingMult(MultQuantizionHandler):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs['opts']

        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        G = kwargs['G']
        in_q = in_qs[0]

        if (in_q.asymmetric and isinstance(params, PoolingParameters) and params.padding.has_padding):
            in_qs = cls.force_symmetric(in_qs)
            if in_qs is None:
                return None
            in_q = in_qs[0]

        cls.check_valid_ranges(params, stats, idx=0, dirs='in')
        min_val = stats['range_in'][0]['min']
        max_val = stats['range_in'][0]['max']

        if force_out_q:
            if force_out_q.asymmetric and not opts.get('allow_asymmetric'):
                LOG.warning('%s could be asymmetricaly quantized but allow_asymmetric option not selected', params.name)
                return None
            if force_out_q.dtype != in_q.dtype:
                return None
            o_q = force_out_q
            in_q = deepcopy(force_out_q)
            if force_out_q.dtype != in_q.dtype or force_out_q.zero_point != in_q.zero_point:
                if in_q.forced and force_out_q.zero_point != 0:
                    return None
            LOG.warning('node %s output forced to range %s/%s  %s - actual range %s/%s',
                        params.name, o_q.min, o_q.max, "asymmetric" if o_q.asymmetric else "symmetric",
                        min_val, max_val)
        else:
            o_q = deepcopy(in_q)

        if opts['hwc']:
            cls.check_order(params, [['h', 'w', 'c']], [['h', 'w', 'c']])
        else:
            cls.check_order(params, [['c', 'h', 'w']], [['c', 'h', 'w']])
        return QRec.scaled(in_qs=[in_q],
                           out_qs=[o_q])

    @classmethod
    def can_handle_asymmetric_input(cls, params, **kwargs):
        return not isinstance(params, PoolingParameters) or not params.padding.has_padding

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=np.int8,
                                      asymmetric=cls.can_handle_asymmetric_input(params, **kwargs) and in_qs[idx].asymmetric)
                if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]


@options(
    USE_NE16_OPTION,
    FORCE_NE16_OPTION
)
@params_type(PoolingParameters)
@in_qs_constraint({'dtype': set([np.uint8, np.int16, np.uint16])})
@out_qs_constraint({'dtype': set([np.uint8, np.int16, np.uint16])})
@priority(2)
class NE16PoolingMult(MultQuantizionHandler):

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        # copy in_qs because we may modify it
        in_qs = in_qs.copy()
        opts = kwargs['opts']
        force_out_qs, out_dtype = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        G = kwargs['G']
        in_q = in_qs[0]

        cls.check_valid_ranges(params, stats, idx=0, dirs='in')
        min_val = stats['range_in'][0]['min']
        max_val = stats['range_in'][0]['max']

        if force_out_q:
            # get rid of the force out if ne16 is not selected.
            if not (opts.get('use_ne16') or opts.get('force_ne16')):
                LOG.info(
                    '%s ne16 max pool possible but ne16 mode not enabled', params.name)
                return None
            o_q = force_out_q
            if in_q.forced and in_q.zero_point != o_q.zero_point:
                return None
            in_q = deepcopy(o_q)

            LOG.warning('node %s output forced to range %s/%s - actual range %s/%s %s',
                        params.name, o_q.min, o_q.max, min_val, max_val,
                        "asymmetric" if o_q.asymmetric else "symmetric")
        else:
            o_q = deepcopy(in_q)
        o_q.attr.ne16 = True
        cls.check_order(params, [['h', 'w', 'c']], [['h', 'w', 'c']])
        return QRec.scaled(in_qs=[in_q],
                           out_qs=[o_q],
                           ne16=True)

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return [QType.from_min_max_sq(stats['range_in'][idx]['min'],
                                      stats['range_in'][idx]['max'],
                                      dtype=np.uint8,
                                      asymmetric=cls.can_handle_asymmetric_input(params, **kwargs) and in_qs[idx].asymmetric)
                if dim is not None else None
                for idx, dim in enumerate(params.in_dims)]

    @classmethod
    def can_handle_asymmetric_input(cls, params, **kwargs):
        return not isinstance(params, PoolingParameters) or not params.padding.has_padding
