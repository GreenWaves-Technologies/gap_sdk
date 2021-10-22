# Copyright (C) 2021, 2020  GreenWaves Technologies, SAS

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
from utils.subclasses import get_all_subclasses

from quantization.qtype_constraint import ConstraintBase, MatchAll

LOG = logging.getLogger('nntool.' + __name__)

def min_or_max_none(v):
    return v['min'] is None or v['max'] is None

def dimstr(dims):
    return ", ".join("".join(dimname for dimname in dim) for dim in dims)

class QuantizionHandler(ConstraintBase):
    """ This class is base quantization handler class.

    All quantization handlers MUST put decorator @params_type to register corresponding params handled.
    """
    SCHEME = None
    PARAMS_TYPE = None
    DOCUMENTATION = ''
    OPTIONS = {}
    CAN_DEQUANTIZE = False
    NEEDS_STATS = True
    FUSION_HANDLER = False  # Indicates that the subgraph inside a fusion should be traversed
    PRIORITY = 1

    @classmethod
    def description(cls):
        return f'{cls.__name__}({cls.SCHEME}, {cls.constraints_repr()})'

    @classmethod
    def check_cls(cls):
        if not cls.PARAMS_TYPE:
            LOG.warning(
                "%s doesn't have PARAMS_TYPE. "
                "Please use Handler.params_type decorator to register PARAMS_TYPE.",
                cls.__name__)
        if not cls.SCHEME:
            LOG.warning(
                "%s doesn't have SCHEME. "
                "Please use Handler.params_type decorator to register SCHEME.",
                cls.__name__)

    @classmethod
    def check_valid_ranges(cls, params, stats, idx=None, dirs=None):
        if stats is None:
            raise ValueError(
                f'no valid range information for node {params.name}. Graph cannot be quantized.')
        if dirs is None:
            dirs = ('in', 'out')
        elif isinstance(dirs, str):
            dirs = (dirs,)
        for direction in dirs:
            if not f'range_{direction}' in stats:
                raise ValueError(
                    f'no valid range information for node {params.name} range_{direction}. Graph cannot be quantized.')
            range_info = stats[f'range_{direction}']
            if idx is not None:
                if len(range_info) <= idx or range_info[idx] is None or min_or_max_none(range_info[idx]):
                    raise ValueError(
                        f'no valid range information for node {params.name} range_{direction}[{idx}]. Graph cannot be quantized.')
            else:
                for i, item in enumerate(range_info):
                    if item is None or min_or_max_none(item):
                        raise ValueError(
                            f'no valid range information for node {params.name} range_{direction}[{i}]. Graph cannot be quantized.')

    @classmethod
    def check_order(cls, params, in_order, out_order):
        if params.ker_in_order and params.ker_in_order != in_order:
            LOG.warning('indicating change of %s input from %s to %s order - rerun adjust command',
            params.name,
            dimstr(params.ker_in_order),
            dimstr(in_order))
            params.ker_in_order = in_order
        if params.ker_out_order and params.ker_out_order != out_order:
            LOG.warning('indicating change of %s output from %s to %s order - rerun adjust command',
            params.name,
            dimstr(params.ker_out_order),
            dimstr(out_order))
            params.ker_out_order = out_order

    @classmethod
    def get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        return cls._get_in_qs_from_stats(params, stats, in_qs, **kwargs)

    @classmethod
    def _get_in_qs_from_stats(cls, params, stats, in_qs, **kwargs):
        raise NotImplementedError("no _get_in_qs_from_stats method implemented")

    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        raise NotImplementedError("no _quantize method implemented")

    @classmethod
    def get_options(cls, params, opts):
        popts = cls.get_default_options()
        popts.update(opts.get(params, {}))
        return popts

    @classmethod
    def quantize(cls, params, in_qs, stats, **kwargs):
        return cls._quantize(params, in_qs, stats, **kwargs)

    @classmethod
    def _dequantize(cls, params, qrec):
        raise ValueError("this handler cannot dequantize")

    @classmethod
    def dequantize(cls, params, qrec):
        cls._dequantize(params, qrec)

    @classmethod
    def get_default_options(cls):
        return {name: opt['default'] for name, opt in cls.OPTIONS.items()}

    @staticmethod
    def needs_stats(yesno):
        return QuantizionHandler.property_register("NEEDS_STATS", yesno)

    @staticmethod
    def params_type(*args):
        return QuantizionHandler.property_register("PARAMS_TYPE", args)

    @staticmethod
    def documentation(doc):
        return QuantizionHandler.property_register("DOCUMENTATION", doc)

    @staticmethod
    def priority(num):
        return QuantizionHandler.property_register("PRIORITY", num)

    @staticmethod
    def scheme(doc):
        return QuantizionHandler.property_register("SCHEME", doc)

    @staticmethod
    def fusion_handler(deco_cls):
        setattr(deco_cls, "FUSION_HANDLER", True)
        return deco_cls

    @staticmethod
    def options(*args):
        def deco(cls):
            # copy the closest base class options so that we create
            # a new class variable on this class
            setattr(cls, "OPTIONS", deepcopy(getattr(cls, "OPTIONS")))
            # Now add / overide options
            cls_opts = getattr(cls, "OPTIONS")
            cls_opts.update({opt['name']: opt for opt in args})
            cls_opts_hash = object.__hash__(cls_opts)
            # since object classes can be intialized in an arbitrary order
            # copy to all subclasses that have already set options
            for subcls in get_all_subclasses(cls):
                sub_cls_opts = getattr(subcls, "OPTIONS")
                if object.__hash__(sub_cls_opts) != cls_opts_hash:
                    sub_cls_opts.update({opt['name']: opt for opt in args})


            return cls
        return deco

    @staticmethod
    def can_dequantize(yes_no):
        return QuantizionHandler.property_register("CAN_DEQUANTIZE", yes_no)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

    @classmethod
    def get_prefered_input_dtypes(cls, params, **kwargs):
        input_constraints = cls.INPUT_CONSTRAINTS
        current_constraint = None
        if input_constraints:
            constraints = []
            input_constraint = None
            for idx, _ in enumerate(params.in_dims):
                if current_constraint is None:
                    if len(input_constraints) > idx:
                        input_constraint = input_constraints[idx]
                        if isinstance(input_constraint, MatchAll):
                            current_constraint = input_constraint = input_constraint.constraint
                    else:
                        input_constraint = None
                else:
                    input_constraint = current_constraint
                if input_constraint and 'dtype' in input_constraint:
                    dtype_constraint = input_constraint['dtype']
                    if isinstance(dtype_constraint, set):
                        constraints.append(next(dtype_constraint.__iter__()))
                    else:
                        constraints.append(dtype_constraint)
                else:
                    constraints.append(None)
        else:
            constraints = [None] * len(params.in_dims)
        return constraints

#pylint: disable=invalid-name
params_type = QuantizionHandler.params_type
documentation = QuantizionHandler.documentation
options = QuantizionHandler.options
option_constraint = QuantizionHandler.option_constraint
can_dequantize = QuantizionHandler.can_dequantize
scheme = QuantizionHandler.scheme
in_qs_constraint = QuantizionHandler.in_qs_constraint
out_qs_constraint = QuantizionHandler.out_qs_constraint
fusion_handler = QuantizionHandler.fusion_handler
priority = QuantizionHandler.priority
needs_stats = QuantizionHandler.needs_stats
