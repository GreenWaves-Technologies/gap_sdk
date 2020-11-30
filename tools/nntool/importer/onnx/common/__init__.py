import logging
import uuid
from numbers import Number

from onnx import TensorProto, mapping, helper

logger = logging.getLogger('nntool.' + __name__)

def get_unique_suffix():
    """ Get unique suffix by using first 8 chars from uuid.uuid4
    to make unique identity name.
    :return: Unique suffix string.
    """
    return str(uuid.uuid4())[:8]

def convert_onnx(attr_proto):
    """
    Convert an ONNX AttributeProto into an appropriate Python object
    for the type.
    NB: Tensor attribute gets returned as the straight proto.
    """
    if attr_proto.HasField('f'):
        return attr_proto.f
    elif attr_proto.HasField('i'):
        return attr_proto.i
    elif attr_proto.HasField('s'):
        return str(attr_proto.s, 'utf-8')
    elif attr_proto.HasField('t'):
        return attr_proto.t  # this is a proto!
    elif attr_proto.HasField('g'):
        return attr_proto.g
    elif attr_proto.floats:
        return list(attr_proto.floats)
    elif attr_proto.ints:
        return list(attr_proto.ints)
    elif attr_proto.strings:
        str_list = list(attr_proto.strings)
        str_list = list(map(lambda x: str(x, 'utf-8'), str_list))
        return str_list
    elif attr_proto.HasField('sparse_tensor'):
        return attr_proto.sparse_tensor
    else:
        raise ValueError("Unsupported ONNX attribute: {}".format(attr_proto))

def onnx2np(dtype):
    return mapping.TENSOR_TYPE_TO_NP_TYPE[dtype]

ONNX_ATTR_TRANSLATOR = {
    "axis": int,
    "axes": lambda x: [int(a) for a in x],
    "dtype": onnx2np,
    "keepdims": bool,
    "to": onnx2np,
}

def translate_onnx(key, val):
    return ONNX_ATTR_TRANSLATOR.get(key, lambda x: x)(val)
