from .linear import FcParameters
from .conv2d import Conv2DParameters
from .pooling import PoolingParameters
from .others import (InputParameters, OutputParameters, MatrixAddParameters,
                     ConcatParameters, ActivationParameters, UpsampleParameters,
                     YoloParameters, SoftMaxParameters, FusionParameters,
                     ReshapeParameters, GroupParameters, UnconvertedOpParameters,
                     UnknownOpParameters, UnexecutableOpParameters, PadParameters,
                     TransposeParameters, Transposable)
from .base import (Parameters, FilterLikeParameters, FilterParameters, SingleInputAndOutput,
                   EdgeParameters, NNEdge, NodeOptions)

VALID_PARAMETERS = {
    "input": InputParameters,
    "output": OutputParameters,
    "conv2d": Conv2DParameters,
    "fc": FcParameters,
    "pool": PoolingParameters,
    "matrix_add": MatrixAddParameters,
    "concat": ConcatParameters,
    "activation": ActivationParameters,
    "upsample": UpsampleParameters,
    "yolo": YoloParameters,
    "softmax": SoftMaxParameters,
    "fusion": FusionParameters,
    "reshape": ReshapeParameters,
    "group": GroupParameters,
    "pad": PadParameters,
    "transpose": TransposeParameters,
    "unknown": UnknownOpParameters,
    "unconverted": UnconvertedOpParameters
}

def parameter_to_op(param):
    for i, j in VALID_PARAMETERS.items():
        if isinstance(param, j):
            return i
    raise KeyError()
