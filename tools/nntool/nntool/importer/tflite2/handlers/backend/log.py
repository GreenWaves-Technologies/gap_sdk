import numpy as np
from nntool.graph.types.piecewise import LogOpNode, CosOpNode

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .math_mixin import BasicMathMixin


@tflite_op("LOG")
class Log(BasicMathMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        return super(Log, cls)._common(
            node,
            params_class=CosOpNode,
            constant_operation=np.log,
            **kwargs)