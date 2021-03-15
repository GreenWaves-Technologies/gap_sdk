from copy import deepcopy
import inspect
import logging

LOG = logging.getLogger('nntool.' + __name__)


class QuantizionHandler(object):
    """ This class is base quantization handler class.

    All quantization handlers MUST put decorator @params_type to register corresponding params handled.
    """

    PARAMS_TYPE = None
    DOCUMENTATION = ''
    OPTIONS = {}
    CAN_DEQUANTIZE = False
    DEFAULT_QREC = None # This should be implemented by the quantization handler base

    @classmethod
    def check_cls(cls):
        if not cls.PARAMS_TYPE:
            LOG.warning(
                "%s doesn't have PARAMS_TYPE. "
                "Please use Handler.params_type decorator to register PARAMS_TYPE.",
                cls.__name__)

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
    def get_versions(cls):
        """ Get all support versions.

        :return: Version list.
        """
        versions = []
        for k, _ in inspect.getmembers(cls, inspect.ismethod):
            if k.startswith("version_"):
                versions.append(int(k.replace("version_", "")))
        return versions

    @classmethod
    def get_default_options(cls):
        return {name: opt['default'] for name, opt in cls.OPTIONS.items()}

    @classmethod
    def check_input_quantization(cls, in_qs, **kwargs):
        return None

    @staticmethod
    def params_type(*args):
        return QuantizionHandler.property_register("PARAMS_TYPE", args)

    @staticmethod
    def documentation(doc):
        return QuantizionHandler.property_register("DOCUMENTATION", doc)

    @staticmethod
    def options(*args):
        def deco(cls):
            # copy the closest base class options so that we create
            # a new class variable on this class
            setattr(cls, "OPTIONS", deepcopy(getattr(cls, "OPTIONS")))
            # Now add / overide options
            getattr(cls, "OPTIONS").update({opt['name']: opt for opt in args})
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


#pylint: disable=invalid-name
params_type = QuantizionHandler.params_type
documentation = QuantizionHandler.documentation
options = QuantizionHandler.options
can_dequantize = QuantizionHandler.can_dequantize
