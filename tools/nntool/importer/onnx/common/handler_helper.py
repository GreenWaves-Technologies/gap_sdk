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

from onnx import defs

from .. import common
# pylint: disable=wildcard-import,unused-wildcard-import
from ..handlers.backend import *  # noqa
from ..handlers.backend_handler import BackendHandler


def get_all_backend_handlers(opset_dict):
    """ Get a dict of all backend handler classes.
    e.g. {'domain': {'Abs': Abs handler class}, ...}, }.

    :param opset_dict: A dict of opset. e.g. {'domain': version, ...}
    :return: Dict.
    """
    handlers = {}
    for handler in BackendHandler.__subclasses__():
        handler.check_cls()

        domain = handler.DOMAIN
        version = opset_dict[domain] if domain in opset_dict else 1
        handler.VERSION = version

        since_version = 1
        if defs.has(handler.ONNX_OP, domain=handler.DOMAIN):
            try:
                since_version = defs.get_schema(
                    handler.ONNX_OP,
                    domain=handler.DOMAIN,
                    max_inclusive_version=version).since_version
            except RuntimeError:
                common.logger.debug("Fail to get since_version of %s in domain `%s` "
                                    "with max_inclusive_version=%s. Set to 1.",
                                    handler.ONNX_OP, handler.DOMAIN, version)
        else:
            common.logger.debug("Unknown op %s in domain `%s`.",
                                handler.ONNX_OP, handler.DOMAIN or "ai.onnx")
        handler.SINCE_VERSION = since_version
        handlers.setdefault(domain, {})[handler.ONNX_OP] = handler
    return handlers


def get_backend_coverage():
    """ Get backend coverage for document.

    :return: onnx_coverage: e.g. {'domain': {'ONNX_OP': [versions], ...}, ...}
    """

    onnx_coverage = {}
    experimental_op = set()
    for handler in BackendHandler.__subclasses__():
        handler.check_cls()

        versions = handler.get_versions()
        domain = handler.DOMAIN
        if getattr(handler, "EXPERIMENTAL", False):
            experimental_op.add(handler.ONNX_OP)
        _update_coverage(onnx_coverage, domain, handler.ONNX_OP, versions)
    return onnx_coverage, experimental_op


def _update_coverage(coverage, domain, key, versions):
    domain_coverage = coverage.setdefault(domain, {})
    vers = domain_coverage.get(key, [])
    vers.extend(versions)
    domain_coverage[key] = sorted(list(set(vers)))


def get_backend_partial_support_detail():
    ps_dict = {}
    opset_dict = dict([(defs.ONNX_DOMAIN, defs.onnx_opset_version())])
    handlers = get_all_backend_handlers(opset_dict)[defs.ONNX_DOMAIN]
    for op_name in handlers:
        if handlers[op_name].PARTIAL_SUPPORT:
            ps_dict[op_name] = handlers[op_name].PS_DESCRIPTION
    return ps_dict
