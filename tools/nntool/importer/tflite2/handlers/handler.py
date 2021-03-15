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

from importer.common.handler_options import HandlerOptions, handler_option

from .. import common

@handler_option('remove_quantize_ops', val_type=bool, default=True, desc="remove cast and quantization operations on non-constant tensors")
@handler_option('load_quantization', val_type=bool, default=False, desc="load TFLITE tensor quantization", shortcut='q')
class Handler(HandlerOptions):
    """ This class is base handler class.
    Base backend and frontend base handler class inherit this class.

    All operator handler MUST put decorator @tflite_op to register corresponding op.
    """

    TFLITE_OP = None
    TFLITE_CUSTOM_OP = None

    GRAPH_VERSION = 0
    PARTIAL_SUPPORT = False
    NOT_SUPPORTED = False
    PS_DESCRIPTION = ''

    @classmethod
    def check_cls(cls):
        if not cls.TFLITE_OP:
            common.LOG.warning(
                "%s doesn't have TFLITE_OP. "
                "Please use Handler.tflite_op decorator to register TFLITE_OP.",
                cls.__name__)

    @classmethod
    def handle(cls, node, **kwargs):
        """ Main method in handler. It will find corresponding versioned handle method,
        whose name format is `version_%d`. So prefix `version_` is reserved.
        DON'T use it for other purpose.

        :param node: Operator for backend.
        :param kwargs: Other args.
        :return: TensorflowNode for backend.
        """
        possible_versions = [ver for ver in cls.get_versions() if ver <= node.op_version]
        if possible_versions:
            handle_version = max(possible_versions)
            ver_handle = getattr(cls, "version_{}".format(handle_version), None)
    #pylint: disable=not-callable
            return ver_handle(node, **kwargs)

        raise ValueError(
            "{} version {} is not implemented.".format(node.op_type, node.op_version))

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
    def tflite_op(op):
        return Handler.property_register("TFLITE_OP", op)

    @staticmethod
    def tflite_custom_op(op):
        return Handler.property_register("TFLITE_CUSTOM_OP", op)

    @staticmethod
    def partial_support(ps):
        return Handler.property_register("PARTIAL_SUPPORT", ps)

    @staticmethod
    def not_supported(ps):
        return Handler.property_register("NOT_SUPPORTED", ps)

    @staticmethod
    def ps_description(psd):
        return Handler.property_register("PS_DESCRIPTION", psd)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco


tflite_op = Handler.tflite_op
tflite_custom_op = Handler.tflite_custom_op
partial_support = Handler.partial_support
not_supported = Handler.not_supported
ps_description = Handler.ps_description
property_register = Handler.property_register
