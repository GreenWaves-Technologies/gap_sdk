import logging
import uuid
from numbers import Number

from onnx import TensorProto, mapping, helper

LOG = logging.getLogger('nntool.' + __name__)

class TFLiteImportException(Exception):
    pass

class NoQuantizationError(Exception):
    pass

def check(condition, message):
    if not condition:
        raise TFLiteImportException(message)

def get_unique_suffix():
    """ Get unique suffix by using first 4 chars from uuid.uuid4
    to make unique identity name.
    :return: Unique suffix string.
    """
    return str(uuid.uuid4())[:4]
