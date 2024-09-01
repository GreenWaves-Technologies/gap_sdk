from nntool.graph.types.activations import SoftMaxNode
from nntool.importer.tflite2.tflite_schema_head.SoftmaxOptions import SoftmaxOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .math_mixin import BasicMathMixin


@tflite_op("LOGSOFTMAX")
class LogSoftmax(BasicMathMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(SoftmaxOptions)
        # quantization loaded by softmax from tflite will not be correct since
        # we need it to be scaled to POW2 scaling. This is now taken care of after loading
        return super(LogSoftmax, cls)._common(
            node,
            params_class=SoftMaxNode,
            param_args={'beta': node_opts.Beta()},
            **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
