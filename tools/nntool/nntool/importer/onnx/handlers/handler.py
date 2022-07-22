
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

import inspect

from onnx import defs
from onnx.backend.test.runner import BackendIsNotSupposedToImplementIt

from .. import common


class Handler(object):
    """ This class is base handler class.
    Base backend and frontend base handler class inherit this class.

    All operator handler MUST put decorator @onnx_op to register corresponding op.
    """

    ONNX_OP = None

    DOMAIN = defs.ONNX_DOMAIN
    VERSION = 0
    SINCE_VERSION = 0
    PARTIAL_SUPPORT = False
    PS_DESCRIPTION = ''
    CONSTANT_ONLY = False

    @classmethod
    def check_cls(cls):
        if not cls.ONNX_OP:
            common.logger.warning(
                "%s doesn't have ONNX_OP. "
                "Please use Handler.onnx_op decorator to register ONNX_OP.",
                cls.__name__)

    @classmethod
    def handle(cls, node, **kwargs):
        """ Main method in handler. It will find corresponding versioned handle method,
        whose name format is `version_%d`. So prefix `version_` is reserved.
        DON'T use it for other purpose.

        :param node: NodeProto for backend.
        :param kwargs: Other args.
        :return: TensorflowNode for backend.
        """
        ver_handle = getattr(cls, "version_{}".format(cls.SINCE_VERSION), None)
        if ver_handle:
            #pylint: disable=not-callable
            return ver_handle(node, **kwargs)

        raise BackendIsNotSupposedToImplementIt(
            "{} version {} is not implemented.".format(node.op_type, cls.SINCE_VERSION))

    @classmethod
    def get_versions(cls):
        """ Get all support versions.

        :return: Version list.
        """
        versions = []
        for k, _ in inspect.getmembers(cls, inspect.ismethod):
            if k.startswith("version_"):
                versions.append(int(k.replace("version_", "")))
        return versions

    @staticmethod
    def onnx_op(op):
        return Handler.property_register("ONNX_OP", op)

    @staticmethod
    def domain(dom):
        return Handler.property_register("DOMAIN", dom)

    @staticmethod
    def partial_support(is_partial):
        return Handler.property_register("PARTIAL_SUPPORT", is_partial)

    @staticmethod
    def ps_description(psd):
        return Handler.property_register("PS_DESCRIPTION", psd)

    @staticmethod
    def constant_only(is_partial):
        return Handler.property_register("CONSTANT_ONLY", is_partial)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco


#pylint: disable=invalid-name
domain = Handler.domain
onnx_op = Handler.onnx_op
partial_support = Handler.partial_support
ps_description = Handler.ps_description
constant_only = Handler.constant_only
property_register = Handler.property_register
