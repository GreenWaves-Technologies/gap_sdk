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

import numpy as np
from cmd2.argparse_custom import Cmd2ArgumentParser
from nntool.graph.types import ConstantInputNode, ReluNode
from nntool.graph.types.base import NNNodeBase
from iteration_utilities import duplicates
from nntool.utils.subclasses import get_all_subclasses

# pylint: disable=wildcard-import,unused-wildcard-import
from nntool.quantization.float.quantizers import *
from nntool.quantization.multiplicative.quantizers import *
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.quantizers import *
from nntool.quantization.symmetric.quantizers import *
from nntool.quantization.unified_quantization_handler import QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)


def add_in_handlers(existing, new):
    for handler in new:
        if handler not in existing:
            existing.append(handler)


def get_quantization_handlers():
    """This sets up the handler map.
    The priority for schemes is:
    - If a handler is present for a class then for the declared scheme
      or any scheme if SCHEME is None it is matched
    - If a handler is present for a superclass of the class then for the
      declared scheme or any scheme if SCHEME is None it is matched
    - The default handler for the scheme is matched
    """
    handlers = {}
    schemes = set()
    # Collect all handlers
    for cls in get_all_subclasses(QuantizionHandler):
        if not cls.PARAMS_TYPE:
            continue
        if cls.SCHEME:
            schemes.add(cls.SCHEME)
        for params_cls in cls.PARAMS_TYPE:
            phandlers = handlers.setdefault(params_cls, {})
            pscheme_handlers = phandlers.setdefault(cls.SCHEME, [])
            pscheme_handlers.append(cls)

    # Iterate through all parameters and set a scheme
    for pclass in get_all_subclasses(NNNodeBase):
        # parameters does not define an op name
        if not pclass.CLS_OP_NAME:
            continue
        # see if we have any handlers for this class
        phandlers = handlers.setdefault(pclass, {})
        for scheme in schemes:
            # handlers for class and scheme are already present
            matched_handlers = phandlers.setdefault(scheme, [])
            for hpclass, class_handlers in handlers.items():
                if hpclass == '__default__':
                    continue
                if issubclass(pclass, hpclass):
                    # is subclass and scheme is present
                    if scheme in class_handlers:
                        add_in_handlers(matched_handlers,
                                        class_handlers[scheme])
                    # is subclass and all schemes match is present
                    if None in class_handlers:
                        add_in_handlers(matched_handlers,
                                        class_handlers[scheme])
            if not matched_handlers:
                # match the default scheme
                add_in_handlers(matched_handlers,
                                handlers['__default__'][scheme])
    del handlers['__default__']
    return handlers


def get_all_options():
    options = {}
    for handler in get_all_subclasses(QuantizionHandler):
        if not handler.OPTIONS:
            continue
        for opt_name, opt in handler.OPTIONS.items():
            optrec = options.setdefault(opt_name, {'handlers': set()})
            for k in opt.keys():
                if k in optrec:
                    if k == 'default' and opt[k] != optrec[k]:
                        defaults = optrec.setdefault('defaults', [optrec[k]])
                        if opt[k] not in defaults:
                            defaults.append(opt[k])
                    if k not in ['help', 'default'] and opt[k] != optrec[k]:
                        raise ValueError(
                            f'Quantization option {k} has different definitions')
                else:
                    optrec[k] = opt[k]
            optrec['handlers'].add(handler)
    return options


def get_all_options_by_params():
    options = {}
    for handler in get_all_subclasses(QuantizionHandler):
        if handler.PARAMS_TYPE is None or not handler.OPTIONS:
            continue
        for params in handler.PARAMS_TYPE:
            if params == '__default__':
                params = NNNodeBase
            options.setdefault(params, {}).update(handler.OPTIONS)
    for params in get_all_subclasses(NNNodeBase):
        poptions = {}
        for k, v in options.items():
            if issubclass(params, k):
                poptions.update(v)
        options[params] = poptions
    return options


def add_options_to_parser(parser: Cmd2ArgumentParser):
    opts = get_all_options()
    shortcuts = [opt['shortcut'] for opt in opts.values() if 'shortcut' in opt]
    duplicate_shortcuts = set(duplicates(shortcuts))
    assert not duplicate_shortcuts, f'the following shortcut commands are duplicates {",".join(duplicate_shortcuts)}'
    for opt_name, opt in opts.items():
        if opt.get('type') is None:
            continue # internal option
        if 'shortcut' in opt:
            names = [f'-{opt["shortcut"]}']
        else:
            names = []

        if opt.get('type') == bool:
            parse_options = {'help': opt.get('help'), 'action': 'store_true'}
            if opt.get('default') or (True in opt.get('defaults', [])):
                names = [f'--no_{opt_name}']
            if not opt.get('default') or (False in opt.get('defaults', [])):
                names.append(f'--{opt_name}')
            parser.add_argument(*names, **parse_options)
        else:
            parse_options = {k: opt.get(k) for k in [
                'type', 'choices', 'help']}
            names.append(f'--{opt_name}')
            parser.add_argument(*names, **parse_options)


def get_arg_or_default(args, opt_name, opt):
    arg = getattr(args, opt_name)
    if arg is None:
        arg = opt.get('default')
    return arg

def get_bool_attr(args, opt_name, opt):
    if hasattr(args, f'no_{opt_name}') and getattr(args, f'no_{opt_name}'):
        return False
    elif hasattr(args, f'{opt_name}') and getattr(args, f'{opt_name}'):
        return True
    if opt.get('defaults'):
        return None
    return opt.get('default')

def get_options_from_args(args):
    return {opt_name: get_bool_attr(args, opt_name, opt)
            if opt.get('type') == bool
            else get_arg_or_default(args, opt_name, opt)
            for opt_name, opt in get_all_options().items() if opt['type'] is not None}


def get_set_options_from_args(args):
    return {opt_name: get_bool_attr(args, opt_name, opt)
            if opt.get('type') == bool
            else getattr(args, opt_name)
            for opt_name, opt in get_all_options().items()
            if getattr(args, f'no_{opt_name}' if opt.get('type') == bool
                       and opt.get('default') else opt_name) is not None}


def match_qtype(constraint, qtype_or_dict):
    if qtype_or_dict is None:
        return True
    # constant always matches since it can always be mutated into a different type
    if qtype_or_dict.is_constant:
        return True
    for k, v in constraint.items():
        if isinstance(qtype_or_dict, dict):
            if k not in qtype_or_dict:
                return False
            val = qtype_or_dict[k]
        else:
            if not hasattr(qtype_or_dict, k):
                return False
            val = getattr(qtype_or_dict, k)

        if k == 'attr':
            assert isinstance(
                v, dict), 'expecting dictionary for attribut match'
            attr = qtype_or_dict.attr
            if not all(hasattr(attr, attr_k) and getattr(attr, attr_k) == attr_val
                       for attr_k, attr_val in v.items()):
                return False
        elif isinstance(v, type(lambda: None)):
            if not v(val):
                return False
        elif isinstance(v, set):
            val = val if isinstance(val, set) else set([val])
            if not (v & val):
                return False
        elif isinstance(val, np.ndarray):
            if not np.all(val == v):
                return False
        elif val != v:
            return False
    return True


def get_closest_qtype(constraint, qtype):
    if 'dtype' in constraint:
        dtype_constraint = constraint['dtype']
        if isinstance(dtype_constraint, set):
            return QType(dtype=next(dtype_constraint))
        return QType(dtype=dtype_constraint)
    return None


def match_in_out_qs(handler, name, in_out_qs, ignore_edge=None):

    key = f'{name.upper()}_CONSTRAINTS'
    constraints = getattr(handler, key)
    if not constraints:
        return True
    for idx, constraint in enumerate(constraints):
        # constant input edges always match since a constant can always be requantized
        if ignore_edge and ignore_edge[idx]:
            continue
        if idx >= len(in_out_qs):
            return False
        if isinstance(constraint, MatchAll):
            # match single constrain from here on
            return all(True if ignore_edge and ignore_edge[jdx] else
                       match_qtype(constraint.constraint, in_out_qs[jdx])
                       for jdx in range(idx, len(in_out_qs)))
        if not match_qtype(constraint, in_out_qs[idx]):
            return False
    return True


def match_in_qs(ignore_edge, handler, in_qs):
    return match_in_out_qs(handler, 'INPUT', in_qs, ignore_edge=ignore_edge)


def match_out_qs(handler, out_qs):
    return match_in_out_qs(handler, 'OUTPUT', out_qs)


def constrained_in_edges(G, name, in_qs, constraints):
    match_all = None
    for idx, edge in enumerate(G.indexed_in_edges(name)):
        if edge is None:
            continue
        if match_all:
            if match_qtype(constraints.constraint, in_qs[idx]):
                continue
            yield idx, edge, get_closest_qtype(constraints.constraint, in_qs[idx])
        else:
            if idx >= len(constraints):
                break
            if match_qtype(constraints[idx], in_qs[idx]):
                continue
            yield idx, edge, get_closest_qtype(constraints[idx], in_qs[idx])


def add_handler(handlers, scheme, scheme_handler):
    scheme_handlers = handlers.setdefault(scheme, [])
    if scheme_handler not in scheme_handlers:
        scheme_handlers.append(scheme_handler)


def check_constraints(ignore_edge, handlers, scheme_priorities, in_qs_constraint, out_qs_constraint):
    if handlers is None:
        return None
    # filter any handlers matching input and output constraints
    filtered_phandlers = {}
    for scheme, scheme_handlers in handlers.items():
        for scheme_handler in scheme_handlers:
            if in_qs_constraint and not match_in_qs(ignore_edge, scheme_handler, in_qs_constraint):
                continue
            if out_qs_constraint and not match_out_qs(scheme_handler, out_qs_constraint):
                continue
            add_handler(filtered_phandlers, scheme, scheme_handler)

    # select the handler with the highest scheme priority
    for scheme in scheme_priorities:
        if scheme in filtered_phandlers:
            selected_handlers = filtered_phandlers[scheme]
            break
    else:
        return None

    return sorted(selected_handlers, key=lambda x: x.PRIORITY)[0]


def check_option_constraints(handlers, params, options, **kwargs):
    filtered_phandlers = {}
    for scheme, scheme_handlers in handlers.items():
        for scheme_handler in scheme_handlers:
            if scheme_handler.OPTION_CONSTRAINT:
                for k, v in scheme_handler.OPTION_CONSTRAINT.items():
                    if k == '__function_constraint':
                        if not v(params, **kwargs):
                            break
                    else:
                        set_value = options.get(k)
                        if callable(v):
                            if not v(set_value, params, **kwargs):
                                break
                        elif isinstance(v, set):
                            if set_value not in v:
                                break
                        elif set_value != v:
                            break
                else:
                    add_handler(filtered_phandlers, scheme, scheme_handler)
            else:
                add_handler(filtered_phandlers, scheme, scheme_handler)
    return filtered_phandlers


def match_handler(graph, handlers, params, scheme_priorities, options,
                  in_qs_constraint=None, out_qs_constraint=None, **kwargs):
    # don't run match on constants or unconnected edges
    ignore_edge = [isinstance(edge.from_node, ConstantInputNode) if edge is not None else True
                   for edge in graph.indexed_in_edges(params.name)]
    # match the class
    params_handlers = handlers.get(params.__class__)
    params_handlers = check_option_constraints(
        params_handlers, params, options, **kwargs)
    handler = check_constraints(
        ignore_edge, params_handlers, scheme_priorities, in_qs_constraint, out_qs_constraint)

    return handler
