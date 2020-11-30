import inspect
import logging

LOG = logging.getLogger('nntool.' + __name__)


class QuantizionHandler(object):
    """ This class is base quantization handler class.

    All quantization handlers MUST put decorator @params_type to register corresponding params handled.
    """

    PARAMS_TYPE = None
    DOCUMENTATION = ''
    OPTIONS = []
    CAN_DEQUANTIZE = False

    @classmethod
    def check_cls(cls):
        if not cls.PARAMS_TYPE:
            LOG.warning(
                "%s doesn't have PARAMS_TYPE. "
                "Please use Handler.params_type decorator to register PARAMS_TYPE.",
                cls.__name__)

    @classmethod
    def _quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        raise NotImplementedError("no _quantize method implemented")

    @classmethod
    def get_options(cls, params, opts):
        popts = {option['name']: option['default']
                 for option in cls.OPTIONS}
        popts.update(opts.get(params, {}))
        return popts

    @classmethod
    def quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        """ Main method in handler. It will find corresponding versioned handle method,
        whose name format is `version_%d`. So prefix `version_` is reserved.
        DON'T use it for other purpose.

        :param node: NodeProto for backend.
        :param kwargs: Other args.
        :return: TensorflowNode for backend.
        """
        return cls._quantize(params, in_qs, out_dtype, stats, **kwargs)

    @classmethod
    def _dequantize(cls, params, qrec):
        raise ValueError("this handler cannot dequantize")

    @classmethod
    def dequantize(cls, params, qrec):
        cls._dequantize(params, qrec)

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
    def params_type(*args):
        return QuantizionHandler.property_register("PARAMS_TYPE", args)

    @staticmethod
    def documentation(doc):
        return QuantizionHandler.property_register("DOCUMENTATION", doc)

    @staticmethod
    def options(*args):
        return QuantizionHandler.property_register("OPTIONS", args)

    @staticmethod
    def can_dequantize(yes_no):
        return QuantizionHandler.property_register("CAN_DEQUANTIZE", yes_no)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco


#pylint: disable=invalid-name
params_type = QuantizionHandler.params_type
documentation = QuantizionHandler.documentation
options = QuantizionHandler.options
can_dequantize = QuantizionHandler.can_dequantize
