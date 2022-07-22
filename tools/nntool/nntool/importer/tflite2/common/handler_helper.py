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

# pylint: disable=wildcard-import,unused-wildcard-import
from nntool.importer.importer import ImportException
from ..handlers.backend import *  # noqa
from ..handlers.backend_handler import BackendHandler


def get_all_backend_handlers(graph_version=3):
    """ Get a dict of all backend handler classes.
    e.g. {'Abs': Abs handler class}, ...}

    :param graph_version: Version of TFLITE graph
    :return: Dict.
    """
    handlers = {}
    for handler in BackendHandler.__subclasses__():
        handler.check_cls()

        handler.GRAPH_VERSION = graph_version
        if handler.TFLITE_OP == 'CUSTOM':
            custom_handlers = handlers.setdefault('CUSTOM', {})
            custom_handlers[handler.TFLITE_CUSTOM_OP] = handler
        else:
            handlers[handler.TFLITE_OP] = handler
    return handlers


def get_backend_coverage():
    """ Get backend coverage for document.

    :return: onnx_coverage: e.g. {'TFLITE_OP': [versions], ...}
    """

    tflite_coverage = {}
    experimental_op = set()
    for handler in BackendHandler.__subclasses__():
        handler.check_cls()

        versions = handler.get_versions()
        if getattr(handler, "EXPERIMENTAL", False):
            experimental_op.add(handler.TFLITE_OP)
        _update_coverage(tflite_coverage, handler.TFLITE_OP, handler.TFLITE_CUSTOM_OP, versions)
    return tflite_coverage, experimental_op


def _update_coverage(coverage, key, custom_op_key, versions):
    if custom_op_key:
        key = "%s_%s"%(key, custom_op_key)
    vers = coverage.setdefault(key, [])
    vers.extend(versions)
    coverage[key] = sorted(list(set(vers)))


def get_backend_partial_support_detail(graph_version=3):
    ps_dict = {}
    handlers = get_all_backend_handlers(graph_version)
    for op_name in handlers:
        if op_name == 'CUSTOM':
            custom_handlers = handlers[op_name]
            for custom_op_name in custom_handlers:
                if custom_handlers[custom_op_name].PARTIAL_SUPPORT:
                    ps_dict["%s_%s"%(op_name, custom_op_name)] = custom_handlers[custom_op_name].PS_DESCRIPTION
        else:
            if handlers[op_name].PARTIAL_SUPPORT:
                ps_dict[op_name] = handlers[op_name].PS_DESCRIPTION
    return ps_dict
